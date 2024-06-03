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

#ifndef TRUSTVect_tools_TPP_included
#define TRUSTVect_tools_TPP_included

#ifdef MICROSOFT
#define HUGE_VALL 1e99
#endif

/*! @brief renvoie 1 si meme strucuture parallele et egalite au sens TRUSTArray (y compris espaces virtuels) BM: faut-il etre aussi strict, comparer uniquement size() elements ?
 *
 */
template <typename _TYPE_>
inline int operator==(const TRUSTVect<_TYPE_>& x, const TRUSTVect<_TYPE_>& y)
{
  x.checkDataOnHost();
  y.checkDataOnHost();
  if (!(x.get_md_vector() == y.get_md_vector())) return 0;
  const TRUSTArray<_TYPE_>& ax = x;
  const TRUSTArray<_TYPE_>& ay = y;
  return ax == ay;
}

template <typename _TYPE_>
inline int operator!=(const TRUSTVect<_TYPE_>& x, const TRUSTVect<_TYPE_>& y)
{
  return !(x == y);
}

template <typename _TYPE_>
extern void invalidate_data(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt);

// ==================================================================================================================================
// DEBUT code pour operation min/max/abs
enum class TYPE_OPERATION_VECT { IMAX_ , IMIN_ , MAX_ , MIN_ , MAX_ABS_ , MIN_ABS_ };

inline double neutral_value_double_(const bool IS_MAX)
{
  return IS_MAX ? (-HUGE_VAL) : HUGE_VAL;
}

inline float neutral_value_float_(const bool IS_MAX)
{
  return IS_MAX ? (-HUGE_VALF) : HUGE_VALF; // et ouiiiiiiiiiiiiiiii
}

inline int neutral_value_int_(const bool IS_MAX)
{
  return IS_MAX ? INT_MIN : INT_MAX;
}

template <typename _TYPE_, TYPE_OPERATION_VECT _TYPE_OP_ >
inline _TYPE_ neutral_value()
{
  static constexpr bool IS_MAX = ((_TYPE_OP_ == TYPE_OPERATION_VECT::IMAX_) || (_TYPE_OP_ == TYPE_OPERATION_VECT::MAX_)), IS_MAX_ABS = (_TYPE_OP_ == TYPE_OPERATION_VECT::MAX_ABS_);
  if (IS_MAX_ABS)
    return 0;
  else
    {
      _TYPE_ neutral_val;

      if (std::is_same<_TYPE_, double>::value) neutral_val = (_TYPE_) neutral_value_double_(IS_MAX);
      else if (std::is_same<_TYPE_, float>::value) neutral_val = (_TYPE_) neutral_value_float_(IS_MAX);
      else neutral_val = (_TYPE_) neutral_value_int_(IS_MAX);

      return neutral_val;
    }
}

template <typename _TYPE_,typename _TYPE_RETURN_, TYPE_OPERATION_VECT _TYPE_OP_ >
extern _TYPE_RETURN_ local_extrema_vect_generic(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt);

template <typename _TYPE_>
inline int local_imax_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_extrema_vect_generic<_TYPE_,int,TYPE_OPERATION_VECT::IMAX_>(vx,opt);
}

template <typename _TYPE_>
inline int local_imin_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_extrema_vect_generic<_TYPE_,int,TYPE_OPERATION_VECT::IMIN_>(vx,opt);
}

template <typename _TYPE_>
inline _TYPE_ local_max_vect_(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_max_vect(vx,opt);
}

template <typename _TYPE_>
inline _TYPE_ local_max_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_extrema_vect_generic<_TYPE_,_TYPE_ /* return type */,TYPE_OPERATION_VECT::MAX_>(vx,opt);
}

template <typename _TYPE_>
inline _TYPE_ local_min_vect_(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_min_vect(vx,opt);
}

