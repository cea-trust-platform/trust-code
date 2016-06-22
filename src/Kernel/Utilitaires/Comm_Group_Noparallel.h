/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Comm_Group_Noparallel.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Comm_Group_Noparallel_included
#define Comm_Group_Noparallel_included

#include <Comm_Group.h>

class Comm_Group_Noparallel : public Comm_Group
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Comm_Group_Noparallel);
public:
  Comm_Group_Noparallel();
  ~Comm_Group_Noparallel();
  void abort() const;

  void mp_collective_op(const double *x, double *resu, int n, Collective_Op op) const;
  void mp_collective_op(const double *x, double *resu, const Collective_Op *op, int n) const;
  void mp_collective_op(const int *x, int *resu, int n, Collective_Op op) const;
  void mp_collective_op(const int *x, int *resu, const Collective_Op *op, int n) const;

  void   barrier(int tag) const;
  int    reverse_send_recv_list(const ArrOfInt& src_list,
                                ArrOfInt& dest_list) const;

  Comm_Group_Noparallel(const ArrOfInt& pe_list);
  void send_recv_start(const ArrOfInt& send_list,
                       const ArrOfInt& send_size,
                       const char * const * const send_buffers,
                       const ArrOfInt& recv_list,
                       const ArrOfInt& recv_size,
                       char * const * const recv_buffers,
                       TypeHint typehint = CHAR) const;
  void send_recv_finish() const;
  void send(int pe, const void *buffer, int size, int tag) const; // Envoi bloquant
  void recv(int pe, void *buffer, int size, int tag) const; // Reception bloquante
  void broadcast(void *buffer, int size, int pe_source) const;
  void all_to_all(const void *src_buffer, void *dest_buffer, int data_size) const;
protected:
  void init_group(const ArrOfInt& pe_list);
private:

  static int sending_;
};
#endif

