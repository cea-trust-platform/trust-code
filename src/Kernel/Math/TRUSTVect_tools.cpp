/****************************************************************************
* Copyright (c) 2025, CEA
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
#include <MD_Vector_seq.h>

/**************************************************************************************/
/* Warning ! This kernels are critical for performance into several TRUST applications !
 * Do not change implementation without using performance regression testing !
 * You are warned.
 */

/*! Determine which blocks of indices should be used to perform an operation.
 */
template <typename _SIZE_>
Block_Iter<_SIZE_> determine_blocks(Mp_vect_options opt, const MD_Vector& md, const _SIZE_ vect_size_tot, const int line_size, int& nblocs_left)
{
  nblocs_left = 1;
#ifndef LATATOOLS
  // Should we use the members bloc_items_to_* of the MD_Vector_* classes ?
  // (this must be avoided when
  //    - md.valeur() is not defined (md is nul)
  //    - we want all items (VECT_SEQUENTIAL_ITEMS)
  //    - md.valeur() is a MD_Vector_seq
  //    - or md.valeur() is a MD_Vector_composite when it is an aggregation of several MD_Vector_seq)
  const bool use_blocks = (opt != VECT_ALL_ITEMS && md.non_nul() && md->use_blocks());

  if (use_blocks)
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

// Explicit instanciations
template Block_Iter<int> determine_blocks(Mp_vect_options opt, const MD_Vector& md, const int vect_size_tot, const int line_size, int& nblocs_left);
#if INT_is_64_ == 2
template Block_Iter<trustIdType> determine_blocks(Mp_vect_options opt, const MD_Vector& md, const trustIdType vect_size_tot, const int line_size, int& nblocs_left);
#endif


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
                                            Block_Iter<_SIZE_>& bloc_itr, const int line_size_vx, const _SIZE_ vect_size_tot, const int delta_line_size)
{
  auto vx_view= vx.template view_ro<1, ExecSpace>().data();
  auto resu_view= resu.template view_rw<1, ExecSpace>().data();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_itr++)) * line_size_vx;
      const int end_bloc = (*(bloc_itr++)) * line_size_vx;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      // Adjust pointers to indices
      const int resu_start_idx = begin_bloc * delta_line_size;

      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_for(policy, KOKKOS_LAMBDA(const int i)
      {
        const _TYPE_ x = vx_view[i];

        //The // for could be also placed there
        for (int j = 0; j < delta_line_size; ++j)
          {
            const int resu_idx = resu_start_idx + i * delta_line_size + j;
            if (IS_MUL)
              resu_view[resu_idx] *= x;
            else //If it's not MUL, it's DIV
              resu_view[resu_idx] *= ((_TYPE_)1 / x);
          }
      });
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);
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
  assert(vx.get_md_vector() == md);

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
#else
  Cerr << "Error! operation_speciale_tres_generic can't be called in your project!" << finl;
  Process::exit();
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
                                       Block_Iter<_SIZE_>& bloc_itr, const _SIZE_ vect_size_tot, const int line_size)
{
  auto vx_view= vx.template view_ro<1, ExecSpace>().data();
  auto resu_view= resu.template view_rw<1, ExecSpace>().data();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_for(policy, KOKKOS_LAMBDA(const int i)
      {
        const _TYPE_ x = vx_view[i];

        if (IS_ADD) //done at compile time
          resu_view[i] += alpha * x;
        else //If it's not ADD, it's SQUARE
          resu_view[i] += alpha * x * x;
      });
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);
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
  assert(vx.get_md_vector() == md);
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
#else
  Cerr << "Error! ajoute_operation_speciale_generic can't be called in your project!" << finl;
  Process::exit();
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
                                       Block_Iter<_SIZE_>& bloc_itr,  const _SIZE_ vect_size_tot, const int line_size)
{
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATOR_VECT::ADD_), IS_SUB = (_TYPE_OP_ == TYPE_OPERATOR_VECT::SUB_),
                        IS_MULT = (_TYPE_OP_ == TYPE_OPERATOR_VECT::MULT_), IS_DIV = (_TYPE_OP_ == TYPE_OPERATOR_VECT::DIV_),
                        IS_EGAL = (_TYPE_OP_ == TYPE_OPERATOR_VECT::EGAL_);

#ifdef TRUST_USE_GPU
  auto vx_view= vx.template view_ro<1, ExecSpace>().data();
  auto resu_view= resu.template view_rw<1, ExecSpace>().data();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_for(policy, KOKKOS_LAMBDA(const _SIZE_ i)
      {
        const _TYPE_ x = vx_view[i];
        if (IS_ADD) resu_view[i] += x;
        if (IS_SUB) resu_view[i] -= x;
        if (IS_MULT) resu_view[i] *= x;
        if (IS_DIV) resu_view[i] /= x;
        if (IS_EGAL) resu_view[i] = x;
      });
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);
    }
