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

#include <Comm_Group_Noparallel.h>
#include <cstring>

Implemente_instanciable_sans_constructeur_ni_destructeur(Comm_Group_Noparallel,"Comm_Group_Noparallel",Comm_Group);

int Comm_Group_Noparallel::sending_ = 0;

/*! @brief constructeur pour le groupe Tous
 *
 */
Comm_Group_Noparallel::Comm_Group_Noparallel() { Comm_Group::init_group_trio(1 /* nproc */, 0 /* rank */); }
Comm_Group_Noparallel::~Comm_Group_Noparallel() { }

/*! @brief constructeur pour un sous-groupe //
 *
 */
void Comm_Group_Noparallel::init_group(const ArrOfInt& pe_list)
{
  assert(pe_list.size_array() == 1);
  Comm_Group::init_group(pe_list);
}

void Comm_Group_Noparallel::abort() const { }

Sortie& Comm_Group_Noparallel::printOn(Sortie& os) const
{
  assert(0);
  exit();
  return os;
}

Entree& Comm_Group_Noparallel::readOn(Entree& is)
{
  assert(0);
  exit();
  return is;
}

void Comm_Group_Noparallel::barrier(int tag) const { return; }

int Comm_Group_Noparallel::reverse_send_recv_list(const ArrOfInt& src_list, ArrOfInt& dest_list) const
{
  assert(src_list.size_array() == 0);
  assert(dest_list.size_array() == 0);
  return 0;
}

void Comm_Group_Noparallel::send_recv_start(const ArrOfInt& send_list, const ArrOfInt& send_size, const char *const*const send_buffers, const ArrOfInt& recv_list, const ArrOfInt& recv_size,
                                            char *const*const recv_buffers, TypeHint typehint) const
{
  assert(sending_ == 0);
  if (send_list.size_array() == 1 && send_list[0] == 0 &&
      recv_list.size_array() == 1 && recv_list[0] == 0 &&
      send_size.size_array() == 1 && recv_size.size_array() == 1
      && send_size[0] == recv_size[0])
    {
      memcpy(recv_buffers[0], send_buffers[0], send_size[0]);
    }
  else
    {
      assert(recv_list.size_array() == 0);
      assert(send_list.size_array() == 0);
    }
  sending_ = 1;
  return;
}

void Comm_Group_Noparallel::send_recv_finish() const
{
  assert(sending_);
  sending_ = 0;
}

void Comm_Group_Noparallel::send(int pe, const void *buffer, int size, int tag) const { assert(0); }

void Comm_Group_Noparallel::recv(int pe, void *buffer, int size, int tag) const { assert(0); }

void Comm_Group_Noparallel::broadcast(void *buffer, int size, int pe_source) const { return; }

void Comm_Group_Noparallel::all_to_all(const void *src_buffer, void *dest_buffer, int data_size) const
{
  memcpy(dest_buffer, src_buffer, data_size);
}

void Comm_Group_Noparallel::gather(const void *src_buffer, void *dest_buffer, int data_size, int root) const
{
  memcpy(dest_buffer, src_buffer, data_size);
}

void Comm_Group_Noparallel::all_gather(const void *src_buffer, void *dest_buffer, int data_size) const
{
  memcpy(dest_buffer, src_buffer, data_size);
}

void Comm_Group_Noparallel::all_gatherv(const void *src_buffer, void *dest_buffer, int send_size, const True_int* recv_size, const True_int* displs) const
{
  memcpy(dest_buffer, src_buffer, recv_size[0]);
}

