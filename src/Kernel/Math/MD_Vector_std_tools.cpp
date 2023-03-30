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

#include <MD_Vector_std.h>

template<typename _TYPE_, VECT_ITEMS_TYPE _ITEM_TYPE_>
void vect_items_generic(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, TRUSTArray<_TYPE_>& vect, Schema_Comm_Vecteurs& buffers)
{
  static constexpr bool IS_READ = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::READ), IS_WRITE = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::WRITE),
                        IS_ADD = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::ADD), IS_MAX = (_ITEM_TYPE_ == VECT_ITEMS_TYPE::MAX);

  assert(line_size > 0);
  int idx = 0; // Index in list.get_data()
  const ArrOfInt& index = list.get_index();
  const int nb_voisins = list.get_nb_lists();
  for (int i_voisin = 0; i_voisin < nb_voisins; i_voisin++)
    {
      // Indice dans list.get_data() de la fin de la liste d'items/blocs pour ce voisin:
      const int idx_end_of_list = index[i_voisin + 1];
      // Nombre d'elements de tableau a envoyer/recevoir de ce voisin
      const int nb_elems = (idx_end_of_list - idx) * line_size;
      TRUSTArray<_TYPE_>& buffer = buffers.get_next_area_template < _TYPE_ >(voisins[i_voisin], nb_elems);
      _TYPE_ *vect_addr;
      _TYPE_ *buffer_addr;
      bool kernelOnDevice = vect.isKernelOnDevice("vect.echange_espace_virtuel() 1)vect_items_generic");
      if (kernelOnDevice)
        {
          if (IS_READ)
            {
              const TRUSTArray<_TYPE_>& const_vect = vect;
              vect_addr = const_cast<_TYPE_ *>(mapToDevice(const_vect, "vect"));
              buffer_addr = allocateOnDevice(buffer, "buffer for packing");
            }
          else
            {
              const TRUSTArray<_TYPE_>& const_buffer = buffer;
              buffer_addr = const_cast<_TYPE_ *>(mapToDevice(const_buffer, "buffer for unpacking"));
              vect_addr = computeOnTheDevice(vect, "vect");
            }
        }
      else
        {
          vect_addr = vect.addr();
          buffer_addr = buffer.addr();
        }
      const int * items_to_process_addr = kernelOnDevice ? mapToDevice(list.get_data(), "items_to_process") : list.get_data().addr();
      assert(idx_end_of_list <= list.get_data().size_array());
      #pragma omp target teams distribute parallel for if (kernelOnDevice && Objet_U::computeOnDevice)
      for (int item=idx; item<idx_end_of_list; item++)
        {
          // Indice de l'item geometrique a copier (ou du premier item du bloc)
          int premier_item_bloc = items_to_process_addr[item];
          const int bloc_size = 1;
          // Adresse des elements a copier dans le vecteur
#ifndef _OPENMP
          assert(premier_item_bloc >= 0 && bloc_size > 0 && (premier_item_bloc + bloc_size) * line_size <= vect.size_array());
#endif
          const int n = line_size * bloc_size;
          for (int j = 0; j < n; j++)
            {
              int ii = (item-idx) * n + j;
              int jj = premier_item_bloc * line_size + j;
              if (IS_READ) buffer_addr[ii] = vect_addr[jj];
              else if (IS_WRITE) vect_addr[jj] = buffer_addr[ii];
              else if (IS_ADD) vect_addr[jj] += buffer_addr[ii];
              else if (IS_MAX)
                {
                  _TYPE_ dest = vect_addr[jj];
                  _TYPE_ src = buffer_addr[ii];
                  vect_addr[jj] = (dest > src) ? dest : src;
                }
              else
                {
                  Cerr << "Unknown VECT_ITEMS_TYPE enum !" << finl;
                  throw;
                }
            }
        }
      idx += idx_end_of_list;
      if (IS_READ) copyFromDevice(buffer, "buffer");
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

template<typename _TYPE_, VECT_BLOCS_TYPE _ITEM_TYPE_>
void vect_blocs_generic(const int line_size, const ArrOfInt& voisins, const Static_Int_Lists& list, const ArrOfInt& nb_items_par_voisin, TRUSTArray<_TYPE_>& vect, Schema_Comm_Vecteurs& buffers)
{
  static constexpr bool IS_READ = (_ITEM_TYPE_ == VECT_BLOCS_TYPE::READ), IS_WRITE = (_ITEM_TYPE_ == VECT_BLOCS_TYPE::WRITE), IS_ADD = (_ITEM_TYPE_ == VECT_BLOCS_TYPE::ADD);

  assert(line_size > 0);
  int idx = 0; // Index in list.get_data()
  const ArrOfInt& index = list.get_index();
  const int nb_voisins = list.get_nb_lists();
  for (int i_voisin = 0; i_voisin < nb_voisins; i_voisin++)
    {
      // Indice dans list.get_data() de la fin de la liste d'items/blocs pour ce voisin:
      const int idx_end_of_list = index[i_voisin + 1];
      // Nombre d'elements de tableau a envoyer/recevoir de ce voisin
      const int nb_elems = nb_items_par_voisin[i_voisin] * line_size;
      TRUSTArray<_TYPE_>& buffer = buffers.get_next_area_template < _TYPE_ >(voisins[i_voisin], nb_elems);
      _TYPE_ *vect_addr;
      _TYPE_ *buffer_addr;
      bool kernelOnDevice = vect.isKernelOnDevice("vect.echange_espace_virtuel() 2)vect_blocs_generic()");
      if (kernelOnDevice)
        {
          if (IS_READ)
            {
              const TRUSTArray<_TYPE_>& const_vect = vect;
              vect_addr = const_cast<_TYPE_ *>(mapToDevice(const_vect, "vect"));
              buffer_addr = allocateOnDevice(buffer, "buffer for packing on device");
            }
          else
            {
              const TRUSTArray<_TYPE_>& const_buffer = buffer;
              buffer_addr = const_cast<_TYPE_ *>(mapToDevice(const_buffer, "buffer for unpacking on device)"));
              vect_addr = computeOnTheDevice(vect, "vect");
            }
        }
      else
        {
          vect_addr = vect.addr();
          buffer_addr = buffer.addr();
        }
      const int * items_to_process_addr = kernelOnDevice ? mapToDevice(list.get_data(), "items_to_process") : list.get_data().addr();
      assert(idx_end_of_list <= list.get_data().size_array());
      #pragma omp target teams distribute parallel for if (kernelOnDevice && Objet_U::computeOnDevice)
      for (int item=idx; item<idx_end_of_list; item+=2)
        {
          // Indice de l'item geometrique a copier (ou du premier item du bloc)
          int premier_item_bloc = items_to_process_addr[item];
          // For blocs, the array contains begin_bloc, end_bloc, begin_bloc, end_bloc...
          const int dernier_item_bloc = items_to_process_addr[item+1];
          const int bloc_size = dernier_item_bloc - premier_item_bloc;
          // Adresse des elements a copier dans le vecteur
#ifndef _OPENMP
          assert(premier_item_bloc >= 0 && bloc_size > 0 && (premier_item_bloc + bloc_size) * line_size <= vect.size_array());
#endif
          const int n = line_size * bloc_size;
          for (int j = 0; j < n; j++)
            {
              int ii = (item-idx)/2 * n + j; // ToDo OpenMP check /2 et reecrire boucles
              int jj = premier_item_bloc * line_size + j;
              if (IS_READ) buffer_addr[ii] = vect_addr[jj];
              else if (IS_WRITE) vect_addr[jj] = buffer_addr[ii];
              else if (IS_ADD) vect_addr[jj] += buffer_addr[ii];
              else
                {
                  Cerr << "Unknown VECT_BLOCS_TYPE enum !" << finl;
                  throw;
                }
            }
        }
      idx += idx_end_of_list;
      if (IS_READ) copyFromDevice(buffer, "buffer");
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