#else
  // Need to keep C++ optimized (pointer) implementation for PolyMAC in Flica5
  _TYPE_ *resu_base = resu.data();
  const _TYPE_ *x_base = vx.data();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size, end_bloc = (*(bloc_itr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_ *resu_ptr = resu_base + begin_bloc;
      const _TYPE_ *x_ptr = x_base + begin_bloc;
      for (_SIZE_ count = 0; count < end_bloc - begin_bloc ; count++)
        {
          const _TYPE_& x = x_ptr[count];
          _TYPE_ &p_resu = resu_ptr[count];
          if (IS_ADD) p_resu += x;
          if (IS_SUB) p_resu -= x;
          if (IS_MULT) p_resu *= x;
          if (IS_EGAL) p_resu = x;
          if (IS_DIV) p_resu /= x;
        }
    }
#endif
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
  assert(vx.get_md_vector() == md);
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
#else
  Cerr << "Error! operator_vect_vect_generic can't be called in your project!" << finl;
  Process::exit();
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
                                         Block_Iter<_SIZE_>& bloc_itr,  const _SIZE_ vect_size_tot, const int line_size)
{
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::ADD_), IS_SUB = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::SUB_),
                        IS_MULT = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::MULT_), IS_DIV = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::DIV_), IS_EGAL = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::EGAL_),
                        IS_NEGATE = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::NEGATE_), IS_INV = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::INV_), IS_ABS = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::ABS_),
                        IS_SQRT = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::SQRT_), IS_SQUARE = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::SQUARE_);

  auto resu_view= resu.template view_rw<1, ExecSpace>().data();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;

      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_for(policy, KOKKOS_LAMBDA(const _SIZE_ i)
      {
        if (IS_SUB) resu_view[i] -= x;
        if (IS_ADD) resu_view[i] += x;
        if (IS_MULT) resu_view[i] *= x;
        if (IS_EGAL) resu_view[i] = x;
        if (IS_NEGATE) resu_view[i] = -resu_view[i];
        if (IS_ABS) resu_view[i] = (_TYPE_) Kokkos::abs(resu_view[i]);
        if (IS_SQRT) resu_view[i] = (_TYPE_) Kokkos::sqrt(resu_view[i]);
        if (IS_SQUARE) resu_view[i] = resu_view[i]*resu_view[i];
        if (IS_DIV) resu_view[i] /= x;
        if (IS_INV) resu_view[i] = (_TYPE_) ((_TYPE_)1 /resu_view[i]);
      });
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);
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
#else
  Cerr << "Error! operator_vect_single_generic can't be called in your project!" << finl;
  Process::exit();
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

template void operator_vect_single_generic<double, trustIdType, TYPE_OPERATOR_SINGLE::DIV_>(TRUSTVect<double, trustIdType>& resu, const double x, Mp_vect_options opt);


#endif

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_,typename _TYPE_RETURN_,  TYPE_OPERATION_VECT _TYPE_OP_>
void local_extrema_vect_generic_kernel(const TRUSTVect<_TYPE_,_SIZE_>& vx, int nblocs_left, Block_Iter<_SIZE_>& bloc_itr,
                                       const _SIZE_ vect_size_tot, const int line_size, _TYPE_& min_max_val, int& i_min_max)
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

  auto vx_view= vx.template view_ro<1, ExecSpace>().data();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;

      //Asserts
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      //Define Policy
      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);

      // Define the object in which the reduction is saved
      reducer_value_type bloc_min_max;

      //Reduction
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_reduce(policy,
                              KOKKOS_LAMBDA(const int i, reducer_value_type& local_min_max)
      {
        const _TYPE_ val = (IS_ABS) ? Kokkos::abs(vx_view[i]) : vx_view[i];

        if ( (IS_MAXS && val>local_min_max.val) || (IS_MINS && val<local_min_max.val) )
          {
            local_min_max.val=val;
            local_min_max.loc=i; // not begin_bloc + i ? This seems to be what was done before, although this is weird to me (dont we want the global index ?)
          }
      }
      ,reducer(bloc_min_max)); //Reduce in bloc_min_max
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);

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
  assert(vx.get_md_vector() == md);

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
  Cerr << "Error! local_extrema_vect_generic can't be called in your project!" << finl;
  Process::exit();
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

#if INT_is_64_ == 2
template int local_extrema_vect_generic<int, trustIdType, int, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<int, trustIdType>& vx, Mp_vect_options opt);
template trustIdType local_extrema_vect_generic<trustIdType, trustIdType, trustIdType, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<trustIdType, trustIdType>& vx, Mp_vect_options opt);
#endif

