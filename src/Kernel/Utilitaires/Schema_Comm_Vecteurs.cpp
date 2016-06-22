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
// File:        Schema_Comm_Vecteurs.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#include <Schema_Comm_Vecteurs.h>
#include <Comm_Group.h>
#include <communications.h>
#include <PE_Groups.h>

int Schema_Comm_Vecteurs::buffer_locked_;
ArrOfDouble Schema_Comm_Vecteurs::tmp_area_double_;
ArrOfInt Schema_Comm_Vecteurs::tmp_area_int_;
Schema_Comm_Vecteurs_Static_Data Schema_Comm_Vecteurs::sdata_;

Schema_Comm_Vecteurs_Static_Data::Schema_Comm_Vecteurs_Static_Data()
{
  buffer_base_ = 0;
  buffer_base_size_ = 0;
  buf_pointers_ = 0;
  buf_pointers_size_ = 0;
}

void Schema_Comm_Vecteurs_Static_Data::init(int min_buf_size)
{
  if (buf_pointers_size_ == 0)
    {
      int n = PE_Groups::groupe_TRUST().nproc();
      buf_pointers_size_ = n * 2; // Maximum requis si on echange avec tous les procs dans exchange()
      buf_pointers_ = new char*[n*2];
      for (int i = 0; i < n*2; i++)
        buf_pointers_[i] = 0;
    }
  // Le buffer global a-t-il une taille suffisante ?
  if (buffer_base_size_ < min_buf_size)
    {
      delete [] buffer_base_;
      buffer_base_ = new char[min_buf_size];
      // GF ajout de la mise a zero pour mpiwrapper valgrind mais est ce util ?
      for (int i = 0; i < min_buf_size; i++)
        buffer_base_[i] = 0;
      buffer_base_size_ = min_buf_size;
      Process::Journal() << "Schema_Comm_Vecteurs::end_init() reallocating buffer, size= " << buffer_base_size_ << finl;
    }
}

Schema_Comm_Vecteurs_Static_Data::~Schema_Comm_Vecteurs_Static_Data()
{
  delete[] buffer_base_;
  delete[] buf_pointers_;
}

Schema_Comm_Vecteurs::Schema_Comm_Vecteurs()
{
  status_ = RESET;
  send_buf_sizes_.set_smart_resize(1);
  recv_buf_sizes_.set_smart_resize(1);
  send_procs_.set_smart_resize(1);
  recv_procs_.set_smart_resize(1);
}

Schema_Comm_Vecteurs::~Schema_Comm_Vecteurs()
{
  assert (status_ == END_INIT || status_ == RESET);
}

// Description: Reinitialise les tailles de buffers.
//  Il faut ensuite definir les tailles de buffers avec add_send/recv_area_...()
//  Cette methode doit etre appelee simultanement sur tous les processeurs du groupe.
// Precondition: etat RESET ou END_INIT
// Postcondition: etat BEGIN_INIT
void Schema_Comm_Vecteurs::begin_init()
{
  assert(status_ == END_INIT || status_ == RESET);
  // Reset des tableaux sizes_
  const int np = Process::nproc();
  send_buf_sizes_.resize_array(np, ArrOfInt::NOCOPY_NOINIT);
  send_buf_sizes_ = 0;
  recv_buf_sizes_.resize_array(np, ArrOfInt::NOCOPY_NOINIT);
  recv_buf_sizes_ = 0;
  send_procs_.resize_array(0);
  recv_procs_.resize_array(0);
  sorted_ = 1;
  status_ = BEGIN_INIT;
}

