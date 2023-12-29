/****************************************************************************
* Copyright (c) 2023, CEA
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

// Ajout d'un flag par appel a end_timer peut etre couteux (creation d'une string)
#ifdef _OPENMP
static bool timer=true;
#else
static bool timer=false;
#endif

template <TYPE_OPERATION_VECT_SPEC _TYPE_OP_ ,typename _TYPE_>
void ajoute_operation_speciale_generic(TRUSTVect<_TYPE_>& resu, _TYPE_ alpha, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt)
{
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC::ADD_), IS_CARRE = (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC::CARRE_);

  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1, one_bloc[2];
  const int *bloc_ptr;
  if (opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
      assert(items_blocs.size_array() % 2 == 0);
      nblocs_left = items_blocs.size_array() >> 1;
      bloc_ptr = items_blocs.addr();
    }
  else if (vect_size_tot > 0)
    {
      // attention, si vect_size_tot est nul, line_size a le droit d'etre nul
      // Compute all data, in the vector (including virtual data), build a big bloc:
      nblocs_left = 1;
      bloc_ptr = one_bloc;
      one_bloc[0] = 0;
      one_bloc[1] = vect_size_tot / line_size;
    }
  else // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
    return;

  bool kernelOnDevice = resu.checkDataOnDevice(vx);
  _TYPE_ *resu_base = computeOnTheDevice(resu, "", kernelOnDevice);
  const _TYPE_ *x_base = mapToDevice(vx, "", kernelOnDevice);
  start_timer();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size, end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_ *resu_ptr = resu_base + begin_bloc;
      const _TYPE_ *x_ptr = x_base + begin_bloc;
      #pragma omp target teams distribute parallel for if (kernelOnDevice)
      for (int count = 0; count < end_bloc - begin_bloc ; count++)
        {
          const _TYPE_ x = x_ptr[count];
          _TYPE_& p_resu = resu_ptr[count];

          if (IS_ADD) p_resu += alpha * x;
          if (IS_CARRE) p_resu += alpha * x * x;
        }
    }
  if (timer) end_timer(kernelOnDevice, "ajoute_operation_speciale_generic(x,alpha,y");
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
}
// Explicit instanciation for templates:
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::ADD_, double>(TRUSTVect<double>& resu, double alpha, const TRUSTVect<double>& vx, Mp_vect_options opt);
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::ADD_, float>(TRUSTVect<float>& resu, float alpha, const TRUSTVect<float>& vx, Mp_vect_options opt);
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::CARRE_, double>(TRUSTVect<double>& resu, double alpha, const TRUSTVect<double>& vx, Mp_vect_options opt);
template void ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::CARRE_, float>(TRUSTVect<float>& resu, float alpha, const TRUSTVect<float>& vx, Mp_vect_options opt);

template <typename _TYPE_, TYPE_OPERATOR_VECT _TYPE_OP_ >
void operator_vect_vect_generic(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt)
{
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATOR_VECT::ADD_), IS_SUB = (_TYPE_OP_ == TYPE_OPERATOR_VECT::SUB_),
                        IS_MULT = (_TYPE_OP_ == TYPE_OPERATOR_VECT::MULT_), IS_DIV = (_TYPE_OP_ == TYPE_OPERATOR_VECT::DIV_), IS_EGAL = (_TYPE_OP_ == TYPE_OPERATOR_VECT::EGAL_);

  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left_size = 1, one_bloc[2];
  const int *bloc_ptr;
  if (opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
      assert(items_blocs.size_array() % 2 == 0);
      nblocs_left_size = items_blocs.size_array() >> 1;
      bloc_ptr = items_blocs.addr();
    }
  else if (vect_size_tot > 0)
    {
      // attention, si vect_size_tot est nul, line_size a le droit d'etre nul. Compute all data, in the vector (including virtual data), build a big bloc:
      nblocs_left_size = 1;
      bloc_ptr = one_bloc;
      one_bloc[0] = 0;
      one_bloc[1] = vect_size_tot / line_size;
    }
  else // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
    return;
  bool kernelOnDevice = resu.checkDataOnDevice(vx);
  _TYPE_ *resu_base = computeOnTheDevice(resu, "", kernelOnDevice);
  const _TYPE_ *x_base = mapToDevice(vx, "", kernelOnDevice);
  start_timer();
  for (int nblocs_left=nblocs_left_size; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size, end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_ *resu_ptr = resu_base + begin_bloc;
      const _TYPE_ *x_ptr = x_base + begin_bloc;
      #pragma omp target teams distribute parallel for if (kernelOnDevice)
      for (int count = 0; count < end_bloc - begin_bloc ; count++)
        {
          const _TYPE_& x = x_ptr[count];
          _TYPE_ &p_resu = resu_ptr[count];
          if (IS_ADD) p_resu += x;
          if (IS_SUB) p_resu -= x;
          if (IS_MULT) p_resu *= x;
          if (IS_EGAL) p_resu = x;
          if (IS_DIV)
            {
#ifndef _OPENMP
              if (x == 0.) error_divide(__func__);
#endif
              p_resu /= x;
            }
          //printf("After resu %p %p %f\n,",(void*)&x, (void*)&p_resu, p_resu);
        }
    }
  if (timer) end_timer(kernelOnDevice, "operator_vect_vect_generic(x,y)");
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
}
// Explicit instanciation for templates:
template void operator_vect_vect_generic<double, TYPE_OPERATOR_VECT::ADD_>(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, TYPE_OPERATOR_VECT::ADD_>(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, TYPE_OPERATOR_VECT::ADD_>(TRUSTVect<float>& resu, const TRUSTVect<float>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, TYPE_OPERATOR_VECT::SUB_>(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, TYPE_OPERATOR_VECT::SUB_>(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, TYPE_OPERATOR_VECT::SUB_>(TRUSTVect<float>& resu, const TRUSTVect<float>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, TYPE_OPERATOR_VECT::MULT_>(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, TYPE_OPERATOR_VECT::MULT_>(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, TYPE_OPERATOR_VECT::MULT_>(TRUSTVect<float>& resu, const TRUSTVect<float>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, TYPE_OPERATOR_VECT::DIV_>(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, TYPE_OPERATOR_VECT::DIV_>(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, TYPE_OPERATOR_VECT::DIV_>(TRUSTVect<float>& resu, const TRUSTVect<float>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<double, TYPE_OPERATOR_VECT::EGAL_>(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<int, TYPE_OPERATOR_VECT::EGAL_>(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt);
template void operator_vect_vect_generic<float, TYPE_OPERATOR_VECT::EGAL_>(TRUSTVect<float>& resu, const TRUSTVect<float>& vx, Mp_vect_options opt);

template <typename _TYPE_, TYPE_OPERATOR_SINGLE _TYPE_OP_ >
void operator_vect_single_generic(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt)
{
  static constexpr bool IS_ADD = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::ADD_), IS_SUB = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::SUB_),
                        IS_MULT = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::MULT_), IS_DIV = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::DIV_), IS_EGAL = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::EGAL_),
                        IS_NEGATE = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::NEGATE_), IS_INV = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::INV_), IS_ABS = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::ABS_),
                        IS_RACINE_CARRE = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::RACINE_CARRE_), IS_CARRE = (_TYPE_OP_ == TYPE_OPERATOR_SINGLE::CARRE_);

  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1, one_bloc[2];
  const int *bloc_ptr;
  if (opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
      assert(items_blocs.size_array() % 2 == 0);
      nblocs_left = items_blocs.size_array() >> 1;
      bloc_ptr = items_blocs.addr();
    }
  else if (vect_size_tot > 0)
    {
      // attention, si vect_size_tot est nul, line_size a le droit d'etre nul
      // Compute all data, in the vector (including virtual data), build a big bloc:
      nblocs_left = 1;
      bloc_ptr = one_bloc;
      one_bloc[0] = 0;
      one_bloc[1] = vect_size_tot / line_size;
    }
  else // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
    return;

  bool kernelOnDevice = resu.checkDataOnDevice();
  _TYPE_ *resu_base = computeOnTheDevice(resu, "", kernelOnDevice);
  start_timer();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size, end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_ *resu_ptr = resu_base + begin_bloc;
      #pragma omp target teams distribute parallel for if (kernelOnDevice)
      for (int count=0; count < end_bloc - begin_bloc; count++)
        {
          _TYPE_ &p_resu = resu_ptr[count];
          if (IS_ADD) p_resu += x;
          if (IS_SUB) p_resu -= x;
          if (IS_MULT) p_resu *= x;
          if (IS_EGAL) p_resu = x;
          if (IS_NEGATE) p_resu = -p_resu;
#if defined(INT_is_64_) && defined(__NVCOMPILER) // Bug compilateur nvc++ std::abs(long) plante...
          if (IS_ABS) p_resu = (_TYPE_) (std::is_same<_TYPE_,int>::value ? std::abs((long long)p_resu) : std::fabs(p_resu));
#else
          if (IS_ABS) p_resu = (_TYPE_) (std::is_same<_TYPE_,int>::value ? std::abs(p_resu) : std::fabs(p_resu));
#endif
          if (IS_RACINE_CARRE) p_resu = (_TYPE_) sqrt(p_resu);  // _TYPE_ casting just to pass 'int' instanciation of the template wo triggering -Wconversion warning
          if (IS_CARRE) p_resu *= p_resu;

          if (IS_DIV)
            {
#ifndef _OPENMP
              if (x == 0.) error_divide(__func__);
#endif
              p_resu /= x;
            }

          if (IS_INV)
            {
#ifndef _OPENMP
              if (p_resu == 0.) error_divide(__func__);
#endif
              p_resu = (_TYPE_) (1. / p_resu); // same as sqrt above
            }
        }
    }
  if (timer) end_timer(kernelOnDevice, "operator_vect_single_generic(x,y)");
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
}
// Explicit instanciation for templates:
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::ADD_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::SUB_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::MULT_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::DIV_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::DIV_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::DIV_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::EGAL_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::EGAL_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::EGAL_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::NEGATE_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::NEGATE_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::NEGATE_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::INV_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::INV_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::INV_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::ABS_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::ABS_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::ABS_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::RACINE_CARRE_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::RACINE_CARRE_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::RACINE_CARRE_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);
template void operator_vect_single_generic<double, TYPE_OPERATOR_SINGLE::CARRE_>(TRUSTVect<double>& resu, const double x, Mp_vect_options opt);
template void operator_vect_single_generic<int, TYPE_OPERATOR_SINGLE::CARRE_>(TRUSTVect<int>& resu, const int x, Mp_vect_options opt);
template void operator_vect_single_generic<float, TYPE_OPERATOR_SINGLE::CARRE_>(TRUSTVect<float>& resu, const float x, Mp_vect_options opt);

template <typename _TYPE_,typename _TYPE_RETURN_, TYPE_OPERATION_VECT _TYPE_OP_ >
_TYPE_RETURN_ local_extrema_vect_generic(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt)
{
  static constexpr bool IS_IMAX = (_TYPE_OP_ == TYPE_OPERATION_VECT::IMAX_), IS_IMIN = (_TYPE_OP_ == TYPE_OPERATION_VECT::IMIN_), IS_MAX = (_TYPE_OP_ == TYPE_OPERATION_VECT::MAX_),
                        IS_MIN = (_TYPE_OP_ == TYPE_OPERATION_VECT::MIN_), IS_MAX_ABS = (_TYPE_OP_ == TYPE_OPERATION_VECT::MAX_ABS_), IS_MIN_ABS = (_TYPE_OP_ == TYPE_OPERATION_VECT::MIN_ABS_);

  _TYPE_ min_max_val = neutral_value<_TYPE_,_TYPE_OP_>(); // _TYPE_ et pas _TYPE_RETURN_ desole ...
  _TYPE_RETURN_ i_min_max = -1 ; // seulement pour IMAX_ et IMIN_

  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1, one_bloc[2];
  const int *bloc_ptr;
  if (opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
      assert(items_blocs.size_array() % 2 == 0);
      nblocs_left = items_blocs.size_array() >> 1;
      bloc_ptr = items_blocs.addr();
    }
  else if (vect_size_tot > 0)
    {
      // attention, si vect_size_tot est nul, line_size a le droit d'etre nul
      // Compute all data, in the vector (including virtual data), build a big bloc:
      nblocs_left = 1;
      bloc_ptr = one_bloc;
      one_bloc[0] = 0;
      one_bloc[1] = vect_size_tot / line_size;
    }
  else // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
    return (IS_IMAX || IS_IMIN) ? i_min_max : (_TYPE_RETURN_)min_max_val;

  bool kernelOnDevice = vx.checkDataOnDevice();
  const _TYPE_ *x_base = mapToDevice(vx, "", kernelOnDevice);
  start_timer();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size, end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_ *x_ptr = x_base + begin_bloc;
      int size_bloc = end_bloc - begin_bloc;
      if (IS_MAX || IS_MAX_ABS)
        {
          if (kernelOnDevice)
            {
              #pragma omp target teams distribute parallel for reduction(max:min_max_val)
              for (int count = 0; count < size_bloc; count++)
                {
#if defined(INT_is_64_) && defined(__NVCOMPILER) // Bug compilateur nvc++ std::abs(long) plante...
                  const _TYPE_ x = IS_MAX ? x_ptr[count] : std::is_same<_TYPE_, int>::value ? (_TYPE_) std::abs((long long)
                                                                                                                x_ptr[count]) : (_TYPE_) std::fabs(x_ptr[count]);
#else
                  const _TYPE_ x = IS_MAX ? x_ptr[count] : std::is_same<_TYPE_, int>::value ? (_TYPE_) std::abs(
                                     x_ptr[count]) : (_TYPE_) std::fabs(x_ptr[count]);
#endif
                  if (x > min_max_val) min_max_val = x;
                }
            }
          else
            {
              for (int count = 0; count < size_bloc; count++)
                {
                  const _TYPE_ x = IS_MAX ? x_ptr[count] : std::is_same<_TYPE_, int>::value ? (_TYPE_) std::abs(
                                     x_ptr[count]) : (_TYPE_) std::fabs(x_ptr[count]);
                  if (x > min_max_val) min_max_val = x;
                }
            }
        }
      else if (IS_MIN || IS_MIN_ABS)
        {
          if (kernelOnDevice)
            {
              #pragma omp target teams distribute parallel for reduction(min:min_max_val)
              for (int count = 0; count < size_bloc; count++)
                {
#if defined(INT_is_64_) && defined(__NVCOMPILER) // Bug compilateur nvc++ std::abs(long) plante...
                  const _TYPE_ x = IS_MIN ? x_ptr[count] : std::is_same<_TYPE_, int>::value ? (_TYPE_) std::abs((long long)
                                                                                                                x_ptr[count]) : (_TYPE_) std::fabs(x_ptr[count]);
#else
                  const _TYPE_ x = IS_MIN ? x_ptr[count] : std::is_same<_TYPE_, int>::value ? (_TYPE_) std::abs(
                                     x_ptr[count]) : (_TYPE_) std::fabs(x_ptr[count]);
#endif
                  if (x < min_max_val) min_max_val = x;
                }
            }
          else
            {
              for (int count = 0; count < size_bloc; count++)
                {
                  const _TYPE_ x = IS_MIN ? x_ptr[count] : std::is_same<_TYPE_, int>::value ? (_TYPE_) std::abs(
                                     x_ptr[count]) : (_TYPE_) std::fabs(x_ptr[count]);
                  if (x < min_max_val) min_max_val = x;
                }
            }
        }
      // ToDo OpenMP Pas porte sur device car le compilateur NVidia plante:
      if (IS_IMAX || IS_IMIN)
        {
          for (int count = 0; count < size_bloc; count++)
            {
              const _TYPE_ x = x_ptr[count];
              if ((IS_IMAX && x > min_max_val) || (IS_IMIN && x < min_max_val))
                {
                  i_min_max = (_TYPE_RETURN_) count;
                  min_max_val = x;
                }
            }
        }
      // Compilateur NVidia plante sur cela: "symbol local_min_max_val(20167) is team-private but we are returning false"
      /*
      if (IS_IMAX || IS_IMIN)
        {
          #pragma omp target parallel if (kernelOnDevice)
          {
            _TYPE_ local_min_max_val = min_max_val;
            _TYPE_RETURN_ local_i_min_max = -1;
            #pragma omp for nowait
            for (int count = 0; count < end_bloc - begin_bloc; count++)
              {
                const _TYPE_ x = x_ptr[count];
                if ((IS_IMAX && x > local_min_max_val) || (IS_IMIN && x < local_min_max_val))
                  {
                    local_min_max_val = x;
                    local_i_min_max = (_TYPE_RETURN_) count;
                  }
              }
            #pragma omp critical
            {
              if ((IS_IMAX && local_min_max_val > min_max_val) || (IS_IMIN && local_min_max_val < min_max_val))
                {
                  min_max_val = local_min_max_val;
                  i_min_max = local_i_min_max;
                }
            }
          }
        } */
    }
  if (timer) end_timer(kernelOnDevice, "local_extrema_vect_generic(x)");
  return (IS_IMAX || IS_IMIN) ? i_min_max : (_TYPE_RETURN_)min_max_val;
}
// Explicit instanciation for templates:
template double local_extrema_vect_generic<double, double, TYPE_OPERATION_VECT::IMAX_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, double, TYPE_OPERATION_VECT::IMIN_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, double, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, double, TYPE_OPERATION_VECT::MIN_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, double, TYPE_OPERATION_VECT::MAX_ABS_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template double local_extrema_vect_generic<double, double, TYPE_OPERATION_VECT::MIN_ABS_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, TYPE_OPERATION_VECT::IMAX_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, TYPE_OPERATION_VECT::IMIN_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, TYPE_OPERATION_VECT::MIN_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, TYPE_OPERATION_VECT::MAX_ABS_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<double, int, TYPE_OPERATION_VECT::MIN_ABS_>(const TRUSTVect<double>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, TYPE_OPERATION_VECT::IMAX_>(const TRUSTVect<int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, TYPE_OPERATION_VECT::IMIN_>(const TRUSTVect<int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, TYPE_OPERATION_VECT::MAX_>(const TRUSTVect<int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, TYPE_OPERATION_VECT::MIN_>(const TRUSTVect<int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, TYPE_OPERATION_VECT::MAX_ABS_>(const TRUSTVect<int>& vx, Mp_vect_options opt);
template int local_extrema_vect_generic<int, int, TYPE_OPERATION_VECT::MIN_ABS_>(const TRUSTVect<int>& vx, Mp_vect_options opt);

template <typename _TYPE_, TYPE_OPERATION_VECT_BIS _TYPE_OP_ >
_TYPE_ local_operations_vect_bis_generic(const TRUSTVect<_TYPE_>& vx,Mp_vect_options opt)
{
  static constexpr bool IS_CARRE = (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::CARRE_), IS_SOMME = (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::SOMME_);

  _TYPE_ sum = 0;
  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " VECT_SEQUENTIAL_ITEMS"
  int nblocs_left = 1, one_bloc[2];
  const int *bloc_ptr;
  if (opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
      assert(items_blocs.size_array() % 2 == 0);
      nblocs_left = items_blocs.size_array() >> 1;
      bloc_ptr = items_blocs.addr();
    }
  else if (vect_size_tot > 0)
    {
      // attention, si vect_size_tot est nul, line_size a le droit d'etre nul
      // Compute all data, in the vector (including virtual data), build a big bloc:
      nblocs_left = 1;
      bloc_ptr = one_bloc;
      one_bloc[0] = 0;
      one_bloc[1] = vect_size_tot / line_size;
    }
  else // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
    return sum;

  bool kernelOnDevice = vx.checkDataOnDevice();
  const _TYPE_ *x_base = mapToDevice(vx, "", kernelOnDevice);
  start_timer();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size, end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_ *x_ptr = x_base + begin_bloc;
      if (kernelOnDevice)
        {
          #pragma omp target teams distribute parallel for reduction(+:sum)
          for (int count = 0; count < end_bloc - begin_bloc; count++)
            {
              const _TYPE_ x = x_ptr[count];
              if (IS_CARRE) sum += x * x;
              if (IS_SOMME) sum += x;
            }
        }
      else
        {
          for (int count = 0; count < end_bloc - begin_bloc; count++)
            {
              const _TYPE_ x = x_ptr[count];
              if (IS_CARRE) sum += x * x;
              if (IS_SOMME) sum += x;
            }
        }
    }
  if (timer) end_timer(kernelOnDevice, "local_operations_vect_bis_generic(x)");
  return sum;
}
// Explicit instanciation for templates:
template double local_operations_vect_bis_generic<double, TYPE_OPERATION_VECT_BIS::CARRE_>(const TRUSTVect<double>& vx,Mp_vect_options opt);
template int local_operations_vect_bis_generic<int, TYPE_OPERATION_VECT_BIS::CARRE_>(const TRUSTVect<int>& vx,Mp_vect_options opt);
template float local_operations_vect_bis_generic<float, TYPE_OPERATION_VECT_BIS::CARRE_>(const TRUSTVect<float>& vx,Mp_vect_options opt);
template double local_operations_vect_bis_generic<double, TYPE_OPERATION_VECT_BIS::SOMME_>(const TRUSTVect<double>& vx,Mp_vect_options opt);
template int local_operations_vect_bis_generic<int, TYPE_OPERATION_VECT_BIS::SOMME_>(const TRUSTVect<int>& vx,Mp_vect_options opt);
template float local_operations_vect_bis_generic<float, TYPE_OPERATION_VECT_BIS::SOMME_>(const TRUSTVect<float>& vx,Mp_vect_options opt);

// ==================================================================================================================================
// DEBUT code pour debug
#ifndef NDEBUG
// INVALID_SCALAR is used to fill arrays when values are not computed (virtual space might not be computed by operators).
// The value below probably triggers errors on parallel test cases but does not prevent from doing "useless" computations with it.
template <typename _TYPE_>
void invalidate_data(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt)
{
  _TYPE_ invalid = (std::is_same<_TYPE_,int>::value) ? INT_MAX : (std::is_same<_TYPE_,float>::value) ? -987654.321f : -987654.321 ;

  const MD_Vector& md = resu.get_md_vector();
  const int line_size = resu.line_size();
  if (opt == VECT_ALL_ITEMS || (!md.non_nul())) return; // no invalid values
  assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
  const TRUSTArray<int>& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
  const int blocs_size = items_blocs.size_array();
  int i = 0;
  bool kernelOnDevice = resu.checkDataOnDevice();
  _TYPE_ *resu_ptr = computeOnTheDevice(resu, "", kernelOnDevice);
  start_timer();
  for (int blocs_idx = 0; blocs_idx < blocs_size; blocs_idx += 2) // process data until beginning of next bloc, or end of array
    {
      const int bloc_end = line_size * items_blocs[blocs_idx];
      #pragma omp target teams distribute parallel for if (kernelOnDevice)
      for (int count=i; count < bloc_end; count++) resu_ptr[count] = invalid;
      i = items_blocs[blocs_idx+1] * line_size;
    }
  const int bloc_end = resu.size_array(); // Process until end of vector
  #pragma omp target teams distribute parallel for if (kernelOnDevice)
  for (int count=i; count < bloc_end; count++) resu_ptr[count] = invalid;
  if (timer) end_timer(kernelOnDevice, "invalidate_data(x)");
}
// FIN code pour debug
// ==================================================================================================================================
// Explicit instanciation for templates:
template void invalidate_data<double>(TRUSTVect<double>& resu, Mp_vect_options opt);
template void invalidate_data<float>(TRUSTVect<float>& resu, Mp_vect_options opt);
template void invalidate_data<int>(TRUSTVect<int>& resu, Mp_vect_options opt);
#endif /* NDEBUG */

template<typename _TYPE_>
_TYPE_ local_prodscal(const TRUSTVect<_TYPE_>& vx, const TRUSTVect<_TYPE_>& vy, Mp_vect_options opt)
{
  _TYPE_ sum = 0;
  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size && vy.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot && vy.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md && vy.get_md_vector() == md);
  // Determine blocs of data to process, depending on " VECT_SEQUENTIAL_ITEMS"
  int nblocs_left = 1, one_bloc[2];
  const int *bloc_ptr;
  if (opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
      assert(items_blocs.size_array() % 2 == 0);
      nblocs_left = items_blocs.size_array() >> 1;
      bloc_ptr = items_blocs.addr();
    }
  else if (vect_size_tot > 0)
    {
      // attention, si vect_size_tot est nul, line_size a le droit d'etre nul
      // Compute all data, in the vector (including virtual data), build a big bloc:
      nblocs_left = 1;
      bloc_ptr = one_bloc;
      one_bloc[0] = 0;
      one_bloc[1] = vect_size_tot / line_size;
    }
  else // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
    return sum;

  bool kernelOnDevice = const_cast<TRUSTVect<_TYPE_>&>(vx).checkDataOnDevice(vy);
  const _TYPE_ *vx_ptr = mapToDevice(vx, "", kernelOnDevice);
  const _TYPE_ *vy_ptr = mapToDevice(vy, "", kernelOnDevice);
  start_timer();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size, end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      // ToDo OpenMP bug nvc++ compiler recent bouh: https://forums.developer.nvidia.com/t/openmp-nvc-duplicate-name-in-reduction-clause-error-with-recent-sdk/255696/3
      if (kernelOnDevice)
        #pragma omp target teams distribute parallel for reduction(+:sum)
        for (int i=begin_bloc; i<end_bloc; i++)
          sum += vx_ptr[i] * vy_ptr[i];
      else
        for (int i=begin_bloc; i<end_bloc; i++)
          sum += vx_ptr[i] * vy_ptr[i];
    }
  if (timer) end_timer(kernelOnDevice, "local_prodscal(vx,vy)");
  return sum;
}
// Explicit instanciation for templates:
template double local_prodscal(const TRUSTVect<double>& vx, const TRUSTVect<double>& vy, Mp_vect_options opt);
template float local_prodscal(const TRUSTVect<float>& vx, const TRUSTVect<float>& vy, Mp_vect_options opt);
