/****************************************************************************
* Copyright (c) 2024, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <TRUSTVect.h>
#include <TRUSTVect_tools.tpp>
#include <TRUSTTabs.h>
#include <View_Types.h>

// Ajout d'un flag par appel a end_timer peut etre couteux (creation d'une string)
#ifdef _OPENMP_TARGET
static bool timer=true;
#else
static bool timer=false;
#endif
#endif

namespace
{

/*! Small iterator class to scan blocks of a MD vector (blocks described by integer indices) or scan a single big block (trustIdType).
 * The important point: the operator*() always returns a trustIdType (TID), whether internal data is int* or trustIdType* :
 *   - in sequential, we can scan a whole Big array from start to end,
 *   - in parallel, we can scan chunks of the arrays as defined by MD_Vector_base2::blocs_items_to_compute_ (stored as int, on purpose, it should never be too big
 *   after Scatter)
 */
template<typename _SIZE_>
struct Block_Iter
{

  Block_Iter() = default;
  Block_Iter(const Block_Iter& other) = default;  // default copy ctor, will copy all members
  Block_Iter(const int * p) : int_ptr(p) {}
  Block_Iter(_SIZE_ s, _SIZE_ e) : start(s), end(e) {}

  _SIZE_ operator*() const
  {
    return start == -1 ? (_SIZE_)*int_ptr : start;  // potentially casting!
  }
  Block_Iter operator++(True_int)   // Postfix operator
  {
    Block_Iter ret = *this;
    if(int_ptr) int_ptr++;
    else std::swap(start, end);
    return ret;
  }
  bool empty() const { return int_ptr == nullptr && start == -1; }

  const int * int_ptr=nullptr;
  _SIZE_ start=-1;
  _SIZE_ end=-1;
};

/*! Determine which blocks of indices should be used to perform an operation.
 */
template <typename _SIZE_>
Block_Iter<_SIZE_> determine_blocks(Mp_vect_options opt, const MD_Vector& md, const _SIZE_ vect_size_tot, const int line_size, int& nblocs_left)
{
  nblocs_left = 1;
#ifndef LATATOOLS
  if (opt != VECT_ALL_ITEMS && md.non_nul() && Process::is_parallel())
    {
      assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
#if INT_is_64_ == 2
      // Should never use parallel patterns in 64b:
      assert( (!std::is_same<_SIZE_,std::int64_t>::value) );
#endif
      const ArrOfInt& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md->get_items_to_sum() : md->get_items_to_compute();
      assert(items_blocs.size_array() % 2 == 0);
      nblocs_left = items_blocs.size_array() >> 1;
      return Block_Iter<_SIZE_>(items_blocs.addr());  // iterator on int, but operator*() will cast and return a _SIZE_
    }
  else
#endif
    if (vect_size_tot > 0)
      {
        // Warning, if vect_size_tot is 0, line_size might be 0 too
        // Compute all data, in the vector (including virtual data), build a big bloc:
        nblocs_left = 1;
        return Block_Iter<_SIZE_>(0, vect_size_tot / line_size);   // iterator on a single (big) block
      }
  return Block_Iter<_SIZE_>();
}


} // end anonymous NS

template<typename _TYPE_, typename _SIZE_>
void ajoute_produit_scalaire(TRUSTVect<_TYPE_,_SIZE_>& resu, _TYPE_ alpha, const TRUSTVect<_TYPE_,_SIZE_>& vx, const TRUSTVect<_TYPE_,_SIZE_>& vy, Mp_vect_options opt)
{
#ifndef LATATOOLS
  ToDo_Kokkos("critical"); // Ne semble pas utilise...
  resu.ensureDataOnHost();
  vx.ensureDataOnHost();
  vy.ensureDataOnHost();
  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_,_SIZE_>& master_vect = resu;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size && vy.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot && vy.size_totale() == vect_size_tot); // this test is necessary if md is null
#ifndef LATATOOLS
  assert(vx.get_md_vector() == md && vy.get_md_vector() == md);