// Description:
//  Une fois les donnees a echanger declarees avec add_send/recv_area_...(),
//  initialise les offset de buffers et alloue un buffer global de taille
//  suffisante.
//  Methode a appeler par tous les processeurs du groupe.
// Precondition: etat BEGIN_INIT
// Postcondtion: etat END_INIT
void Schema_Comm_Vecteurs::end_init()
{
  assert(status_ == BEGIN_INIT);
  assert(Process::nproc() == send_buf_sizes_.size_array());
  // Verification des tailles en emission et en reception:
  if (Comm_Group::check_enabled())
    {
      const int n = send_buf_sizes_.size_array();
      ArrOfInt tmp(n);
      envoyer_all_to_all(send_buf_sizes_, tmp);
      int err = 0;
      for (int i = 0; i < n; i++)
        if (tmp[i] != recv_buf_sizes_[i])
          err++;
      if (Process::mp_sum(err))
        {
          Cerr << "Error in Schema_Comm_Vecteurs::end_init(): send_size_ and recv_size_ don't match, see log files" << finl;
          Process::Journal() << "Error in Schema_Comm_Vecteurs::end_init():\n"
                             << "send_sizes_ = " << send_buf_sizes_
                             << "\n recv_sizes_ = " << recv_buf_sizes_ << finl;
          Process::barrier();
          Process::exit();
        }
    }
  // Tri des numeros de processeurs dans l'ordre croissant
  if (!sorted_)
    {
      send_procs_.ordonne_array();
      recv_procs_.ordonne_array();
    }
  const int nsend = send_procs_.size_array();
  const int nrecv = recv_procs_.size_array();
  int offset = 0;
  int i;
  for (i = 0; i < nsend; i++)
    {
      int pe = send_procs_[i];
      const int size = (send_buf_sizes_[pe]+7)&(~7); // align size on 8 bytes
      offset += size;
      assert(pe >= i); // send_procs_ trie dans l'ordre croissant
      send_buf_sizes_[i] = size;
    }
  for (i = 0; i < nrecv; i++)
    {
      int pe = recv_procs_[i];
      const int size = (recv_buf_sizes_[pe]+7)&(~7); // align size on 8 bytes
      offset += size;
      assert(pe >= i); // recv_procs_ trie dans l'ordre croissant
      recv_buf_sizes_[i] = size;
    }
  min_buf_size_ = offset; // taille du buffer a alouer    recv_buf_offset_[i] = offset;

  send_buf_sizes_.resize_array(nsend);
  recv_buf_sizes_.resize_array(nrecv);
  status_ = END_INIT;
}

// Description:
//  Commence un nouvel echange de donnees (les tailles de buffers
//   doivent avoir ete initialisees avec begin_init() ... end_init())
//  On place les sdata_.buf_pointers_ au debut des buffers pour chaque
//   processeur pour lequel un buffer a ete declare en "send"
//  Apres begin_comm(), il faut remplir les buffers en utilisant
//   get_next_area_int() ou get_next_area_double() dans le meme
//   ordre que celui declare dans la phase d'initialisation,
//   puis appeler exchange()
// Precondition: Le schema doit etre initialise (etat END_INIT),
//   et les buffers non bloques (pas d'autre communication en cours
//   avec ce schema)
// Postcondition: Le schema est dans l'etat BEGIN_COMM, buffers bloques.
void Schema_Comm_Vecteurs::begin_comm()
{
  assert(status_ == END_INIT);
  // Pas un assert car erreur grave et sans doute rare...
  if (buffer_locked_)
    {
      Cerr << "Internal error in Schema_Comm_Vecteurs::begin_comm(): buffers already locked by another communication" << finl;
      Process::exit();
    }
  buffer_locked_ = 1;
  sdata_.init(min_buf_size_);

  // Fait pointer les buffers sur le debut des send_buffers
  char *ptr = sdata_.buffer_base_;

  const int nsend = send_procs_.size_array();
  for (int i = 0; i < nsend; i++)
    {
      const int pe = send_procs_[i];
      sdata_.buf_pointers_[pe] = ptr;
      ptr += send_buf_sizes_[i];
    }
  buffer_locked_ = 1;
  status_ = BEGIN_COMM;
}

void Schema_Comm_Vecteurs::exchange()
{
  assert(status_ == BEGIN_COMM);
  // Verifie que tous les buffers sont pleins
  assert(check_buffers_full());
  // Echange les donnees
  const int nsend = send_procs_.size_array();
  const int nrecv = recv_procs_.size_array();
  // On utilise le tableau sdata_.buf_pointers_ pour stocker les adresses
  //  des buffers a donner a Comm_Group::send_recv_start()
  // (dimensionne a 2*nproc() donc suffisant)
  assert(nsend + nrecv <= sdata_.buf_pointers_size_);
  char ** send_bufs = sdata_.buf_pointers_;
  char ** recv_bufs = sdata_.buf_pointers_ + nsend;
  char * ptr = sdata_.buffer_base_;
  int i;
  for (i = 0; i < nsend; i++)
    {
      send_bufs[i] = ptr;
      ptr += send_buf_sizes_[i];
    }
  char * recv_ptr_base = ptr; // Pour toute a l'heure adresse de base des buffers de reception
  for (i = 0; i < nrecv; i++)
    {
      recv_bufs[i] = ptr;
      ptr += recv_buf_sizes_[i];
    }

  const Comm_Group& group = PE_Groups::current_group();
  // On devrait pouvoir mettre un int64 comme type ici car
  // les buffers sont de alignes sur 8 octets.
  group.send_recv_start(send_procs_, send_buf_sizes_, send_bufs,
                        recv_procs_, recv_buf_sizes_, recv_bufs,
                        Comm_Group::INT);
  group.send_recv_finish();
  // Fait pointer les buffers sur les donnees recues
  ptr = recv_ptr_base;
  for (i = 0; i < nrecv; i++)
    {
      const int pe = recv_procs_[i];
      sdata_.buf_pointers_[pe] = ptr;
      ptr += recv_buf_sizes_[i];
    }
  status_ = EXCHANGED;
}

