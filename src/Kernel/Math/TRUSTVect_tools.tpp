/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        TRUSTVect_tools.tpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTVect_tools_TPP_included
#define TRUSTVect_tools_TPP_included

// Description: renvoie 1 si meme strucuture parallele et egalite au sens ArrOfDouble (y compris espaces virtuels)
//  BM: faut-il etre aussi strict, comparer uniquement size() elements ?
template <typename _TYPE_>
inline int operator==(const TRUSTVect<_TYPE_>& x, const TRUSTVect<_TYPE_>& y)
{
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

#ifdef MICROSOFT
#define HUGE_VALL 1e99
#endif

// INVALID_SCALAR is used to fill arrays when values are not computed (virtual space might not be computed by operators).
// The value below probably triggers errors on parallel test cases but does not prevent from doing "useless" computations with it.
#ifndef NDEBUG

template <typename _TYPE_>
inline void invalidate_data(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt)
{
  _TYPE_ invalid;

  if (std::is_same<_TYPE_,double>::value) invalid = -987654.321;
  else invalid = INT_MAX;

  const MD_Vector& md = resu.get_md_vector();
  const int line_size = resu.line_size();
  if (opt == VECT_ALL_ITEMS || (!md.non_nul()))
    return; // no invalid values
  assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
  const ArrOfInt& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
  const int blocs_size = items_blocs.size_array();
  int i = 0;
  for (int blocs_idx = 0; blocs_idx < blocs_size; blocs_idx += 2)
    {
      // process data until beginning of next bloc, or end of array
      const int bloc_end = line_size * items_blocs[blocs_idx];
      _TYPE_ *ptr = resu.addr() + i;
      for (; i < bloc_end; i++)
        *(ptr++) = invalid;
      i = items_blocs[blocs_idx+1] * line_size;
    }
  // Process until end of vector
  const int bloc_end = resu.size_array();
  _TYPE_ *ptr = resu.addr() + i;
  for (; i < bloc_end; i++)
    *(ptr++) = invalid;
}

#endif /* NDEBUG */

template <typename _TYPE_>
inline int local_imax_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ max_val;

  if (std::is_same<_TYPE_,double>::value) max_val = (-HUGE_VALL);
  else max_val = INT_MIN;

  int i_max = -1;
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  i_max;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          if (x > max_val) { i_max = x_ptr - x_base; max_val = x; }
          x_ptr++;
        }
    }
  return  i_max;
}

template <typename _TYPE_>
inline _TYPE_ local_max_vect_(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_max_vect(vx,opt);
}

template <typename _TYPE_>
inline _TYPE_ local_max_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ max_val;

  if (std::is_same<_TYPE_,double>::value) max_val = (-HUGE_VALL);
  else max_val = INT_MIN;

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const TRUSTArray<int>& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  max_val;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          max_val = (x > max_val) ? x : max_val;
          x_ptr++;
        }
    }
  return  max_val;
}

template <typename _TYPE_>
inline int local_imin_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ min_val;

  if (std::is_same<_TYPE_,double>::value) min_val = HUGE_VALL;
  else min_val = INT_MAX;

  int i_min = -1;
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  i_min;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          if (x < min_val) { i_min = x_ptr - x_base; min_val = x; }
          x_ptr++;
        }
    }
  return  i_min;
}

template <typename _TYPE_>
inline _TYPE_ local_min_vect_(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_min_vect(vx,opt);
}

template <typename _TYPE_>
inline _TYPE_ local_min_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ min_val;

  if (std::is_same<_TYPE_,double>::value) min_val = HUGE_VALL;
  else min_val = INT_MAX;

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  min_val;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          min_val = (x < min_val) ? x : min_val;
          x_ptr++;
        }
    }
  return  min_val;
}

template <typename _TYPE_>
inline _TYPE_ mp_max_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_max_vect(x,opt);
}

template <typename _TYPE_>
inline _TYPE_ mp_max_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_max_vect(x, opt);
  s =  Process::mp_max(s);
  return s;
}

template <typename _TYPE_>
inline _TYPE_ mp_min_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_min_vect(x,opt);
}

template <typename _TYPE_>
inline _TYPE_ mp_min_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_min_vect(x, opt);
  s =  Process::mp_min(s);
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
  _TYPE_ max_val = 0;
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  max_val;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          _TYPE_ xx;

          xx = std::is_same<_TYPE_,double>::value ? std::fabs(x) : abs(x);
          max_val = (xx > max_val) ? xx : max_val;
          x_ptr++;
        }
    }
  return  max_val;
}

template <typename _TYPE_>
inline _TYPE_ local_min_abs_vect_(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return local_min_abs_vect(vx,opt);
}

