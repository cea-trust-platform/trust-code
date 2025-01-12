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

#include <MD_Vector_std.h>
#include <stat_counters.h>
#include <Device.h>
#include <sstream>

#ifndef LATATOOLS
template<typename ExecSpace, typename _TYPE_, VECT_ITEMS_TYPE _ITEM_TYPE_>
void vect_items_generic_kernel(int line_size, int idx, int idx_end_of_list, const Static_Int_Lists& list, TRUSTArray<_TYPE_>& vect, TRUSTArray<_TYPE_>& buffer)
{
  static constexpr bool IS_READ = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::READ), IS_WRITE = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::WRITE),
                        IS_ADD = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::ADD), IS_MAX = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::MAX);
  static constexpr bool kernelOnDevice = !std::is_same<ExecSpace, Kokkos::DefaultHostExecutionSpace>::value;

  const int bloc_size = 1;
  const int n = line_size * bloc_size;
  Kokkos::RangePolicy<ExecSpace> policy(idx, idx_end_of_list);
  auto items_to_process_view = list.get_data().template view_ro<1, ExecSpace>();
  if (IS_READ)
    {
      auto buffer_view = buffer.template view_wo<1, ExecSpace>();
      auto vect_view = vect.template view_ro<1, ExecSpace>();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(
                             const int item)
      {
        // Indice de l'item geometrique a copier (ou du premier item du bloc)
        int premier_item_bloc = items_to_process_view[item];
        // Adresse des elements a copier dans le vecteur
        for (int j = 0; j < n; j++)
          {
            int ii = (item - idx) * n + j;
            int jj = premier_item_bloc * line_size + j;
            buffer_view[ii] = vect_view[jj];
          }
      });
    }
  else
    {
      auto buffer_view = buffer.template view_ro<1, ExecSpace>();
      auto vect_view = vect.template view_rw<1, ExecSpace>();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(
                             const int item)
      {
        // Indice de l'item geometrique a copier (ou du premier item du bloc)
        int premier_item_bloc = items_to_process_view[item];
        // Adresse des elements a copier dans le vecteur
        for (int j = 0; j < n; j++)
          {
            int ii = (item - idx) * n + j;
            int jj = premier_item_bloc * line_size + j;
            if (IS_WRITE) vect_view[jj] = buffer_view[ii];
            else if (IS_ADD) vect_view[jj] += buffer_view[ii];
            else if (IS_MAX)
              {
                _TYPE_ dest = vect_view[jj];
                _TYPE_ src = buffer_view[ii];
                vect_view[jj] = (dest > src) ? dest : src;
              }
          }
      });
    }
  end_gpu_timer(__KERNEL_NAME__, kernelOnDevice);
}
#endif

template<typename _TYPE_, VECT_ITEMS_TYPE _ITEM_TYPE_>
void vect_items_generic(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<_TYPE_>& vect, Schema_Comm_Vecteurs& buffers)
{
  assert(line_size > 0);
  const ArrOfInt& index = list.get_index();
  const int nb_voisins = list.get_nb_lists();
  for (int i_voisin = 0; i_voisin < nb_voisins; i_voisin++)
    {
      // Indice dans list.get_data() de la fin de la liste d'items/blocs pour ce voisin:
      const int idx = index[i_voisin];
      const int idx_end_of_list = index[i_voisin + 1];
      // Nombre d'elements de tableau a envoyer/recevoir de ce voisin
      const int nb_elems = (idx_end_of_list - idx) * line_size;
      if (nb_elems>0)
        {
          TRUSTArray<_TYPE_>& buffer = buffers.get_next_area_template<_TYPE_>(voisins[i_voisin], nb_elems);

          assert(nb_elems == buffer.size_array());
          assert(idx_end_of_list <= list.get_data().size_array());
          bool kernelOnDevice = vect.checkDataOnDevice();
          if (kernelOnDevice)
            vect_items_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _ITEM_TYPE_>(line_size, idx, idx_end_of_list, list, vect, buffer);
          else
            vect_items_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _ITEM_TYPE_>(line_size, idx, idx_end_of_list, list, vect, buffer);
        }
    }
}
template void vect_items_generic<double, VECT_ITEMS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<double>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<double, VECT_ITEMS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<double>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<double, VECT_ITEMS_TYPE::ADD>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<double>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<double, VECT_ITEMS_TYPE::MAX>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<double>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<float, VECT_ITEMS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<float>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<float, VECT_ITEMS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<float>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<float, VECT_ITEMS_TYPE::ADD>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<float>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<float, VECT_ITEMS_TYPE::MAX>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<float>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<int, VECT_ITEMS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<int>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<int, VECT_ITEMS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<int>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<int, VECT_ITEMS_TYPE::ADD>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<int>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<int, VECT_ITEMS_TYPE::MAX>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<int>& vect, Schema_Comm_Vecteurs& buffers);
#if INT_is_64_ == 2
template void vect_items_generic<trustIdType, VECT_ITEMS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<trustIdType>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<trustIdType, VECT_ITEMS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<trustIdType>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<trustIdType, VECT_ITEMS_TYPE::ADD>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<trustIdType>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_items_generic<trustIdType, VECT_ITEMS_TYPE::MAX>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<trustIdType>& vect, Schema_Comm_Vecteurs& buffers);
#endif


