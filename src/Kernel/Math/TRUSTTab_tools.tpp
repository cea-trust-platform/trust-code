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

#ifndef TRUSTTab_tools_TPP_included
#define TRUSTTab_tools_TPP_included

#include <limits>

// NB: all those methods are never used on big (64b) arrays, so the size type is always 'int'.

template <typename _T_>
void local_carre_norme_tab(const TRUSTTab<_T_,int>& tableau, TRUSTArray<_T_,int>& norme_colonne);

template <>
inline void local_carre_norme_tab(const TRUSTTab<int,int>& , TRUSTArray<int,int>& ) = delete;


template <typename _T_>
inline void mp_carre_norme_tab(const TRUSTTab<_T_,int>& tableau, TRUSTArray<_T_,int>& norme_colonne)
{
  local_carre_norme_tab(tableau, norme_colonne);
  mp_sum_for_each_item(norme_colonne);
}

template <>
inline void mp_carre_norme_tab(const TRUSTTab<int,int>& , TRUSTArray<int,int>& ) = delete;


template <typename _T_>
inline void mp_norme_tab(const TRUSTTab<_T_,int>& tableau, TRUSTArray<_T_,int>& norme_colonne)
{
  mp_carre_norme_tab(tableau,norme_colonne);
  for (int c=0; c<norme_colonne.size_array(); c++) norme_colonne[c] = sqrt(norme_colonne[c]);
}

template <>
inline void mp_norme_tab(const TRUSTTab<int,int>& , TRUSTArray<int,int>& ) = delete;


template <typename _T_>
void local_max_abs_tab(const TRUSTTab<_T_,int>& tableau, TRUSTArray<_T_,int>& max_colonne);

template <>
inline void local_max_abs_tab(const TRUSTTab<int,int>& , TRUSTArray<int,int>& ) = delete;


template <typename _T_>
inline void mp_max_abs_tab(const TRUSTTab<_T_,int>& tableau, TRUSTArray<_T_,int>& max_colonne)
{
  local_max_abs_tab(tableau, max_colonne);
  mp_max_for_each_item(max_colonne);
}

/**
 * @brief Compares two `TRUSTTab<double, _SZ_>` objects for equality.
 *
 * Performs an element-wise comparison of the two arrays, using Kokkos for GPU data
 * and a sequential approach for host data. Returns `true` if all elements match, `false` otherwise.
 *
 * @tparam _SZ_ The size type of the `TRUSTTab` object.
 * @param a First array to compare.
 * @param b Second array to compare.
 * @return `true` if the arrays are equal, `false` otherwise.
 */
template <typename _SIZE_>
bool sameDoubleTab(const TRUSTTab<double, _SIZE_>& a, const TRUSTTab<double, _SIZE_>& b)
{
  _SIZE_ size_a = a.size_array();
  _SIZE_ size_b = b.size_array();
  if (size_a != size_b)
    return false;
  bool kernelOnDevice = a.checkDataOnDevice() && b.checkDataOnDevice();
  if (kernelOnDevice)
    {
      auto a_v = static_cast<const ArrOfDouble&>(a).view_ro();
      auto b_v = static_cast<const ArrOfDouble&>(b).view_ro();
      bool same = true;
      Kokkos::parallel_reduce(start_gpu_timer(), size_a, KOKKOS_LAMBDA(const int i, bool& local_same)
      {
        if (a_v(i) != b_v(i)) local_same = false;
      }, Kokkos::LAnd<bool>(same));
      end_gpu_timer(__KERNEL_NAME__);
      return same;
    }
  else
    {
      for (_SIZE_ i = 0; i < size_a; i++)
        if (a.addr()[i] != b.addr()[i])
          return false;
      return true;
    }
}

template <>
inline void mp_max_abs_tab(const TRUSTTab<int,int>& , TRUSTArray<int,int>& ) = delete;

#endif /* TRUSTTab_tools_TPP_included */