#endif
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty()) return;

  _TYPE_ *resu_base = resu.addr();
  const _TYPE_ *x_base = vx.addr();
  const _TYPE_ *y_base = vy.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_itr++)) * line_size, end_bloc = (*(bloc_itr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_ *resu_ptr = resu_base + begin_bloc;
      const _TYPE_ *x_ptr = x_base + begin_bloc;
      const _TYPE_ *y_ptr = y_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          const _TYPE_ y = *(y_ptr++);
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu += alpha * x * y;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
#endif // LATATOOLS
}

// Explicit instanciation for templates:
template void ajoute_produit_scalaire<double, int>(TRUSTVect<double, int>& resu, double alpha, const TRUSTVect<double, int>& vx, const TRUSTVect<double, int>& vy, Mp_vect_options opt);
template void ajoute_produit_scalaire<float, int>(TRUSTVect<float, int>& resu, float alpha, const TRUSTVect<float, int>& vx, const TRUSTVect<float, int>& vy, Mp_vect_options opt);


//Process bloc function used below in operation_speciale_tres_generic
//It is templated as a function of the in/out view location and execution spaces (Device/Host)
#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_, bool IS_MUL>
void operation_speciale_tres_generic_kernel(TRUSTVect<_TYPE_, _SIZE_>& resu, const TRUSTVect<_TYPE_, _SIZE_>& vx, int nblocs_left,
                                            Block_Iter<_SIZE_>& bloc_itr, const int line_size_vx, const int vect_size_tot, const int delta_line_size)
{
  auto vx_view= vx.template view_ro<ExecSpace>();
  auto resu_view= resu.template view_rw<ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_itr++)) * line_size_vx;
      const int end_bloc = (*(bloc_itr++)) * line_size_vx;
      const int count = end_bloc - begin_bloc;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      // Adjust pointers to indices
      const int resu_start_idx = begin_bloc * delta_line_size;
      const int x_start_idx = begin_bloc;

      Kokkos::RangePolicy<ExecSpace> policy(0, count);
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(const int i)
      {
        const _TYPE_ x = vx_view(x_start_idx + i);

        //The // for could be also placed there
        for (int j = 0; j < delta_line_size; ++j)
          {
            const int resu_idx = resu_start_idx + i * delta_line_size + j;
            if (IS_MUL)
              resu_view(resu_idx) *= x;
            else //If it's not MUL, it's DIV
              resu_view(resu_idx) *= ((_TYPE_)1 / x);
          }
      });
      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
    }
}
}
#endif

template<TYPE_OPERATION_VECT_SPEC_GENERIC _TYPE_OP_, typename _TYPE_, typename _SIZE_>
void operation_speciale_tres_generic(TRUSTVect<_TYPE_, _SIZE_>& resu, const TRUSTVect<_TYPE_,_SIZE_>& vx, Mp_vect_options opt)
{
#ifndef LATATOOLS

  // Check the nature of the operation
  static constexpr bool IS_MUL = (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_); //it's either MUL or DIV

  // get info for computation
  const int line_size = resu.line_size(), line_size_vx = vx.line_size(), vect_size_tot = resu.size_totale();
  const MD_Vector& md = resu.get_md_vector();
  // Le line_size du vecteur resu doit etre un multiple du line_size du vecteur vx
  assert(line_size > 0 && line_size_vx > 0 && line_size % line_size_vx == 0);
  const int delta_line_size = line_size / line_size_vx;
  assert(vx.size_totale() * delta_line_size == vect_size_tot); // this test is necessary if md is null
#ifndef LATATOOLS
  assert(vx.get_md_vector() == md);
#endif

  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty())
    return;

  bool kernelOnDevice = resu.checkDataOnDevice(vx);

  //Lauch computation with the execution space and view types as (template) parameters
  if (kernelOnDevice)
    operation_speciale_tres_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_, IS_MUL>(resu, vx, nblocs_left, bloc_itr, line_size_vx, vect_size_tot, delta_line_size);
  else
    operation_speciale_tres_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_, IS_MUL>(resu, vx, nblocs_left, bloc_itr, line_size_vx, vect_size_tot, delta_line_size);

#ifndef NDEBUG
  // In debug mode, put invalid values where data has not been computed
  invalidate_data(resu, opt);
#endif
  return;
#endif
}


