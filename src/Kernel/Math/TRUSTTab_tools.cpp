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

#include <TRUSTTab.h>
#include <TRUSTTab_tools.tpp>
#include <MD_Vector_seq.h>
#include <limits>

template <typename _TYPE_>
void local_carre_norme_tab(const TRUSTTab<_TYPE_>& tableau, TRUSTArray<_TYPE_>& norme_colonne)
{
  norme_colonne = 0.;

  const TRUSTVect<_TYPE_,int>& vect = tableau;
  const int lsize = vect.line_size(), vect_size_tot = vect.size_totale();
  assert(lsize == norme_colonne.size_array());

  int nblocs_left;
  Block_Iter<int> bloc_itr = ::determine_blocks(VECT_SEQUENTIAL_ITEMS, tableau.get_md_vector(), vect_size_tot, lsize, nblocs_left);

  for (; nblocs_left; nblocs_left--)
    {
      const int begin_bloc = (*(bloc_itr++)), end_bloc = (*(bloc_itr++));
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
void local_max_abs_tab_kernel(const TRUSTTab<_TYPE_>& tableau, TRUSTArray<_TYPE_>& max_colonne)
{
  const TRUSTVect<_TYPE_,int>& vect = tableau;
  const int lsize = vect.line_size(), vect_size_tot = vect.size_totale();
  assert(lsize == max_colonne.size_array());

  int nblocs_left;
  Block_Iter<int> bloc_itr = ::determine_blocks(VECT_REAL_ITEMS, tableau.get_md_vector(), vect_size_tot, lsize, nblocs_left);

  for (int j = 0; j < lsize; j++) max_colonne[j] = 0;
  assert(lsize == max_colonne.size_array());

  auto tableau_view= tableau.template view_ro<2, ExecSpace>();
  auto max_colonne_view= max_colonne.template view_rw<1, ExecSpace>();

  for (; nblocs_left; nblocs_left--)
    {
      const int begin_bloc = (*(bloc_itr++)), end_bloc = (*(bloc_itr++));
      if (begin_bloc<end_bloc) // very important: empty bloc at the end would erase max_colonne
        {
          Kokkos::RangePolicy<ExecSpace> policy(begin_bloc, end_bloc);

          for (int j=0; j<lsize; j++) //Outer loop
            {
              if (timer) start_gpu_timer(__KERNEL_NAME__);
              Kokkos::parallel_reduce(policy,
                                      KOKKOS_LAMBDA(const int i, _TYPE_& local_max)
              {
                const _TYPE_ x = Kokkos::fabs(tableau_view(i,j));
                local_max = Kokkos::fmax(local_max, x);
              },
              //Reduce in a subview, enabled by specifying execspace in the reducer !
              Kokkos::Max<_TYPE_, ExecSpace>(Kokkos::subview(max_colonne_view,j)));

              bool kernelOnDevice = is_default_exec_space<ExecSpace>;
              if (timer) end_gpu_timer(__KERNEL_NAME__, kernelOnDevice);
            }
        }
    }
}

}

template <typename _TYPE_>
void local_max_abs_tab(const TRUSTTab<_TYPE_>& tableau, TRUSTArray<_TYPE_>& max_colonne)
{
  max_colonne = std::numeric_limits<_TYPE_>::min();
  bool kernelOnDevice = tableau.checkDataOnDevice();

  if (kernelOnDevice)
    local_max_abs_tab_kernel<Kokkos::DefaultExecutionSpace, _TYPE_>(tableau, max_colonne);
  else
    local_max_abs_tab_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_>(tableau, max_colonne);
}

template void local_carre_norme_tab<double>(const TRUSTTab<double,int>& tableau, TRUSTArray<double,int>& norme_colonne);
template void local_carre_norme_tab<float>(const TRUSTTab<float,int>& tableau, TRUSTArray<float,int>& norme_colonne);
template void local_max_abs_tab<double>(const TRUSTTab<double,int>& tableau, TRUSTArray<double,int>& max_colonne);
template void local_max_abs_tab<float>(const TRUSTTab<float,int>& tableau, TRUSTArray<float,int>& max_colonne);