template <typename _TYPE_>
inline _TYPE_ local_min_abs_vect(const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ min_val;

  if (std::is_same<_TYPE_,double>::value) min_val = HUGE_VALL;
  else min_val = INT_MAX;

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  min_val;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          _TYPE_ xx;

          xx = std::is_same<_TYPE_,double>::value ? std::fabs(x) : abs(x);
          min_val = (xx < min_val) ? xx : min_val;
          x_ptr++;
        }
    }
  return  min_val;
}

template <typename _TYPE_>
inline _TYPE_ mp_max_abs_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_max_abs_vect(x,opt);
}

template <typename _TYPE_>
inline _TYPE_ mp_max_abs_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_max_abs_vect(x, opt);
  s = Process::mp_max(s);
  return s;
}

template <typename _TYPE_>
inline _TYPE_ mp_min_abs_vect_(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  return mp_min_abs_vect(x,opt);
}

template <typename _TYPE_>
inline _TYPE_ mp_min_abs_vect(const TRUSTVect<_TYPE_>& x, Mp_vect_options opt = VECT_REAL_ITEMS)
{
  _TYPE_ s = local_min_abs_vect(x, opt);
  s = Process::mp_min(s);
  return s;
}

// Valeurs par defaut choisies pour compatibilite approximative avec V1.5.6
// (compatibilite exacte non voulue car necessite echange_espace_virtuel)
template <typename _TYPE_>
inline void operator_add(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu += x;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_add(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu += x;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_sub(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu -= x;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_sub(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu -= x;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_multiply(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu *= x;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

//// ET OUIIIIIIIIIIIIIIIIIII ::: reflechir pourquoi !!
//inline void operator_multiply(TRUSTVect<double>& resu, const int x, Mp_vect_options opt = VECT_ALL_ITEMS)
//{
//  operator_multiply<double>(resu,x,opt);
//}

//template <typename _TYPE_>
template<typename _TYPE_, typename _SCALAR_TYPE_> typename std::enable_if<std::is_convertible<_SCALAR_TYPE_, _TYPE_>::value >::type
inline operator_multiply(TRUSTVect<_TYPE_>& resu, const _SCALAR_TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu *= x;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_egal(TRUSTVect<_TYPE_>& resu, const TRUSTVect<_TYPE_>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu = x;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_egal(TRUSTVect<_TYPE_>& resu, _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu = x;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_negate(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu = -p_resu;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

/********************** */
// FIXME : PAS POUR INT

template <typename _TYPE_>
inline _TYPE_ local_prodscal(const TRUSTVect<_TYPE_>& vx, const TRUSTVect<_TYPE_>& vy)
{
  double sum = 0;
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  assert(vy.line_size() == line_size);
  assert(vy.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vy.get_md_vector() == md);
  // Determine blocs of data to process, depending on " VECT_SEQUENTIAL_ITEMS"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( VECT_SEQUENTIAL_ITEMS != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( VECT_SEQUENTIAL_ITEMS == VECT_SEQUENTIAL_ITEMS ||  VECT_SEQUENTIAL_ITEMS == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( VECT_SEQUENTIAL_ITEMS == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  sum;
    }
  const _TYPE_ *x_base = vx.addr();
  const _TYPE_ *y_base = vy.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      const _TYPE_* y_ptr = y_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          const _TYPE_ y = *(y_ptr++);
          sum += x * y;
          x_ptr++;
        }
    }
  return  sum;
}

template <typename _TYPE_>
inline _TYPE_ local_carre_norme_vect(const TRUSTVect<_TYPE_>& vx)
{
  _TYPE_ sum = 0;
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " VECT_SEQUENTIAL_ITEMS"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( VECT_SEQUENTIAL_ITEMS != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( VECT_SEQUENTIAL_ITEMS == VECT_SEQUENTIAL_ITEMS ||  VECT_SEQUENTIAL_ITEMS == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( VECT_SEQUENTIAL_ITEMS == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  sum;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          sum += x * x;
          x_ptr++;
        }
    }
  return  sum;
}

template <typename _TYPE_>
inline _TYPE_ mp_carre_norme_vect(const TRUSTVect<_TYPE_>& vx)
{
  return Process::mp_sum(local_carre_norme_vect(vx));
}

template <typename _TYPE_>
inline _TYPE_ local_somme_vect(const TRUSTVect<_TYPE_>& vx)
{
  _TYPE_ sum = 0;
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = vx;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " VECT_SEQUENTIAL_ITEMS"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( VECT_SEQUENTIAL_ITEMS != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( VECT_SEQUENTIAL_ITEMS == VECT_SEQUENTIAL_ITEMS ||  VECT_SEQUENTIAL_ITEMS == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( VECT_SEQUENTIAL_ITEMS == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return  sum;
    }
  const _TYPE_ *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      const _TYPE_* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const _TYPE_ x = *x_ptr;
          sum += x;
          x_ptr++;
        }
    }
  return  sum;
}

template <typename _TYPE_>
inline _TYPE_ mp_prodscal(const TRUSTVect<_TYPE_>& x,  const TRUSTVect<_TYPE_>& y )
{
  return Process::mp_sum(local_prodscal(x, y));
}

template <typename _TYPE_>
inline _TYPE_ mp_norme_vect(const TRUSTVect<_TYPE_>& vx)
{
  double x = mp_carre_norme_vect(vx);
  x = sqrt(x);
  return x;
}

template <typename _TYPE_>
inline _TYPE_ mp_moyenne_vect(const TRUSTVect<_TYPE_>& x)
{
  double s = mp_somme_vect(x);
  double n;
  const MD_Vector& md = x.get_md_vector();
  if (md.non_nul())
    n = md.valeur().nb_items_seq_tot() * x.line_size();
  else
    {
      // Coding error: mp_moyenne_vect is used on a not distributed DoubleVect !
      assert(Process::nproc()==1);
      n = x.size_totale();
    }
  return s / n;
}

inline void ajoute_alpha_v(TRUSTVect<double>& resu, double alpha, const TRUSTVect<double>& vx, Mp_vect_options opt = VECT_REAL_ITEMS)
{

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  const double *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      const double* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const double x = *x_ptr;
          double& p_resu = *(resu_ptr++);
          p_resu += alpha * x;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

inline void ajoute_carre(TRUSTVect<double>& resu, double alpha, const TRUSTVect<double>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  const double *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      const double* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const double x = *x_ptr;
          double& p_resu = *(resu_ptr++);
          p_resu += alpha * x * x;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

inline void ajoute_carre_(TRUSTVect<double>& resu, double alpha, const TRUSTVect<double>& vx, Mp_vect_options opt)
{
  ajoute_carre(resu,alpha,vx,opt);
}

inline void ajoute_produit_scalaire(TRUSTVect<double>& resu, double alpha, const TRUSTVect<double>& vx, const TRUSTVect<double>& vy, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  assert(vy.line_size() == line_size);
  assert(vy.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vy.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  const double *x_base = vx.addr();
  const double *y_base = vy.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      const double* x_ptr = x_base + begin_bloc;
      const double* y_ptr = y_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const double x = *x_ptr;
          const double y = *(y_ptr++);
          double& p_resu = *(resu_ptr++);
          p_resu += alpha * x * y;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

inline void racine_carree(TRUSTVect<double>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          double& p_resu = *(resu_ptr++);
          p_resu = sqrt(p_resu);
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

inline void racine_carree_(TRUSTVect<double>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  racine_carree(resu,opt);
}


inline void carre(TRUSTVect<double>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          double& p_resu = *(resu_ptr++);
          p_resu *= p_resu;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

inline void operator_inverse(TRUSTVect<double>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          double& p_resu = *(resu_ptr++);
          if (p_resu==0) { Cerr << "Divide by 0 in DoubleVect::operateur_inverse()" << finl; Process::exit();};
          p_resu = 1. / p_resu;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

template <typename _TYPE_>
inline void operator_abs(TRUSTVect<_TYPE_>& resu, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      _TYPE_* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          _TYPE_& p_resu = *(resu_ptr++);
          p_resu = std::is_same<_TYPE_,double>::value ? std::fabs(p_resu) : abs(p_resu);
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

static void tab_multiply_any_shape_(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  int line_size_vx = vx.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Le line_size du vecteur resu doit etre un multiple du line_size du vecteur vx
  assert(line_size > 0 && line_size_vx > 0 && line_size % line_size_vx == 0);
  const int delta_line_size = line_size / line_size_vx;
  assert(vx.size_totale() * delta_line_size == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  const double *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size_vx;
      const int end_bloc = (*(bloc_ptr++)) * line_size_vx;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc * delta_line_size;
      const double* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const double x = *x_ptr;
          // Any shape: pour chaque item de vx, on a delta_line_size items de resu a traiter
          for(int count2 = delta_line_size; count2; count2--)
            {
              double& p_resu = *(resu_ptr++);
              p_resu *= x;
            }
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

static void tab_divide_any_shape_(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt)
{

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  int line_size_vx = vx.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Le line_size du vecteur resu doit etre un multiple du line_size du vecteur vx
  assert(line_size > 0 && line_size_vx > 0 && line_size % line_size_vx == 0);
  const int delta_line_size = line_size / line_size_vx;
  assert(vx.size_totale() * delta_line_size == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  const double *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size_vx;
      const int end_bloc = (*(bloc_ptr++)) * line_size_vx;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc * delta_line_size;
      const double* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const double x = *x_ptr;
          // Any shape: pour chaque item de vx, on a delta_line_size items de resu a traiter
          for(int count2 = delta_line_size; count2; count2--)
            {
              double& p_resu = *(resu_ptr++);
              if (x==0) { Cerr << "Divide by 0 in DoubleVect::tab_divide_any_shape_()" << finl; Process::exit();};
              p_resu *= (1. / x);
            }
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

// Cette methode permettent de multiplier un tableau a plusieurs dimensions par un tableau
//  de dimension inferieure (par exemple un tableau a trois composantes par un tableau a une composante).
//  Chaque valeur du tableau vx est utilisee pour plusieurs items consecutifs du tableau resu
//  (le nombre de fois est le rapport des line_size() des deux tableaux).
//  resu.line_size() doit etre un multiple int de vx.line_size() et les descripteurs doivent etre identiques.
//  Cas particulier: vx peut contenir une constante unique (size_array() == 1 et descripteur nul),
//   dans ce cas c'est un simple produit par la constante
inline void tab_multiply_any_shape(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  if (vx.size_array() == 1 && !vx.get_md_vector().non_nul())
    {
      // Produit par une constante
      double x = vx[0];
      operator_multiply(resu, x, opt);
    }
  else if (vx.line_size() == resu.line_size())
    {
      // Produit membre a membre
      operator_multiply(resu, vx, opt);
    }
  else
    {
      // Cas general
      tab_multiply_any_shape_(resu, vx, opt);
    }
}

inline void operator_divide(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{

  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<double>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  assert(vx.line_size() == line_size);
  assert(vx.size_totale() == vect_size_tot); // this test is necessary if md is null
  assert(vx.get_md_vector() == md);
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  double *resu_base = resu.addr();
  const double *x_base = vx.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      const double* x_ptr = x_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          const double x = *x_ptr;
          double& p_resu = *(resu_ptr++);
          if (x==0) { Cerr << "Divide by 0 in DoubleVect::operator_divide()" << finl; Process::exit();};
          p_resu /= x;
          x_ptr++;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

// Idem que tab_multiply_any_shape() mais avec une division
inline void tab_divide_any_shape(TRUSTVect<double>& resu, const TRUSTVect<double>& vx, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  if (vx.size_array() == 1 && !vx.get_md_vector().non_nul())
    {
      // Produit par une constante
      if (vx[0]==0)
        {
          Cerr << "Divide by 0 in DoubleVect::tab_divide_any_shape()" << finl;
          Process::exit();
        }
      double x = 1. / vx[0];
      operator_multiply(resu, x, opt);
    }
  else if (vx.line_size() == resu.line_size())
    {
      // Produit membre a membre
      operator_divide(resu, vx, opt);
    }
  else
    {
      // Cas general
      tab_divide_any_shape_(resu, vx, opt);
    }
}

template <typename _TYPE_>
inline void operator_divide(TRUSTVect<_TYPE_>& resu, const _TYPE_ x, Mp_vect_options opt = VECT_ALL_ITEMS)
{
  // Master vect donne la structure de reference, les autres vecteurs
  // doivent avoir la meme structure.
  const TRUSTVect<_TYPE_>& master_vect = resu;
  int line_size = master_vect.line_size();
  const MD_Vector& md = master_vect.get_md_vector();
  const int vect_size_tot = master_vect.size_totale();
  // Determine blocs of data to process, depending on " opt"
  int nblocs_left = 1;
  int one_bloc[2];
  const int *bloc_ptr;
  if ( opt != VECT_ALL_ITEMS && md.non_nul())
    {
      assert( opt == VECT_SEQUENTIAL_ITEMS ||  opt == VECT_REAL_ITEMS);
      const ArrOfInt& items_blocs = ( opt == VECT_SEQUENTIAL_ITEMS) ? md.valeur().get_items_to_sum() : md.valeur().get_items_to_compute();
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
  else
    {
      // raccourci pour les tableaux vides (evite le cas particulier line_size == 0)
      return ;
    }
  _TYPE_ *resu_base = resu.addr();
  for (; nblocs_left; nblocs_left--)
    {
      // Get index of next bloc start:
      const int begin_bloc = (*(bloc_ptr++)) * line_size;
      const int end_bloc = (*(bloc_ptr++)) * line_size;
      assert(begin_bloc >= 0 && end_bloc <= vect_size_tot && end_bloc >= begin_bloc);
      double* resu_ptr = resu_base + begin_bloc;
      int count = end_bloc - begin_bloc;
      for (; count; count--)
        {
          _TYPE_& p_resu = *(resu_ptr++);
          if(x==0.) { Cerr << "Error: divide by 0 in operator_divide." << finl; Process::exit();};
          p_resu /= x;
        }
    }
  // In debug mode, put invalid values where data has not been computed
#ifndef NDEBUG
  invalidate_data(resu,  opt);
#endif
  return ;
}

#endif /* TRUSTVect_tools_TPP_included */