// Explicit instanciation for templates:
template void operation_speciale_tres_generic<TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_, double, int>(TRUSTVect<double, int>& resu, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void operation_speciale_tres_generic<TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_, float, int>(TRUSTVect<float, int>& resu, const TRUSTVect<float, int>& vx, Mp_vect_options opt);
template void operation_speciale_tres_generic<TYPE_OPERATION_VECT_SPEC_GENERIC::DIV_, double, int>(TRUSTVect<double, int>& resu, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void operation_speciale_tres_generic<TYPE_OPERATION_VECT_SPEC_GENERIC::DIV_, float, int>(TRUSTVect<float, int>& resu, const TRUSTVect<float, int>& vx, Mp_vect_options opt);

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_, bool IS_ADD>
void operation_speciale_generic_kernel(TRUSTVect<_TYPE_, _SIZE_>& resu, const TRUSTVect<_TYPE_, _SIZE_>& vx, _TYPE_ alpha, int nblocs_left,
                                       Block_Iter<_SIZE_>& bloc_itr, const int vect_size_tot, const int line_size)
{
  auto vx_view= vx.template view_ro<ExecSpace>();
  auto resu_view= resu.template view_rw<ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;
      const int count = end_bloc - begin_bloc;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      Kokkos::RangePolicy<ExecSpace> policy(0, count);
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(const int i)
      {
        const _TYPE_ x = vx_view(begin_bloc + i);

        const int resu_idx = begin_bloc + i ;
        if (IS_ADD) //done at compile time
          resu_view(resu_idx) += alpha * x;
        else //If it's not ADD, it's SQUARE
          resu_view(resu_idx) += alpha * x * x;
      });
      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
    }
}
}
#endif

template <TYPE_OPERATION_VECT_SPEC _TYPE_OP_ ,typename _TYPE_, typename _SIZE_>
void ajoute_operation_speciale_generic(TRUSTVect<_TYPE_,_SIZE_>& resu, _TYPE_ alpha,
                                       const TRUSTVect<_TYPE_,_SIZE_>& vx, Mp_vect_options opt)
{
#ifndef LATATOOLS
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC::ADD_);

  const int line_size = resu.line_size();
  const _SIZE_ vect_size_tot = resu.size_totale();
  const MD_Vector& md = resu.get_md_vector();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
#ifndef LATATOOLS
  assert(vx.get_md_vector() == md);
#endif
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty()) return;

  bool kernelOnDevice = resu.checkDataOnDevice(vx);

  if (kernelOnDevice)
    operation_speciale_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_, IS_ADD>(resu, vx, alpha, nblocs_left, bloc_itr, vect_size_tot, line_size);
  else
    operation_speciale_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_, IS_ADD>(resu, vx, alpha, nblocs_left, bloc_itr, vect_size_tot, line_size);


#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
#endif
}

// Explicit instanciation for templates:
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::ADD_, double, int>(TRUSTVect<double, int>& resu, double alpha, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::ADD_, float, int>(TRUSTVect<float, int>& resu, float alpha, const TRUSTVect<float, int>& vx, Mp_vect_options opt);
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::SQUARE_, double, int>(TRUSTVect<double, int>& resu, double alpha, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::SQUARE_, float, int>(TRUSTVect<float, int>& resu, float alpha, const TRUSTVect<float, int>& vx, Mp_vect_options opt);

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_, TYPE_OPERATOR_VECT _TYPE_OP_>
void operator_vect_vect_generic_kernel(TRUSTVect<_TYPE_,_SIZE_>& resu, const TRUSTVect<_TYPE_, _SIZE_>& vx, int nblocs_left,
                                       Block_Iter<_SIZE_>& bloc_itr,  const int vect_size_tot, const int line_size)
{
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATOR_VECT::ADD_), IS_SUB = (_TYPE_OP_ == TYPE_OPERATOR_VECT::SUB_),
                        IS_MULT = (_TYPE_OP_ == TYPE_OPERATOR_VECT::MULT_), IS_DIV = (_TYPE_OP_ == TYPE_OPERATOR_VECT::DIV_),
                        IS_EGAL = (_TYPE_OP_ == TYPE_OPERATOR_VECT::EGAL_);

  auto vx_view= vx.template view_ro<ExecSpace>();
  auto resu_view= resu.template view_rw<ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;
      const int count = end_bloc - begin_bloc;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      Kokkos::RangePolicy<ExecSpace> policy(0, count);
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(const int i)
      {
        const _TYPE_ x = vx_view(begin_bloc + i);

        const int resu_idx = begin_bloc + i ;

        if (IS_ADD) resu_view(resu_idx) += x;
        if (IS_SUB) resu_view(resu_idx) -= x;
        if (IS_MULT) resu_view(resu_idx) *= x;
        if (IS_DIV) resu_view(resu_idx) /= x;
        if (IS_EGAL) resu_view(resu_idx) = x;
      });
      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
    }
}
}
#endif