template<typename _TYPE_>
inline _TYPE_ local_min_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_extrema_vect_generic<_TYPE_,_TYPE_ /* return type */,TYPE_OPERATION_VECT::MIN_>(vx, opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_max_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_max_vect(x, opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_max_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_max_vect(x, opt);
  s = Process::mp_max(s);
  return s;
}

template<typename _TYPE_>
inline _TYPE_ mp_min_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_min_vect(x, opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_min_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_min_vect(x, opt);
  s = Process::mp_min(s);
  return s;
}

template <typename _TYPE_>
inline _TYPE_ mp_somme_vect(const TRUSTVect<_TYPE_>& vx)
{
  _TYPE_ x = local_somme_vect(vx);
  _TYPE_ y = Process::mp_sum(x);
  return y;
}

template <typename _TYPE_>
inline _TYPE_ local_max_abs_vect_(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_max_abs_vect(vx,opt);
}

template <typename _TYPE_>
inline _TYPE_ local_max_abs_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_extrema_vect_generic<_TYPE_,_TYPE_ /* return type */,TYPE_OPERATION_VECT::MAX_ABS_>(vx, opt);
}

template <typename _TYPE_>
inline _TYPE_ local_min_abs_vect_(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_min_abs_vect(vx,opt);
}

template<typename _TYPE_>
inline _TYPE_ local_min_abs_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_extrema_vect_generic<_TYPE_,_TYPE_ /* return type */,TYPE_OPERATION_VECT::MIN_ABS_>(vx, opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_max_abs_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_max_abs_vect(x, opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_max_abs_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_max_abs_vect(x, opt);
  s = Process::mp_max(s);
  return s;
}

template<typename _TYPE_>
inline _TYPE_ mp_min_abs_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_min_abs_vect(x, opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_min_abs_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_min_abs_vect(x, opt);
  s = Process::mp_min(s);
  return s;
}

template <typename _TYPE_>
extern _TYPE_ local_prodscal(const TRUSTVect<_TYPE_>& vx, const TRUSTVect<_TYPE_>& vy, Mp_vect_options opt = VECT_SEQUENTIAL_ITEMS);

enum class TYPE_OPERATION_VECT_BIS { CARRE_ , SOMME_ };

template <typename _TYPE_, TYPE_OPERATION_VECT_BIS _TYPE_OP_ >
extern _TYPE_ local_operations_vect_bis_generic(const TRUSTVect<_TYPE_>& vx,Mp_vect_options opt);

template<typename _TYPE_>
inline _TYPE_ local_carre_norme_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_SEQUENTIAL_ITEMS)
{
  return local_operations_vect_bis_generic<_TYPE_,TYPE_OPERATION_VECT_BIS::CARRE_>(vx,opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_carre_norme_vect(const TRUSTVect<_TYPE_>& vx)
{
  return Process::mp_sum(local_carre_norme_vect(vx));
}

template<typename _TYPE_>
inline _TYPE_ local_somme_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_SEQUENTIAL_ITEMS)
{
  return local_operations_vect_bis_generic<_TYPE_,TYPE_OPERATION_VECT_BIS::SOMME_>(vx,opt);
}

template<typename _TYPE_>
inline _TYPE_ mp_prodscal(const TRUSTVect<_TYPE_>& x, const TRUSTVect<_TYPE_>& y)
{
  return Process::mp_sum(local_prodscal(x, y));
}

inline int mp_norme_vect(const TRUSTVect<int>& vx) = delete; // forbidden

template<typename _TYPE_>
inline _TYPE_ mp_norme_vect(const TRUSTVect<_TYPE_>& vx)
{
  _TYPE_ x = mp_carre_norme_vect(vx);
  x = sqrt(x);
  return x;
}

template<typename _TYPE_>
inline _TYPE_ mp_norme_vect_(const TRUSTVect<_TYPE_>& vx)
{
  return mp_norme_vect(vx);
}

inline int mp_moyenne_vect(const TRUSTVect<int>& x) = delete; // forbidden

template<typename _TYPE_>
inline _TYPE_ mp_moyenne_vect(const TRUSTVect<_TYPE_>& x)
{
  _TYPE_ s = mp_somme_vect(x), n;
  const MD_Vector& md = x.get_md_vector();
  if (md.non_nul()) n = md.valeur().nb_items_seq_tot() * x.line_size();
  else
    {
      assert(Process::nproc() == 1); // Coding error: mp_moyenne_vect is used on a not distributed TRUSTVect<double> !
      n = x.size_totale();
    }
  return s / n;
}
// FIN code pour operation min/max/abs
// ==================================================================================================================================

// ==================================================================================================================================
// DEBUT code pour operateurs vect/vect ou vect/scalair
enum class TYPE_OPERATOR_VECT { ADD_ , SUB_ , MULT_ , DIV_ , EGAL_ };

inline void error_divide(const char * nom_funct)
{
  Cerr << "What ??? Divide by 0 in " << nom_funct << " operator_divide() !!" << finl;
  throw;
}

template <typename _TYPE_, TYPE_OPERATOR_VECT _TYPE_OP_ >
extern void operator_vect_vect_generic(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt);

template <typename _TYPE_>
inline void operator_add(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_vect_generic<_TYPE_,TYPE_OPERATOR_VECT::ADD_>(resu,vx,opt);
}

template <typename _TYPE_>
inline void operator_sub(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_vect_generic<_TYPE_,TYPE_OPERATOR_VECT::SUB_>(resu,vx,opt);
}

template <typename _TYPE_>
inline void operator_multiply(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_vect_generic<_TYPE_,TYPE_OPERATOR_VECT::MULT_>(resu,vx,opt);
}

inline void operator_divide(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden

template <typename _TYPE_>
inline void operator_divide(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_vect_generic<_TYPE_,TYPE_OPERATOR_VECT::DIV_>(resu,vx,opt);
}

template <typename _TYPE_>
inline void operator_egal(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_vect_generic<_TYPE_,TYPE_OPERATOR_VECT::EGAL_>(resu,vx,opt);
}

enum class TYPE_OPERATOR_SINGLE { ADD_ , SUB_ , MULT_ , DIV_ , EGAL_ , NEGATE_ , INV_ , ABS_ , RACINE_CARRE_ , CARRE_ };

template <typename _TYPE_, TYPE_OPERATOR_SINGLE _TYPE_OP_ >
extern void operator_vect_single_generic(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt);

template <typename _TYPE_>
inline void operator_add(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::ADD_>(resu,x,opt);
}

template <typename _TYPE_>
inline void operator_sub(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::SUB_>(resu,x,opt);
}

// ATTENTION : on utilise is_convertible sinon soucis quand TYPE = double et x = int ... ex : operator_multiply(TRUSTVect<double>, 1) ...
template<typename _TYPE_, typename _SCALAR_TYPE_>
typename std::enable_if<std::is_convertible<_SCALAR_TYPE_, _TYPE_>::value >::type
inline operator_multiply(TRUSTVect<_TYPE_>& resu, const _SCALAR_TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::MULT_>(resu,x,opt);
}

inline void operator_divide(TRUSTVect<int>& resu, const int x, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden (avant c'etait possible ... a voir si besoin)

template <typename _TYPE_>
inline void operator_divide(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::DIV_>(resu,x,opt);
}

template <typename _TYPE_>
inline void operator_egal(TRUSTVect<_TYPE_>& resu, _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::EGAL_>(resu,x,opt);
}

template <typename _TYPE_>
inline void operator_negate(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::NEGATE_>(resu,(_TYPE_)0 /* inutile */,opt);
}

inline void operator_inverse(TRUSTVect<int>& resu, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden

template <typename _TYPE_>
inline void operator_inverse(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::INV_>(resu,0. /* inutile */,opt);
}

template <typename _TYPE_>
inline void operator_abs(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::ABS_>(resu,(_TYPE_)0 /* inutile */,opt);
}

inline void racine_carree(TRUSTVect<int>& resu, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden

template <typename _TYPE_>
inline void racine_carree(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::RACINE_CARRE_>(resu,0. /* inutile */,opt);
}

inline void racine_carree_(TRUSTVect<int>& resu, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden

template <typename _TYPE_>
inline void racine_carree_(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  racine_carree(resu,opt);
}

template <typename _TYPE_>
inline void carre(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  operator_vect_single_generic<_TYPE_,TYPE_OPERATOR_SINGLE::CARRE_>(resu,0. /* inutile */,opt);
}

template <typename _TYPE_>
inline void carre_(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt)
{
  carre(resu,opt);
}

// FIN code pour operateurs vect/vect ou vect/scalair
// ==================================================================================================================================

// ==================================================================================================================================
// DEBUT code pour operations speciales
enum class TYPE_OPERATION_VECT_SPEC { ADD_ , CARRE_ };

template <TYPE_OPERATION_VECT_SPEC _TYPE_OP_ >
inline void ajoute_operation_speciale_generic(TRUSTVect<int>& resu, int alpha, const TRUSTVect<int>& vx, Mp_vect_options opt) = delete; // forbidden ... ajoute si besoin

template <TYPE_OPERATION_VECT_SPEC _TYPE_OP_ ,typename _TYPE_>
extern void ajoute_operation_speciale_generic(TRUSTVect<_TYPE_>& resu, _TYPE_ alpha, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt);

template <typename _TYPE_>
inline void ajoute_alpha_v(TRUSTVect<_TYPE_>& resu, _TYPE_ alpha, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::ADD_,_TYPE_>(resu,alpha,vx,opt);
}

inline void ajoute_carre(TRUSTVect<int>& resu, int alpha, const TRUSTVect<int>& vx, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden ... ajoute si besoin

template <typename _TYPE_>
inline void ajoute_carre(TRUSTVect<_TYPE_>& resu, _TYPE_ alpha, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::CARRE_,_TYPE_>(resu,alpha,vx,opt);
}

inline void ajoute_carre_(TRUSTVect<int>& resu, int alpha, const TRUSTVect<int>& vx, Mp_vect_options opt) = delete; // forbidden ... ajoute si besoin

template <typename _TYPE_>
inline void ajoute_carre_(TRUSTVect<_TYPE_>& resu, _TYPE_ alpha, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt)
{
  ajoute_carre(resu,alpha,vx,opt);
}

// ToDo OpenMP offload in .cpp (mais semble pas utilise...)
template <typename _TYPE_>
inline void ajoute_produit_scalaire(TRUSTVect<_TYPE_>& resu, _TYPE_ alpha, const TRUSTVect<_TYPE_>& vx, const TRUSTVect<_TYPE_>& vy, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  resu.checkDataOnHost();
  vx.checkDataOnHost();
  vy.checkDataOnHost();
  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  const int line_size = master_vect.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  assert(vx.line_size() == line_size && vy.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot && vy.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md && vy.get_md_vector() == md);
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

  _TYPE_ *resu_base = resu.addr();
  const _TYPE_ *x_base = vx.addr();
  const _TYPE_ *y_base = vy.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size, end_bloc = (*(bloc_ptr++)) * line_size;
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
}

enum class TYPE_OPERATION_VECT_SPEC_GENERIC { MUL_ , DIV_ };

template <TYPE_OPERATION_VECT_SPEC_GENERIC _TYPE_OP_ >
inline void operation_speciale_tres_generic(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt) = delete; // forbidden !!

// ToDo OpenMP offload in .cpp
template <TYPE_OPERATION_VECT_SPEC_GENERIC _TYPE_OP_ , typename _TYPE_>
inline void operation_speciale_tres_generic(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt)
{
  resu.checkDataOnHost();
  vx.checkDataOnHost();
  static constexpr bool IS_MUL = (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_), IS_DIV = (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC_GENERIC::DIV_);

  // Master vect donne la structure de reference, les autres vecteurs doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  const int line_size = master_vect.line_size(), line_size_vx = vx.line_size(), vect_size_tot = master_vect.size_totale();
  const MD_Vector& md = master_vect.get_md_vector();
  // Le line_size du vecteur resu doit etre un multiple du line_size du vecteur vx
  assert(line_size > 0 && line_size_vx > 0 && line_size % line_size_vx == 0);
  const int delta_line_size = line_size / line_size_vx;
  assert(vx.size_totale() * delta_line_size == vect_size_tot); // this test is necessary if md is null
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

  _TYPE_ *resu_base = resu.addr();
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size_vx, end_bloc = (*(bloc_ptr++)) * line_size_vx;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_ *resu_ptr = resu_base + begin_bloc * delta_line_size;
      const _TYPE_ *x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          // Any shape: pour chaque item de vx, on a delta_line_size items de resu a traiter
          for (int count2 = delta_line_size; count2; count2--)
            {
              _TYPE_& p_resu = *(resu_ptr++);

              if (IS_MUL) p_resu *= x;

              if (IS_DIV)
                {
                  if (x == 0) error_divide(__func__);
                  p_resu *= (1. / x);
                }
            }
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu, opt);
#endif
  return;
}

inline void tab_multiply_any_shape_(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt) = delete; // forbidden

template<typename _TYPE_>
inline void tab_multiply_any_shape_(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt)
{
  operation_speciale_tres_generic<TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_,_TYPE_>(resu,vx,opt);
}

inline void tab_divide_any_shape_(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt) = delete; // forbidden

template<typename _TYPE_>
inline void tab_divide_any_shape_(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt)
{
  operation_speciale_tres_generic<TYPE_OPERATION_VECT_SPEC_GENERIC::DIV_,_TYPE_>(resu,vx,opt);
}

// Cette methode permettent de multiplier un tableau a plusieurs dimensions par un tableau de dimension inferieure (par exemple un tableau a trois composantes par un tableau a une composante).
//  Chaque valeur du tableau vx est utilisee pour plusieurs items consecutifs du tableau resu (le nombre de fois est le rapport des line_size() des deux tableaux).
//  resu.line_size() doit etre un multiple int de vx.line_size() et les descripteurs doivent etre identiques.
//  Cas particulier: vx peut contenir une constante unique (size_array() == 1 et descripteur nul), dans ce cas c'est un simple produit par la constante
template<typename _TYPE_>
inline void tab_multiply_any_shape(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  if (vx.size_array() == 1 && !vx.get_md_vector().non_nul()) // Produit par une constante
    {
      const _TYPE_ x = vx[0];
      operator_multiply(resu, x, opt);
    }
  else if (vx.line_size() == resu.line_size()) // Produit membre a membre
    operator_multiply(resu, vx, opt);
  else // Cas general
    tab_multiply_any_shape_(resu, vx, opt);
}

inline void tab_multiply_any_shape(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden

// Idem que tab_multiply_any_shape() mais avec une division
template<typename _TYPE_>
inline void tab_divide_any_shape(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  if (vx.size_array() == 1 && !vx.get_md_vector().non_nul()) // division par une constante
    {
      if (vx[0] == 0) error_divide(__func__);
      const _TYPE_ x = 1. / vx[0];
      operator_multiply(resu, x, opt);
    }
  else if (vx.line_size() == resu.line_size()) // division membre a membre
    operator_divide(resu, vx, opt);
  else // Cas general
    tab_divide_any_shape_(resu, vx, opt);
}

inline void tab_divide_any_shape(TRUSTVect<int>& resu, const TRUSTVect<int>& vx, Mp_vect_options opt = VECT_ALL_ITEMS) = delete; // forbidden
// FIN code pour operations speciales
// ==================================================================================================================================

#endif /* TRUSTVect_tools_TPP_included */