#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_, TYPE_OPERATION_VECT_BIS _TYPE_OP_>
void local_operations_vect_bis_generic_kernel(const TRUSTVect<_TYPE_,_SIZE_>& vx, int nblocs_left,
                                              Block_Iter<_SIZE_>& bloc_itr, const _SIZE_ vect_size_tot, const int line_size, _TYPE_& sum)
{
  static constexpr bool IS_SQUARE = (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::SQUARE_), IS_SUM = (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::SOMME_);
  // Performance important point for TRUSTArray dynamic kernel to have serial mode performance:
  // Use pointer access into Kokkos loop with [] and getting raw pointer to view with .data() !
  auto vx_view= vx.template view_ro<1, ExecSpace>().data();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;

      //Asserts
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      //Define Policy
      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);

      // Define the bloc sum
      _TYPE_ bloc_sum=0;

      //Reduction
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_reduce(policy, KOKKOS_LAMBDA(const _SIZE_ i, _TYPE_& local_sum)
      {
        const _TYPE_ x = vx_view[i];
        if (IS_SQUARE) local_sum += x * x;
        if (IS_SUM) local_sum += x;
      }
      ,bloc_sum); //Reduce in bloc_sum
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);

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
  assert(vx.get_md_vector() == md);
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
  Cerr << "Error! local_operations_vect_bis_generic can't be called in your project!" << finl;
  Process::exit();
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
// BEGIN code for debug
#ifndef NDEBUG
// INVALID_SCALAR is used to fill arrays when values are not computed (virtual space might not be computed by operators).
// The value below probably triggers errors on parallel test cases but does not prevent from doing "useless" computations with it.
#ifndef LATATOOLS
namespace
{
template<typename ExecSpace, typename _TYPE_, typename _SIZE_>
void invalidate_data_kernel(TRUSTVect<_TYPE_,_SIZE_>& resu,
                            const ArrOfInt& items_blocs, const int line_size, const int blocs_size)
{
  _TYPE_ invalid = (_TYPE_)-987654321;
  auto resu_view= resu.template view_rw<1, ExecSpace>().data();

  int i = 0;
  for (int blocs_idx = 0; blocs_idx < blocs_size; blocs_idx += 2) // process data until beginning of next bloc, or end of array
    {
      const int bloc_end = line_size * items_blocs[blocs_idx];
      //Define Policy
      Kokkos::RangePolicy<ExecSpace> policy(i, bloc_end);
      //Loop
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_for(policy,KOKKOS_LAMBDA(const int count)
      {
        resu_view[count]=invalid;
      });
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);
      i = items_blocs[blocs_idx+1] * line_size;
    }
  const _SIZE_ bloc_end = resu.size_array(); // Process until end of vector
  //Define Policy
  Kokkos::RangePolicy<ExecSpace> policy(i, bloc_end);
  //Loop
  if (timer) start_gpu_timer(__KERNEL_NAME__);
  Kokkos::parallel_for(policy,KOKKOS_LAMBDA(const int count)
  {
    resu_view[count]=invalid;
  });
  if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);
}
}
#endif

template <typename _TYPE_, typename _SIZE_>
void invalidate_data(TRUSTVect<_TYPE_,_SIZE_>& resu, Mp_vect_options opt)
{
#ifndef LATATOOLS
  if (Process::is_sequential()) return; // no invalid values in sequential

  const MD_Vector& md = resu.get_md_vector();
  const int line_size = resu.line_size();
  if (opt == VECT_ALL_ITEMS || (!md.non_nul())) return; // no invalid values
  assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
  const ArrOfInt& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md->get_items_to_sum() : md->get_items_to_compute();
  const int blocs_size = items_blocs.size_array();

  bool kernelOnDevice = resu.checkDataOnDevice();

  if (kernelOnDevice)
    invalidate_data_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _SIZE_>(resu, items_blocs, line_size, blocs_size);
  else
    invalidate_data_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _SIZE_>(resu, items_blocs, line_size, blocs_size);
#else
  Cerr << "Error! invalidate_data can't be called in your project!" << finl;
  Process::exit();
#endif
}
//END code for debug
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
  auto vx_view= vx.template view_ro<1, ExecSpace>().data();
  auto vy_view= vy.template view_ro<1, ExecSpace>().data();

  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const _SIZE_ begin_bloc = (*(bloc_itr++)) * line_size;
      const _SIZE_ end_bloc = (*(bloc_itr++)) * line_size;

      //Asserts
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);

      //Define Policy
      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);

      // Define the bloc sum
      _TYPE_ bloc_sum=0;

      //Reduction
      if (timer) start_gpu_timer(__KERNEL_NAME__);
      Kokkos::parallel_reduce(policy, KOKKOS_LAMBDA(const _SIZE_ i, _TYPE_& local_sum)
      {
        local_sum += vx_view[i]*vy_view[i];
      }
      , Kokkos::Sum<_TYPE_>(bloc_sum)); //Reduce in bloc_sum

      //timer
      if (timer) end_gpu_timer(__KERNEL_NAME__, is_default_exec_space<ExecSpace>);

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
  assert(vx.get_md_vector() == md && vy.get_md_vector() == md);
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
  Cerr << "Error! local_prodscal can't be called in your project!" << finl;
  Process::exit();
  return (_TYPE_)0; // For compil in latatools
#endif
}
// Explicit instanciation for templates:
template double local_prodscal(const TRUSTVect<double, int>& vx, const TRUSTVect<double, int>& vy, Mp_vect_options opt);
template float local_prodscal(const TRUSTVect<float, int>& vx, const TRUSTVect<float, int>& vy, Mp_vect_options opt);