template <typename _TYPE_, typename _SIZE_, TYPE_OPERATOR_VECT _TYPE_OP_>
void operator_vect_vect_generic(TRUSTVect<_TYPE_,_SIZE_>& resu, const TRUSTVect<_TYPE_,_SIZE_>& vx, Mp_vect_options opt)
{
#ifndef LATATOOLS
  const int line_size = resu.line_size();
  const _SIZE_ vect_size_tot = resu.size_totale();
  const MD_Vector& md = resu.get_md_vector();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
#ifndef LATATOOLS
  assert(vx.get_md_vector() == md);
#endif
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty()) return;

  bool kernelOnDevice = resu.checkDataOnDevice(vx);

  if (kernelOnDevice)
    operator_vect_vect_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_, _TYPE_OP_>(resu, vx, nblocs_left, bloc_itr, vect_size_tot, line_size);
  else
    operator_vect_vect_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_, _TYPE_OP_>(resu, vx, nblocs_left, bloc_itr, vect_size_tot, line_size);
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
#endif
}
// Explicit instanciation for templates:
template void operator_vect_vect_generic<double, int, TYPE_OPERATOR_VECT::ADD_>(TRUSTVect<double, int>& resu, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, int, TYPE_OPERATOR_VECT::ADD_>(TRUSTVect<int, int>& resu, const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, int, TYPE_OPERATOR_VECT::ADD_>(TRUSTVect<float, int>& resu, const TRUSTVect<float, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, int, TYPE_OPERATOR_VECT::SUB_>(TRUSTVect<double, int>& resu, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, int, TYPE_OPERATOR_VECT::SUB_>(TRUSTVect<int, int>& resu, const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, int, TYPE_OPERATOR_VECT::SUB_>(TRUSTVect<float, int>& resu, const TRUSTVect<float, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, int, TYPE_OPERATOR_VECT::MULT_>(TRUSTVect<double, int>& resu, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, int, TYPE_OPERATOR_VECT::MULT_>(TRUSTVect<int, int>& resu, const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, int, TYPE_OPERATOR_VECT::MULT_>(TRUSTVect<float, int>& resu, const TRUSTVect<float, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, int, TYPE_OPERATOR_VECT::DIV_>(TRUSTVect<double, int>& resu, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, int, TYPE_OPERATOR_VECT::DIV_>(TRUSTVect<int, int>& resu, const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, int, TYPE_OPERATOR_VECT::DIV_>(TRUSTVect<float, int>& resu, const TRUSTVect<float, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, int, TYPE_OPERATOR_VECT::EGAL_>(TRUSTVect<double, int>& resu, const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, int, TYPE_OPERATOR_VECT::EGAL_>(TRUSTVect<int, int>& resu, const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, int, TYPE_OPERATOR_VECT::EGAL_>(TRUSTVect<float, int>& resu, const TRUSTVect<float, int>& vx, Mp_vect_options opt);

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_, TYPE_OPERATOR_SINGLE _TYPE_OP_>
void operator_vect_single_generic_kernel(TRUSTVect<_TYPE_,_SIZE_>& resu, const _TYPE_ x, int nblocs_left,
                                         Block_Iter<_SIZE_>& bloc_itr,  const int vect_size_tot, const int line_size)
{
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::ADD_), IS_SUB = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::SUB_),
                        IS_MULT = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::MULT_), IS_DIV = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::DIV_), IS_EGAL = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::EGAL_),
                        IS_NEGATE = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::NEGATE_), IS_INV = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::INV_), IS_ABS = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::ABS_),
                        IS_SQRT = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::SQRT_), IS_SQUARE = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::SQUARE_);

  auto resu_view= resu.template view_rw<ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;
      const int count = end_bloc - begin_bloc;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      Kokkos::RangePolicy<ExecSpace> policy(0, count);
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(const int i)
      {
        const int resu_idx = begin_bloc + i ;

        if (IS_SUB) resu_view(resu_idx) -= x;
        if (IS_ADD) resu_view(resu_idx) += x;
        if (IS_MULT) resu_view(resu_idx) *= x;
        if (IS_EGAL) resu_view(resu_idx) = x;
        if (IS_NEGATE) resu_view(resu_idx) = -resu_view(resu_idx);
        if (IS_ABS) resu_view(resu_idx) = (_TYPE_) Kokkos::abs(resu_view(resu_idx));
        if (IS_SQRT) resu_view(resu_idx) = (_TYPE_) Kokkos::sqrt(resu_view(resu_idx));
        if (IS_SQUARE) resu_view(resu_idx)=resu_view(resu_idx)*resu_view(resu_idx);
        if (IS_DIV) resu_view(resu_idx) /= x;
        if (IS_INV) resu_view(resu_idx) = (_TYPE_) ((_TYPE_)1 /resu_view(resu_idx));
      });
      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
    }
}
}
#endif


template <typename _TYPE_, typename _SIZE_, TYPE_OPERATOR_SINGLE _TYPE_OP_ >
void operator_vect_single_generic(TRUSTVect<_TYPE_,_SIZE_>& resu, const _TYPE_ x, Mp_vect_options opt)
{
#ifndef LATATOOLS
  const int line_size = resu.line_size();
  const _SIZE_ vect_size_tot = resu.size_totale();
  const MD_Vector& md = resu.get_md_vector();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty()) return;

  bool kernelOnDevice = resu.checkDataOnDevice();

  if (kernelOnDevice)
    operator_vect_single_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_, _TYPE_OP_>(resu, x, nblocs_left, bloc_itr, vect_size_tot, line_size);
  else
    operator_vect_single_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_, _TYPE_OP_>(resu, x, nblocs_left, bloc_itr, vect_size_tot, line_size);

  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
#endif
}
// Explicit instanciation for templates:
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::DIV_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::DIV_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::DIV_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::EGAL_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::EGAL_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::EGAL_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::NEGATE_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::NEGATE_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::NEGATE_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::INV_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::INV_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::INV_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::ABS_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::ABS_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::ABS_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::SQRT_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::SQRT_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::SQRT_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, int, TYPE_OPERATOR_SINGLE::SQUARE_>(TRUSTVect<double, int>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, int, TYPE_OPERATOR_SINGLE::SQUARE_>(TRUSTVect<int, int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, int, TYPE_OPERATOR_SINGLE::SQUARE_>(TRUSTVect<float, int>& resu, const float x, Mp_vect_options opt);

#if INT_is_64_ == 2
template void operator_vect_single_generic<trustIdType, trustIdType, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<trustIdType, trustIdType>& resu, const trustIdType x, Mp_vect_options opt);
template void operator_vect_single_generic<int, trustIdType, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<int, trustIdType>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, trustIdType, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<float, trustIdType>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, trustIdType, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<double, trustIdType>& resu, const double x, Mp_vect_options opt);

