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
// File:        TRUSTTab_tools.tpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTTab_tools_TPP_included
#define TRUSTTab_tools_TPP_included

inline void local_carre_norme_tab(const TRUSTTab<double>& tableau, TRUSTArray<double>& norme_colonne)
{
  const TRUSTArray<int>& blocs = tableau.get_md_vector().valeur().get_items_to_sum();
  const int nblocs = blocs.size_array() >> 1;
  const TRUSTVect<double>& vect = tableau;
  const int lsize = vect.line_size();
  assert(lsize == norme_colonne.size_array());
  for (int ibloc = 0; ibloc < nblocs; ibloc++)
    {
      const int begin_bloc = blocs[ibloc], end_bloc = blocs[ibloc+1];
      for (int i = begin_bloc; i < end_bloc; i++)
        {
          int k = i * lsize;
          for (int j = 0; j < lsize; j++)
            {
              const double x = vect[k++];
              norme_colonne[j] += x*x;
            }
        }
    }
}

inline void mp_carre_norme_tab(const TRUSTTab<double>& tableau, TRUSTArray<double>& norme_colonne)
{
  local_carre_norme_tab(tableau, norme_colonne);
  mp_sum_for_each_item(norme_colonne);
}

inline void mp_norme_tab(const TRUSTTab<double>& tableau, TRUSTArray<double>& norme_colonne)
{
  mp_carre_norme_tab(tableau,norme_colonne);
  for (int c=0; c<norme_colonne.size_array(); c++) norme_colonne[c] = sqrt(norme_colonne[c]);
}

inline void local_max_abs_tab(const TRUSTTab<double>& tableau, TRUSTArray<double>& max_colonne)
{
  const TRUSTArray<int>& blocs = tableau.get_md_vector().valeur().get_items_to_compute();
  const int nblocs = blocs.size_array() >> 1;
  const TRUSTVect<double>& vect = tableau;
  const int lsize = vect.line_size();
  for (int j = 0; j < lsize; j++) max_colonne[j] = 0;
  assert(lsize == max_colonne.size_array());
  for (int ibloc = 0; ibloc < nblocs; ibloc++)
    {
      const int begin_bloc = blocs[ibloc], end_bloc = blocs[ibloc+1];
      for (int i = begin_bloc; i < end_bloc; i++)
        {
          int k = i * lsize;
          for (int j = 0; j < lsize; j++)
            {
              const double x = std::fabs(vect[k++]);
              max_colonne[j] = (x > max_colonne[j]) ? x : max_colonne[j];
            }
        }
    }
}

inline void mp_max_abs_tab(const TRUSTTab<double>& tableau, TRUSTArray<double>& max_colonne)
{
  local_max_abs_tab(tableau, max_colonne);
  mp_max_for_each_item(max_colonne);
}

#endif /* TRUSTTab_tools_TPP_included */
