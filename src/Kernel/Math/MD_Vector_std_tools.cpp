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
  _TYPE_ *data = vect.addrForDevice();
  const int *items_to_process = list.get_data().addr();
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
      // ToDo OpenMP: calcul du buffer sur le device si vect est sur le device
      _TYPE_ *bufptr = buffer.addr();
      assert(idx_end_of_list <= list.get_data().size_array());
      while (idx < idx_end_of_list)
        {
          // Indice de l'item geometrique a copier (ou du premier item du bloc)
          int premier_item_bloc = items_to_process[idx++];
          const int bloc_size = 1;
          // Adresse des elements a copier dans le vecteur
          assert(premier_item_bloc >= 0 && bloc_size > 0 && (premier_item_bloc + bloc_size) * line_size <= vect.size_array());
          _TYPE_ *vectptr = data + premier_item_bloc * line_size;
          const int n = line_size * bloc_size;
          assert(bufptr + n - buffer.addrForDevice() <= buffer.size_array());
          for (int j = 0; j < n; j++)
            {
              if (IS_READ) *(bufptr++) = *(vectptr++);
              else if (IS_WRITE) *(vectptr++) = *(bufptr++);
              else if (IS_ADD) *(vectptr++) += *(bufptr++);
              else if (IS_MAX)
                {
                  _TYPE_ dest = *vectptr;
                  _TYPE_ src = *(bufptr++);
                  *(vectptr++) = (dest > src) ? dest : src;
                }
              else
                {
                  Cerr << "Unknown VECT_ITEMS_TYPE enum !" << finl;
                  throw;
                }
            }
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