template void operator_vect_single_generic<trustIdType, trustIdType, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<trustIdType, trustIdType>& resu, const trustIdType x, Mp_vect_options opt);
template void operator_vect_single_generic<int, trustIdType, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<int, trustIdType>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, trustIdType, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<float, trustIdType>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, trustIdType, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<double, trustIdType>& resu, const double x, Mp_vect_options opt);

template void operator_vect_single_generic<double, trustIdType, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<double, trustIdType>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<float, trustIdType, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<float, trustIdType>& resu, const float x, Mp_vect_options opt);


#endif

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_,typename _TYPE_RETURN_,  TYPE_OPERATION_VECT _TYPE_OP_>
void local_extrema_vect_generic_kernel(const TRUSTVect<_TYPE_,_SIZE_>& vx, int nblocs_left, Block_Iter<_SIZE_>& bloc_itr,
                                       const int vect_size_tot, const int line_size, _TYPE_& min_max_val, int& i_min_max)
{
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty()) return ;

  static constexpr bool IS_IMAX = (_TYPE_OP_ == TYPE_OPERATION_VECT::IMAX_), IS_IMIN = (_TYPE_OP_ == TYPE_OPERATION_VECT::IMIN_), IS_MAX = (_TYPE_OP_ == TYPE_OPERATION_VECT::MAX_),
                        IS_MIN = (_TYPE_OP_ == TYPE_OPERATION_VECT::MIN_), IS_MAX_ABS = (_TYPE_OP_ == TYPE_OPERATION_VECT::MAX_ABS_), IS_MIN_ABS = (_TYPE_OP_ == TYPE_OPERATION_VECT::MIN_ABS_);

  //For clearer code, is it max, is it a min, is it done on absolute values ?
  static constexpr bool IS_MAXS = (IS_MAX || IS_MAX_ABS || IS_IMAX);
  static constexpr bool IS_MINS = (IS_MIN || IS_MIN_ABS || IS_IMIN);
  static constexpr bool IS_ABS = (IS_MAX_ABS || IS_MIN_ABS);

  // Define the reducer, based on the reduction type
  using reducer = typename std::conditional<IS_MAXS, Kokkos::MaxLoc<_TYPE_, int>, Kokkos::MinLoc<_TYPE_, int>>::type;
  // Define the type of what the reducer will return ( a value + a index)
  using reducer_value_type  = typename reducer::value_type;

  if (not(IS_MAXS || IS_MINS)) {Process::exit("Wrong operation type in local_extrema_vect_generic_kernel");}

  auto vx_view= vx.template view_ro<ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;
      const int count = end_bloc - begin_bloc;

      //Asserts
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      //Define Policy
      Kokkos::RangePolicy<ExecSpace> policy(0, count);

      // Define the object in which the reduction is saved
      reducer_value_type bloc_min_max;

      //Reduction
      Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                              policy,
                              KOKKOS_LAMBDA(const int i, reducer_value_type& local_min_max)
      {
        const int resu_idx = begin_bloc + i ;

        _TYPE_ val = (IS_ABS) ? Kokkos::abs(vx_view(resu_idx)) : vx_view(resu_idx);

        if ( (IS_MAXS && val>local_min_max.val) || (IS_MINS && val<local_min_max.val) )
          {
            local_min_max.val=val;
            local_min_max.loc=i; // not begin_bloc + i ? This seems to be what was done before, although this is weird to me (dont we want the global index ?)
          }
      }
      ,reducer(bloc_min_max)); //Reduce in bloc_min_max

      //timer
      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);

      //Fence (necessary for now)
      Kokkos::fence();

      //Bloc-level reduction
      if ( (IS_MAXS && bloc_min_max.val > min_max_val) || (IS_MINS && bloc_min_max.val < min_max_val) )
        {
          min_max_val=bloc_min_max.val;
          i_min_max= bloc_min_max.loc;
        }
    }
}
}
#endif

