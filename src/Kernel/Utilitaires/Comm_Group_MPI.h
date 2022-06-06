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

#ifndef Comm_Group_MPI_included
#define Comm_Group_MPI_included

#include <Comm_Group_Noparallel.h>
#include <Ref_Comm_Group.h>


//GF comm_incl inclu mpi.h mais il est plus facile de faire un atelier
// ou l on change le mpi si on passe par ce fichier intermediaire

#include <comm_incl.h>

// .DESCRIPTION        :
//  Classe Comm_Group_MPI, derivee de la classe abstraite Comm_Group.
//  Cette classe represente un groupe de communication qui repose sur MPI.
//
//  Construction de l'objet : voir init_group_trio() pour le groupe_TRUST
//   et init_group() pour les autres.
class Comm_Group_MPI : public Comm_Group
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Comm_Group_MPI);
public:
  Comm_Group_MPI();
  ~Comm_Group_MPI() override;
  void abort() const override;

  void mp_collective_op(const double *x, double *resu, int n, Collective_Op op) const override;
  void mp_collective_op(const double *x, double *resu, const Collective_Op *op, int n) const override;
  void mp_collective_op(const int *x, int *resu, int n, Collective_Op op) const override;
  void mp_collective_op(const int *x, int *resu, const Collective_Op *op, int n) const override;

  void barrier(int tag) const override;
  void send_recv_start(const ArrOfInt& send_list,
                       const ArrOfInt& send_size,
                       const char * const * const send_buffers,
                       const ArrOfInt& recv_list,
                       const ArrOfInt& recv_size,
                       char * const * const recv_buffers,
                       TypeHint typehint = CHAR) const override;
  void send_recv_finish() const override;
  void send(int pe, const void *buffer, int size, int tag) const override; // Envoi bloquant
  void recv(int pe, void *buffer, int size, int tag) const override; // Reception bloquante
  void broadcast(void *buffer, int size, int pe_source) const override;
  void all_to_all(const void *src_buffer, void *dest_buffer, int data_size) const override;

#ifdef MPI_
  void init_group_trio();
  void free();
  void all_to_allv(const void *src_buffer, int *send_data_size, int *send_data_offset,
                   void *dest_buffer, int *recv_data_size, int *recv_data_offset) const;
  static void set_trio_u_world(MPI_Comm world);
  static MPI_Comm get_trio_u_world();
  static void set_must_mpi_initialize(int flag);

  void ptop_send_recv(const void * send_buf, int send_buf_size, int send_proc,
                      void * recv_buf, int recv_buf_size, int recv_proc) const;

  void set_must_finalize(int flag)
  {
    must_finalize_=flag;
  };
  MPI_Comm get_mpi_comm() const
  {
    return mpi_comm_;
  };
protected:
  void init_group(const ArrOfInt& pe_list) override;
  void internal_collective(const int *x, int *resu, int nx, const Collective_Op *op, int nop, int level) const;
  void internal_collective(const double *x, double *resu, int nx, const Collective_Op *op, int nop, int level) const;
  int  mppartial_sum(int x) const;

private:
  // Voir set_must_mpi_initialize() et init_group_trio()
  static int must_mpi_initialize_;
  // Le groupe trio_u global est associe a ce communicateur
  //  (different de MPI_COMM_WORLD pour du couplage par exemple)
  static MPI_Comm trio_u_world_;
  static MPI_Status * mpi_status_;
  static MPI_Request * mpi_requests_;
  static int mpi_nrequests_;
  static int mpi_maxrequests_;
  static int current_msg_size_; // La taille des donnees envoyees/recues pour le send_recv_start courant

  MPI_Group mpi_group_;// Handle sur le groupe mpi
  MPI_Comm  mpi_comm_; // Handle sur le communicateur mpi

  int must_finalize_; // Faut-il le faire dans le destructeur ?
  REF(Comm_Group) groupe_pere_;
#endif
};

#endif