void Schema_Comm_Vecteurs::end_comm()
{
  assert(status_ == EXCHANGED);
  // Verifie qu'on a bien lu toutes les donnees
  assert(check_buffers_full());
  status_ = END_INIT; // pret pour un nouveau begin_comm()
  buffer_locked_ = 0;
}

// Description: Selon status_, verifie que tous les pointeurs de buffers
//  pointent a la fin du buffer aloue pour chaque processeur en emission
//  ou reception. Renvoie 0 en cas d'erreur (si un buffer n'a pas ete
//  entierement rempli ou vide)
int Schema_Comm_Vecteurs::check_buffers_full() const
{
  char *ptr = sdata_.buffer_base_;
  const int nsend = send_procs_.size_array();
  int i;
  int ok = 1;
  if (status_ == BEGIN_COMM)
    {
      for (i = 0; i < nsend; i++)
        {
          ptr += send_buf_sizes_[i];
          const int pe = send_procs_[i];
          char *ptr2 = sdata_.buf_pointers_[pe];
          ALIGN_SIZE(ptr2, sizeof(double));
          if (ptr != ptr2)
            {
              Cerr << "Internal error in Schema_Comm_Vecteurs::check_buffers_full(): send buffer for processor "
                   << pe << " is not full" << finl;
              ok = 0;
            }
        }
    }
  else if (status_ == EXCHANGED)
    {
      for (i = 0; i < nsend; i++)
        ptr += send_buf_sizes_[i];
      const int nrecv = recv_procs_.size_array();
      for (i = 0; i < nrecv; i++)
        {
          ptr += recv_buf_sizes_[i];
          const int pe = recv_procs_[i];
          char *ptr2 = sdata_.buf_pointers_[pe];
          ALIGN_SIZE(ptr2, sizeof(double));
          if (ptr != ptr2)
            {
              Cerr << "Internal error in Schema_Comm_Vecteurs::check_buffers_full(): recv buffer for processor "
                   << pe << " has not been read entirely" << finl;
              ok = 0;
            }
        }
    }
  else
    {
      Cerr << "check_buffers_full: What ?" << finl;
      Process::exit();
    }
  return ok;
}

// Description: verifie qu'il reste au moins byte_size octets dans le buffer du processeur pe
int Schema_Comm_Vecteurs::check_next_area(int pe, int byte_size) const
{
  assert(byte_size >= 0);
  if (byte_size == 0)
    {
      return 1;
    }
  const ArrOfInt& procs = (status_ == BEGIN_COMM) ? send_procs_ : recv_procs_;
  const ArrOfInt& sizes = (status_ == BEGIN_COMM) ? send_buf_sizes_ : recv_buf_sizes_;
  const int n = procs.size_array();
  int i;
  char * ptr = sdata_.buffer_base_;
  // Si on est en phase de reception, le debut des buffers de reception se trouve a la
  // fin des buffers en emission:
  if (status_ != BEGIN_COMM)
    {
      const int nsend = send_procs_.size_array();
      for (i = 0; i < nsend; i++)
        ptr += send_buf_sizes_[i];
    }
  for (i = 0; i < n; i++)
    {
      ptr += sizes[i]; // pointeur sur la fin du buffer de ce processeur
      if (procs[i] == pe)
        return (sdata_.buf_pointers_[pe] + byte_size) <= ptr;
    }
  // aucun buffer declare pour ce processeur
  return 0;
}