template <typename _TYPE_, typename _SIZE_, typename _TYPE_RETURN_, TYPE_OPERATION_VECT _TYPE_OP_ >
_TYPE_RETURN_ local_extrema_vect_generic(const TRUSTVect<_TYPE_,_SIZE_>& vx, Mp_vect_options opt)
{
#ifndef LATATOOLS

  //Array info
  const int line_size = vx.line_size();
  const _SIZE_ vect_size_tot = vx.size_totale();
  const MD_Vector& md = vx.get_md_vector();

  //Asserts
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
#ifndef LATATOOLS
  assert(vx.get_md_vector() == md);
#endif

  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);

  //Initialize results
  _TYPE_ min_max_val = neutral_value<_TYPE_,_TYPE_OP_>(); // _TYPE_ et pas _TYPE_RETURN_ desole ...
  int i_min_max = -1 ; // seulement pour IMAX_ et IMIN_

  //Localize data
  bool kernelOnDevice = vx.checkDataOnDevice();

  //Compute reduction
  if (kernelOnDevice)
    local_extrema_vect_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_, _TYPE_RETURN_, _TYPE_OP_>(vx, nblocs_left, bloc_itr, vect_size_tot, line_size, min_max_val, i_min_max);
  else
    local_extrema_vect_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_, _TYPE_RETURN_, _TYPE_OP_>(vx, nblocs_left, bloc_itr, vect_size_tot, line_size, min_max_val, i_min_max);

  //Return index or value
  static constexpr bool IS_IMAX = (_TYPE_OP_ == TYPE_OPERATION_VECT::IMAX_), IS_IMIN = (_TYPE_OP_ == TYPE_OPERATION_VECT::IMIN_);

  return (IS_IMAX || IS_IMIN) ? (_TYPE_RETURN_)i_min_max : (_TYPE_RETURN_)min_max_val;

#else
  return (_TYPE_RETURN_)0; // For compil in latatools
