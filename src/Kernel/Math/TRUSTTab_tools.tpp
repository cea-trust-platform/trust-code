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

#ifndef TRUSTTab_tools_TPP_included
#define TRUSTTab_tools_TPP_included

#include <limits>

template <typename _SIZE_>
inline void local_carre_norme_tab(const TRUSTTab<int,_SIZE_>& , TRUSTArray<int,_SIZE_>& ) = delete;

template <typename _T_, typename _SIZE_>
extern void local_carre_norme_tab(const TRUSTTab<_T_,_SIZE_>& tableau, TRUSTArray<_T_,_SIZE_>& norme_colonne);

template <typename _SIZE_>
inline void mp_carre_norme_tab(const TRUSTTab<int,_SIZE_>& , TRUSTArray<int,_SIZE_>& ) = delete;

template <typename _T_, typename _SIZE_>
inline void mp_carre_norme_tab(const TRUSTTab<_T_,_SIZE_>& tableau, TRUSTArray<_T_,_SIZE_>& norme_colonne)
{
  local_carre_norme_tab(tableau, norme_colonne);
  mp_sum_for_each_item(norme_colonne);
}

template <typename _SIZE_>
inline void mp_norme_tab(const TRUSTTab<int,_SIZE_>& , TRUSTArray<int,_SIZE_>& ) = delete;

template <typename _T_, typename _SIZE_>
inline void mp_norme_tab(const TRUSTTab<_T_,_SIZE_>& tableau, TRUSTArray<_T_,_SIZE_>& norme_colonne)
{
  mp_carre_norme_tab(tableau,norme_colonne);
  for (_SIZE_ c=0; c<norme_colonne.size_array(); c++) norme_colonne[c] = sqrt(norme_colonne[c]);
}

template <typename _SIZE_>
inline void local_max_abs_tab(const TRUSTTab<int,_SIZE_>& , TRUSTArray<int,_SIZE_>& ) = delete;

template <typename _T_, typename _SIZE_>
extern void local_max_abs_tab(const TRUSTTab<_T_,_SIZE_>& tableau, TRUSTArray<_T_,_SIZE_>& max_colonne);

template <typename _SIZE_>
inline void mp_max_abs_tab(const TRUSTTab<int,_SIZE_>& , TRUSTArray<int,_SIZE_>& ) = delete;

template <typename _T_, typename _SIZE_>
inline void mp_max_abs_tab(const TRUSTTab<_T_,_SIZE_>& tableau, TRUSTArray<_T_,_SIZE_>& max_colonne)
{
  local_max_abs_tab(tableau, max_colonne);
  mp_max_for_each_item(max_colonne);
}

#endif /* TRUSTTab_tools_TPP_included */
