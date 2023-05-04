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

#include <TRUSTTab.h>
#include <TRUSTTab_tools.tpp>
#include <limits>

// ToDo OpenMP: porter boucle mais mp_norme_tab semble pas utilise
template <typename _T_>
void local_carre_norme_tab(const TRUSTTab<_T_>& tableau, TRUSTArray<_T_>& norme_colonne)
{
  norme_colonne = 0.;
  const TRUSTArray<int>& blocs = tableau.get_md_vector().valeur().get_items_to_sum();
  const int nblocs = blocs.size_array() >> 1;
  const TRUSTVect<_T_>& vect = tableau;
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
              const _T_ x = vect[k++];
              norme_colonne[j] += x*x;
            }
        }
    }
}

template <typename _T_>
void local_max_abs_tab(const TRUSTTab<_T_>& tableau, TRUSTArray<_T_>& max_colonne)
{
  max_colonne = std::numeric_limits<_T_>::min();
  const TRUSTArray<int>& blocs = tableau.get_md_vector().valeur().get_items_to_compute();
  const int nblocs = blocs.size_array() >> 1;
  const TRUSTVect<_T_>& vect = tableau;
  const int lsize = vect.line_size();
  for (int j = 0; j < lsize; j++) max_colonne[j] = 0;
  assert(lsize == max_colonne.size_array());
  bool kernelOnDevice = vect.isKernelOnDevice("local_max_abs_tab(x)");
  const _T_* vect_addr = kernelOnDevice ? mapToDevice(vect) : vect.addr();
  _T_* max_colonne_addr = kernelOnDevice ? computeOnTheDevice(max_colonne) : max_colonne.addr();
  for (int ibloc = 0; ibloc < nblocs; ibloc++)
    {
      const int begin_bloc = blocs[ibloc], end_bloc = blocs[ibloc+1];
      // Necessaire de faire un test sur lsize le compilateur crayCC OpenMP ne supporte pas la reduction sur tableau avec taille dynamique...
      if (lsize==1)
        {
          #pragma omp target teams distribute parallel for if (kernelOnDevice && Objet_U::computeOnDevice) reduction(max:max_colonne_addr[0:1])
          for (int i = begin_bloc; i < end_bloc; i++)
            {
              int k = i * lsize;
              for (int j = 0; j < lsize; j++)
                {
                  const _T_ x = std::fabs(vect_addr[k++]);
                  max_colonne_addr[j] = (x > max_colonne_addr[j]) ? x : max_colonne_addr[j];
                }
            }
        }
      else if (lsize==2)
        {
          #pragma omp target teams distribute parallel for if (kernelOnDevice && Objet_U::computeOnDevice) reduction(max:max_colonne_addr[0:2])
          for (int i = begin_bloc; i < end_bloc; i++)
            {
              int k = i * lsize;
              for (int j = 0; j < lsize; j++)
                {
                  const _T_ x = std::fabs(vect_addr[k++]);
                  max_colonne_addr[j] = (x > max_colonne_addr[j]) ? x : max_colonne_addr[j];
                }
            }
        }
      else if (lsize==3)
        {
          #pragma omp target teams distribute parallel for if (kernelOnDevice && Objet_U::computeOnDevice) reduction(max:max_colonne_addr[0:3])
          for (int i = begin_bloc; i < end_bloc; i++)
            {
              int k = i * lsize;
              for (int j = 0; j < lsize; j++)
                {
                  const _T_ x = std::fabs(vect_addr[k++]);
                  max_colonne_addr[j] = (x > max_colonne_addr[j]) ? x : max_colonne_addr[j];
                }
            }
        }
      else
        for (int i = begin_bloc; i < end_bloc; i++)
          {
            int k = i * lsize;
            for (int j = 0; j < lsize; j++)
              {
                const _T_ x = std::fabs(vect_addr[k++]);
                max_colonne_addr[j] = (x > max_colonne_addr[j]) ? x : max_colonne_addr[j];
              }
          }
      copyFromDevice(max_colonne, "max_colonne in local_max_abs_tab"); // ToDo OpenMP pourquoi necessaire ? Est ce a cause des ecritures put(addr[]) ?
    }
}

template void local_carre_norme_tab<double>(const TRUSTTab<double>& tableau, TRUSTArray<double>& norme_colonne);
template void local_carre_norme_tab<float>(const TRUSTTab<float>& tableau, TRUSTArray<float>& norme_colonne);
template void local_max_abs_tab<double>(const TRUSTTab<double>& tableau, TRUSTArray<double>& max_colonne);
template void local_max_abs_tab<float>(const TRUSTTab<float>& tableau, TRUSTArray<float>& max_colonne);