#endif
}
// Explicit instanciation for templates:
template double local_extrema_vect_generic<double, int, double, TYPE_OPERATION_VECT::IMAX_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, int, double, TYPE_OPERATION_VECT::IMIN_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, int, double, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, int, double, TYPE_OPERATION_VECT::MIN_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, int, double, TYPE_OPERATION_VECT::MAX_ABS_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, int, double, TYPE_OPERATION_VECT::MIN_ABS_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, int, TYPE_OPERATION_VECT::IMAX_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, int, TYPE_OPERATION_VECT::IMIN_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, int, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, int, TYPE_OPERATION_VECT::MIN_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, int, TYPE_OPERATION_VECT::MAX_ABS_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, int, TYPE_OPERATION_VECT::MIN_ABS_>(const TRUSTVect<double, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, int, TYPE_OPERATION_VECT::IMAX_>(const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, int, TYPE_OPERATION_VECT::IMIN_>(const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, int, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, int, TYPE_OPERATION_VECT::MIN_>(const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, int, TYPE_OPERATION_VECT::MAX_ABS_>(const TRUSTVect<int, int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, int, TYPE_OPERATION_VECT::MIN_ABS_>(const TRUSTVect<int, int>& vx, Mp_vect_options opt);

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_, TYPE_OPERATION_VECT_BIS _TYPE_OP_>
void local_operations_vect_bis_generic_kernel(const TRUSTVect<_TYPE_,_SIZE_>& vx, int nblocs_left,
                                              Block_Iter<_SIZE_>& bloc_itr, const int vect_size_tot, const int line_size, _TYPE_& sum)
{
  static constexpr bool IS_SQUARE = (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::SQUARE_), IS_SUM = (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::SOMME_);

  auto vx_view= vx.template view_ro<ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;
      const int count = end_bloc - begin_bloc;

      //Asserts
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      //Define Policy
      Kokkos::RangePolicy<ExecSpace> policy(0, count);

      // Define the bloc sum
      _TYPE_ bloc_sum=0;

      //Reduction
      Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                              policy,
                              KOKKOS_LAMBDA(const int i, _TYPE_& local_sum)
      {
        const int resu_idx = begin_bloc + i ;
        if (IS_SUM)
          {
            local_sum += vx_view(resu_idx);
          }
        else if (IS_SQUARE)
          {
            local_sum += vx_view(resu_idx)*vx_view(resu_idx);
          }
      }
      ,bloc_sum); //Reduce in bloc_sum

      //timer
      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);

      //Fence (necessary for now)
      Kokkos::fence();

      //Bloc-level reduction
      sum += bloc_sum;
    }
}
}
#endif

template <typename _TYPE_, typename _SIZE_, TYPE_OPERATION_VECT_BIS _TYPE_OP_ >
_TYPE_ local_operations_vect_bis_generic(const TRUSTVect<_TYPE_,_SIZE_>& vx,Mp_vect_options opt)
{
#ifndef LATATOOLS
  _TYPE_ sum = 0;
  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_,_SIZE_>& master_vect = vx;
  const int line_size = master_vect.line_size();
  const _SIZE_ vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
#ifndef LATATOOLS
  assert(vx.get_md_vector() == md);
#endif
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr  = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty()) return sum;

  bool kernelOnDevice = vx.checkDataOnDevice();

  if (kernelOnDevice)
    local_operations_vect_bis_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_, _TYPE_OP_>(vx, nblocs_left, bloc_itr, vect_size_tot, line_size, sum);
  else
    local_operations_vect_bis_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_, _TYPE_OP_>(vx, nblocs_left, bloc_itr, vect_size_tot, line_size, sum);

  return sum;
#else
  return (_TYPE_)0; // For compil in latatools
#endif
}
// Explicit instanciation for templates:
template double local_operations_vect_bis_generic<double, int, TYPE_OPERATION_VECT_BIS::SQUARE_>(const TRUSTVect<double, int>& vx,Mp_vect_options opt);
template int local_operations_vect_bis_generic<int, int, TYPE_OPERATION_VECT_BIS::SQUARE_>(const TRUSTVect<int, int>& vx,Mp_vect_options opt);
template float local_operations_vect_bis_generic<float, int, TYPE_OPERATION_VECT_BIS::SQUARE_>(const TRUSTVect<float, int>& vx,Mp_vect_options opt);
template double local_operations_vect_bis_generic<double, int, TYPE_OPERATION_VECT_BIS::SOMME_>(const TRUSTVect<double, int>& vx,Mp_vect_options opt);
template int local_operations_vect_bis_generic<int, int, TYPE_OPERATION_VECT_BIS::SOMME_>(const TRUSTVect<int, int>& vx,Mp_vect_options opt);
template float local_operations_vect_bis_generic<float, int, TYPE_OPERATION_VECT_BIS::SOMME_>(const TRUSTVect<float, int>& vx,Mp_vect_options opt);

#if INT_is_64_ == 2
template double local_operations_vect_bis_generic<double, trustIdType, TYPE_OPERATION_VECT_BIS::SOMME_>(const TRUSTVect<double, trustIdType>& vx,Mp_vect_options opt);
#endif