#ifndef LATATOOLS
template<typename ExecSpace, typename _TYPE_, VECT_BLOCS_TYPE _ITEM_TYPE_>
void vect_blocs_generic_kernel(int line_size, int idx, int idx_end_of_list, const Static_Int_Lists& list, TRUSTArray<_TYPE_>& vect, TRUSTArray<_TYPE_>& buffer)
{
  static constexpr bool IS_READ = (_ITEM_TYPE_ == VECT_BLOCS_TYPE::READ), IS_WRITE = (_ITEM_TYPE_ == VECT_BLOCS_TYPE::WRITE), IS_ADD = (_ITEM_TYPE_ == VECT_BLOCS_TYPE::ADD);
  static constexpr bool kernelOnDevice = !std::is_same<ExecSpace, Kokkos::DefaultHostExecutionSpace>::value;

  const TRUSTArray<int>& items_to_process = list.get_data();
  int ii_base = 0;
  for (int item = idx; item < idx_end_of_list; item += 2)
    {
      // Indice de l'item geometrique a copier (ou du premier item du bloc)
      int premier_item_bloc = items_to_process[item];
      // For blocs, the array contains begin_bloc, end_bloc, begin_bloc, end_bloc...
      const int dernier_item_bloc = items_to_process[item + 1];
      const int bloc_size = dernier_item_bloc - premier_item_bloc;
      // Adresse des elements a copier dans le vecteur
#ifndef TRUST_USE_GPU
      assert(premier_item_bloc >= 0 && bloc_size > 0 &&
             (premier_item_bloc + bloc_size) * line_size <= vect.size_array());
#endif
      const int n = line_size * bloc_size;
      Kokkos::RangePolicy<ExecSpace> policy(0, n);
      auto items_to_process_view = items_to_process.template view_ro<1, ExecSpace>();
      if (IS_READ)
        {
          auto buffer_view = buffer.template view_wo<1, ExecSpace>();
          auto vect_view = vect.template view_ro<1, ExecSpace>();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(
                                 const int j)
          {
            int ii = ii_base * line_size + j;
            int jj = premier_item_bloc * line_size + j;
            buffer_view[ii] = vect_view[jj];
          });
        }
      else
        {
          auto buffer_view = buffer.template view_ro<1, ExecSpace>();
          auto vect_view = vect.template view_rw<1, ExecSpace>();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), policy, KOKKOS_LAMBDA(
                                 const int j)
          {
            int ii = ii_base * line_size + j;
            int jj = premier_item_bloc * line_size + j;
            if (IS_WRITE) vect_view[jj] = buffer_view[ii];
            else if (IS_ADD) vect_view[jj] += buffer_view[ii];
          });
        }
      end_gpu_timer(__KERNEL_NAME__, kernelOnDevice);
      ii_base += bloc_size;
    }
}
#endif

template<typename _TYPE_, VECT_BLOCS_TYPE _ITEM_TYPE_>
void vect_blocs_generic(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<_TYPE_>& vect, Schema_Comm_Vecteurs& buffers)
{
  assert(line_size > 0);
  const ArrOfInt& index = list.get_index();
  const int nb_voisins = list.get_nb_lists();
  for (int i_voisin = 0; i_voisin < nb_voisins; i_voisin++)
    {
      // Nombre d'elements de tableau a envoyer/recevoir de ce voisin
      const int nb_elems = nb_items_par_voisin[i_voisin] * line_size;
      if (nb_elems > 0)
        {
          // Indice dans list.get_data() de la fin de la liste d'items/blocs pour ce voisin:
          const int idx = index[i_voisin];
          const int idx_end_of_list = index[i_voisin + 1];
          TRUSTArray<_TYPE_>& buffer = buffers.get_next_area_template<_TYPE_>(voisins[i_voisin], nb_elems);
          assert(nb_elems == buffer.size_array());
          assert(idx_end_of_list <= list.get_data().size_array());

          bool kernelOnDevice = vect.checkDataOnDevice();
          if (kernelOnDevice)
            vect_blocs_generic_kernel<Kokkos::DefaultExecutionSpace, _TYPE_, _ITEM_TYPE_>(line_size, idx, idx_end_of_list, list, vect, buffer);
          else
            vect_blocs_generic_kernel<Kokkos::DefaultHostExecutionSpace, _TYPE_, _ITEM_TYPE_>(line_size, idx, idx_end_of_list, list, vect, buffer);
        }
    }
}

template void vect_blocs_generic<double, VECT_BLOCS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<double>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<double, VECT_BLOCS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<double>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<double, VECT_BLOCS_TYPE::ADD>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<double>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<float, VECT_BLOCS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<float>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<float, VECT_BLOCS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<float>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<float, VECT_BLOCS_TYPE::ADD>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<float>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<int, VECT_BLOCS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<int>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<int, VECT_BLOCS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<int>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<int, VECT_BLOCS_TYPE::ADD>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<int>& vect, Schema_Comm_Vecteurs& buffers);
#if INT_is_64_ == 2
template void vect_blocs_generic<trustIdType, VECT_BLOCS_TYPE::READ>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<trustIdType>& vect, Schema_Comm_Vecteurs& buffers);
template void vect_blocs_generic<trustIdType, VECT_BLOCS_TYPE::WRITE>(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<trustIdType>& vect, Schema_Comm_Vecteurs& buffers);
#endif
