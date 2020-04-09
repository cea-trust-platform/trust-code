/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Comm_Group_Noparallel.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Comm_Group_Noparallel.h>
Implemente_instanciable_sans_constructeur_ni_destructeur(Comm_Group_Noparallel,"Comm_Group_Noparallel",Comm_Group);

int Comm_Group_Noparallel::sending_ = 0;

// Description : constructeur pour le groupe Tous
Comm_Group_Noparallel::Comm_Group_Noparallel()
{
  Comm_Group::init_group_trio(1 /* nproc */, 0 /* rank */);
}
Comm_Group_Noparallel::~Comm_Group_Noparallel()
{
}

// Description: constructeur pour un sous-groupe */
void Comm_Group_Noparallel::init_group(const ArrOfInt& pe_list)
{
  assert(pe_list.size_array() == 1);
  Comm_Group::init_group(pe_list);
}

void Comm_Group_Noparallel::abort() const
{
}

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

// Description: Operation collective sur chaque element d'un tableau de taille n.
//  x et resu doivent pointer sur un tableau de taille n.
void Comm_Group_Noparallel::mp_collective_op(const double *x, double *resu, int n, Collective_Op op) const
{
  if (op != COLL_PARTIAL_SUM)
    for (int i = 0; i < n; i++)
      resu[i] = x[i];
  else
    for (int i = 0; i < n; i++)
      resu[i] = 0.;
}

// Description: Operation collective sur chaque element d'un tableau de taille n.
//  x, resu et op doivent pointer sur un tableau de taille n.
//  Pour chaque element x[i] on effectue l'operation op[i] et on met le resultat dans resu[i]
void Comm_Group_Noparallel::mp_collective_op(const int *x, int *resu, int n, Collective_Op op) const
{
  if (op != COLL_PARTIAL_SUM)
    for (int i = 0; i < n; i++)
      resu[i] = x[i];
  else
    for (int i = 0; i < n; i++)
      resu[i] = 0;
}

// Description: Operation collective sur chaque element d'un tableau de taille n.
//  x, resu et op doivent pointer sur un tableau de taille n.
//  Pour chaque element x[i] on effectue l'operation op[i] et on met le resultat dans resu[i]
void Comm_Group_Noparallel::mp_collective_op(const long long *x, long long *resu, int n, Collective_Op op) const
{
  if (op != COLL_PARTIAL_SUM)
    for (int i = 0; i < n; i++)
      resu[i] = x[i];
  else
    for (int i = 0; i < n; i++)
      resu[i] = 0;
}


// Description: Operation collective sur chaque element d'un tableau de taille n.
//  x et resu doivent pointer sur un tableau de taille n.
void Comm_Group_Noparallel::mp_collective_op(const double *x, double *resu, const Collective_Op *op, int n) const
{
  for (int i = 0; i < n; i++)
    {
      if (op[i] != COLL_PARTIAL_SUM)
        resu[i] = x[i];
      else
        resu[i] = 0.;
    }
}

// Description: Operation collective sur chaque element d'un tableau de taille n.
//  x, resu et op doivent pointer sur un tableau de taille n.
//  Pour chaque element x[i] on effectue l'operation op[i] et on met le resultat dans resu[i]
void Comm_Group_Noparallel::mp_collective_op(const int *x, int *resu, const Collective_Op *op, int n) const
{
  for (int i = 0; i < n; i++)
    {
      if (op[i] != COLL_PARTIAL_SUM)
        resu[i] = x[i];
      else
        resu[i] = 0;
    }
}

// Description: Operation collective sur chaque element d'un tableau de taille n.
//  x, resu et op doivent pointer sur un tableau de taille n.
//  Pour chaque element x[i] on effectue l'operation op[i] et on met le resultat dans resu[i]
void Comm_Group_Noparallel::mp_collective_op(const long long *x, long long *resu, const Collective_Op *op, int n) const
{
  for (int i = 0; i < n; i++)
    {
      if (op[i] != COLL_PARTIAL_SUM)
        resu[i] = x[i];
      else
        resu[i] = 0;
    }
}

void Comm_Group_Noparallel::barrier(int tag) const
{
  return;
}

int Comm_Group_Noparallel::reverse_send_recv_list(const ArrOfInt& src_list,
                                                  ArrOfInt& dest_list) const
{
  assert(src_list.size_array() == 0);
  assert(dest_list.size_array() == 0);
  return 0;
}

void Comm_Group_Noparallel::send_recv_start(const ArrOfInt& send_list,
                                            const ArrOfInt& send_size,
                                            const char * const * const send_buffers,
                                            const ArrOfInt& recv_list,
                                            const ArrOfInt& recv_size,
                                            char * const * const recv_buffers,
                                            TypeHint typehint) const
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

void Comm_Group_Noparallel::send(int pe, const void *buffer, int size, int tag) const
{
  assert(0);
}

void Comm_Group_Noparallel::recv(int pe, void *buffer, int size, int tag) const
{
  assert(0);
}

void Comm_Group_Noparallel::broadcast(void *buffer, int size, int pe_source) const
{
  return;
}

void Comm_Group_Noparallel::all_to_all(const void *src_buffer, void *dest_buffer, int data_size) const
{
  memcpy(dest_buffer, src_buffer, data_size);
}