// ==================================================================================================================================
// DEBUT code pour debug
#ifndef NDEBUG
// INVALID_SCALAR is used to fill arrays when values are not computed (virtual space might not be computed by operators).
// The value below probably triggers errors on parallel test cases but does not prevent from doing "useless" computations with it.
template <typename _TYPE_, typename _SIZE_>
void invalidate_data(TRUSTVect<_TYPE_,_SIZE_>& resu, Mp_vect_options opt)
{
#ifndef LATATOOLS
  _TYPE_ invalid = (_TYPE_)-987654321;

  const MD_Vector& md = resu.get_md_vector();
  const int line_size = resu.line_size();
  if (opt == VECT_ALL_ITEMS || (!md.non_nul())) return; // no invalid values
  assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
  const ArrOfInt& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md->get_items_to_sum() : md->get_items_to_compute();
  const int blocs_size = items_blocs.size_array();
  int i = 0;
  bool kernelOnDevice = resu.checkDataOnDevice();
  _TYPE_ *resu_ptr = computeOnTheDevice(resu, "", kernelOnDevice);
  start_gpu_timer();
  for (int blocs_idx = 0; blocs_idx < blocs_size; blocs_idx += 2) // process data until beginning of next bloc, or end of array
    {
      const int bloc_end = line_size * items_blocs[blocs_idx];
      #pragma omp target teams distribute parallel for if (kernelOnDevice)
      for (int count=i; count < bloc_end; count++) resu_ptr[count] = invalid;
      i = items_blocs[blocs_idx+1] * line_size;
    }
  const _SIZE_ bloc_end = resu.size_array(); // Process until end of vector
  #pragma omp target teams distribute parallel for if (kernelOnDevice)
  for (_SIZE_ count=i; count < bloc_end; count++) resu_ptr[count] = invalid;
  if (timer) end_gpu_timer(kernelOnDevice, "invalidate_data(x)");
#endif
}
// FIN code pour debug
// ==================================================================================================================================
// Explicit instanciation for templates:
template void invalidate_data<double>(TRUSTVect<double, int>& resu, Mp_vect_options opt);
template void invalidate_data<float>(TRUSTVect<float, int>& resu, Mp_vect_options opt);
template void invalidate_data<int>(TRUSTVect<int, int>& resu, Mp_vect_options opt);
#endif /* NDEBUG */

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_>
void local_prodscal_kernel(const TRUSTVect<_TYPE_,_SIZE_>& vx, const TRUSTVect<_TYPE_,_SIZE_>& vy, int nblocs_left,
                           Block_Iter<_SIZE_>& bloc_itr, const int vect_size_tot, const int line_size, _TYPE_& sum)
{
  auto vx_view= vx.template view_ro<ExecSpace>();
  auto vy_view= vy.template view_ro<ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;
      const int count = end_bloc - begin_bloc;

      //Asserts
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      //Define Policy
      Kokkos::RangePolicy<ExecSpace> policy(0, count);

      // Define the bloc sum
      _TYPE_ bloc_sum=0;

      //Reduction
      Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                              policy,
                              KOKKOS_LAMBDA(const int i, _TYPE_& local_sum)
      {
        const int resu_idx = begin_bloc + i ;
        local_sum += vx_view(resu_idx)*vy_view(resu_idx);
      }
      ,bloc_sum); //Reduce in bloc_sum

      //timer
      bool kernelOnDevice = is_default_exec_space<ExecSpace>;
      end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);

      //Fence (necessary for now)
      Kokkos::fence();

      //Bloc-level reduction
      sum += bloc_sum;
    }
}
}
#endif

template<typename _TYPE_, typename _SIZE_>
_TYPE_ local_prodscal(const TRUSTVect<_TYPE_,_SIZE_>& vx, const TRUSTVect<_TYPE_,_SIZE_>& vy, Mp_vect_options opt)
{
#ifndef LATATOOLS
  _TYPE_ sum = 0;

  const int line_size = vx.line_size();
  const _SIZE_ vect_size_tot = vx.size_totale();
  const MD_Vector& md = vx.get_md_vector();

  assert(vx.line_size() == line_size && vy.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot && vy.size_totale() == vect_size_tot); // this test is necessary if md is null
#ifndef LATATOOLS
  assert(vx.get_md_vector() == md && vy.get_md_vector() == md);
#endif
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left;
  Block_Iter<_SIZE_> bloc_itr  = ::determine_blocks(opt, md, vect_size_tot, line_size, nblocs_left);
  // Shortcut for empty arrays (avoid case line_size == 0)
  if (bloc_itr.empty()) return sum;

  bool kernelOnDevice = const_cast<TRUSTVect<_TYPE_,_SIZE_>&>(vx).checkDataOnDevice(vy);

  if (kernelOnDevice)
    local_prodscal_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_>(vx, vy, nblocs_left, bloc_itr, vect_size_tot,line_size, sum);
  else
    local_prodscal_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_>(vx, vy, nblocs_left, bloc_itr, vect_size_tot,line_size, sum);

  return sum;

#else
  return (_TYPE_)0; // For compil in latatools
#endif
}
// Explicit instanciation for templates:
template double local_prodscal(const TRUSTVect<double, int>& vx, const TRUSTVect<double, int>& vy, Mp_vect_options opt);
template float local_prodscal(const TRUSTVect<float, int>& vx, const TRUSTVect<float, int>& vy, Mp_vect_options opt);
