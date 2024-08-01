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

#include <TRUSTTab.h>
#include <TRUSTTab_tools.tpp>
#include <MD_Vector_seq.h>
#include <limits>

// ToDo OpenMP: porter boucle mais mp_norme_tab semble pas utilise
template <typename _TYPE_>
void local_carre_norme_tab(const TRUSTTab<_TYPE_>& tableau, TRUSTArray<_TYPE_>& norme_colonne)
{
  norme_colonne = 0.;
  const TRUSTArray<int,int>* blocs_p;
  TRUSTArray<int, int> seq_arr;
  if (!sub_type(MD_Vector_seq, tableau.get_md_vector().valeur()))
    blocs_p = &(tableau.get_md_vector()->get_items_to_sum());
  else
    {
      const MD_Vector_seq& md_seq = ref_cast(MD_Vector_seq, tableau.get_md_vector().valeur());
      seq_arr.resize_array(2);
      assert(md_seq.get_nb_items() < std::numeric_limits<int>::max());
      seq_arr[0] = 0;
      seq_arr[1] = static_cast<int>(md_seq.get_nb_items());
      blocs_p = &seq_arr;
    }
  const TRUSTArray<int,int>& blocs = *blocs_p;
  const int nblocs = blocs.size_array() >> 1;
  const TRUSTVect<_TYPE_,int>& vect = tableau;
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
              const _TYPE_ x = vect[k++];
              norme_colonne[j] += x*x;
            }
        }
    }
}

namespace
{
template <typename ExecSpace, typename _TYPE_>
void local_max_abs_tab_kernel(const TRUSTTab<_TYPE_>& tableau, TRUSTArray<_TYPE_>& max_colonne,
                              const TRUSTArray<int>& blocs, int lsize, bool kernelOnDevice)
{
  auto tableau_view= tableau.template view_ro<ExecSpace>();
  auto max_colonne_view= max_colonne.template view_rw<ExecSpace>();

  const int nblocs = blocs.size_array() >> 1;
  for (int ibloc = 0; ibloc < nblocs; ibloc++)
    {
      const int begin_bloc = blocs[ibloc], end_bloc = blocs[ibloc+1];
      // Define a Kokkos range policy based on the execution space
      Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);
      // Parallel loop for any value of lsize, using atomic_max for thread safety
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(const int i)
      {
        for (int j = 0; j < lsize; j++)
          {
            const _TYPE_ x = Kokkos::fabs(tableau_view(i,j));
            Kokkos::atomic_max(&max_colonne_view(j), x);
          }
      });
    }
  end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
}
}

template <typename _TYPE_>
void local_max_abs_tab(const TRUSTTab<_TYPE_>& tableau, TRUSTArray<_TYPE_>& max_colonne)
{
  max_colonne = std::numeric_limits<_TYPE_>::min();
  const TRUSTArray<int>& blocs = tableau.get_md_vector()->get_items_to_compute();
  const int lsize = tableau.line_size();
  for (int j = 0; j < lsize; j++) max_colonne[j] = 0;
  assert(lsize == max_colonne.size_array());

  bool kernelOnDevice = tableau.isDataOnDevice();

  if (kernelOnDevice)
    local_max_abs_tab_kernel<Kokkos::DefaultExecutionSpace, _TYPE_>(tableau, max_colonne, blocs, lsize, kernelOnDevice);
  else
    local_max_abs_tab_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_>(tableau, max_colonne, blocs, lsize, kernelOnDevice);
}
template void local_carre_norme_tab<double>(const TRUSTTab<double,int>& tableau, TRUSTArray<double,int>& norme_colonne);
template void local_carre_norme_tab<float>(const TRUSTTab<float,int>& tableau, TRUSTArray<float,int>& norme_colonne);
template void local_max_abs_tab<double>(const TRUSTTab<double,int>& tableau, TRUSTArray<double,int>& max_colonne);
template void local_max_abs_tab<float>(const TRUSTTab<float,int>& tableau, TRUSTArray<float,int>& max_colonne);
