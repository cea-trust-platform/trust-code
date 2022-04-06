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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Schema_Comm.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#include <Schema_Comm.h>
#include <PE_Groups.h>
#include <InOutCommBuffers.h>
#include <Comm_Group.h>
#include <communications.h>
#include <Comm_Group_MPI.h>

// ====================================================================
//                      class Schema_Comm
// ====================================================================

Schema_Comm::Static_Status Schema_Comm::status_ = UNINITIALIZED;
InOutCommBuffers   Schema_Comm::buffers_;
int                Schema_Comm::n_buffers_ = 0;

// Description:
// Accesseur a un membre du tableau obuffers_ (avec verification)
inline OutputCommBuffer& Schema_Comm::obuffer(int pe)
{
  assert(pe >= 0 && pe < n_buffers_);
  return buffers_.obuffers_[pe];
}

// Description:
// Accesseur a un membre du tableau ebuffers_ (avec verification)
inline InputCommBuffer&   Schema_Comm::ebuffer(int pe)
{
  assert(pe >= 0 && pe < n_buffers_);
  return buffers_.ebuffers_[pe];
}

//static const int SET_GROUP_TAG = 0;
static const int BEGIN_COMM_TAG = 1;
static const int ECHANGE_MESSAGES_COMM_TAG = 2;
static const int END_COMM_TAG = 3;
static const int COPY_OPERATOR_TAG = 4;
//static const int CHECK_SEND_RCV_TAG = 5;

// Description:
// Construction d'un nouveau schema de communication.
Schema_Comm::Schema_Comm()
{
  send_pe_list_.set_smart_resize(1);
  recv_pe_list_.set_smart_resize(1);
  me_to_me_ = 0;
  use_all_to_allv_ = 0;
  // Pour verifier plus tard qu'on est toujours dans le bon groupe,
  // on conserve une ref au groupe courant.
  ref_group_ = PE_Groups::current_group();
  if (status_ == UNINITIALIZED)
    {
      n_buffers_ = Process::nproc();
      buffers_.obuffers_ = new OutputCommBuffer[n_buffers_];
      buffers_.ebuffers_ = new InputCommBuffer[n_buffers_];
      status_ = RESET;
    }
}

// Description:
// Destruction d'un schema de communication.
Schema_Comm::~Schema_Comm()
{
}

// Description:
// Constructeur par copie (nouveau schema place en mode RESET).
// Attention : tous les membres du Comm_Group doivent executer
// cette fonction simultanement.
Schema_Comm::Schema_Comm(const Schema_Comm& schema)
{
  send_pe_list_.set_smart_resize(1);
  recv_pe_list_.set_smart_resize(1);
  operator= (schema);
}

// Description:
// Operateur copie : on copie la liste des processeurs qui
// communiquent. Le nouveau schema est place dans l'etat RESET.
// Attention : tous les membres du Comm_Group doivent executer
// cette fonction simultanement.
const Schema_Comm& Schema_Comm::operator=(const Schema_Comm& schema)
{
  assert(status_ == RESET);
  ref_group_ = schema.ref_group_;
  assert(&(ref_group_.valeur()) == &PE_Groups::current_group());
  send_pe_list_ = schema.send_pe_list_;
  recv_pe_list_ = schema.recv_pe_list_;
  me_to_me_     = schema.me_to_me_;
  use_all_to_allv_ = schema.use_all_to_allv_;
  const Comm_Group& group = ref_group_.valeur();
  if (group.check_enabled()) group.barrier(COPY_OPERATOR_TAG);
  return *this;
}

// Description:
// Methode obsolete, le groupe associe au schema est le groupe
// courant au moment ou on cree le schema. L'appel a cette methode
// n'est valide qu'avec le meme groupe que le groupe d'origine.
// La methode ne fait rien.
void Schema_Comm::set_group(const Comm_Group& group)
{
  assert(&group == &(ref_group_.valeur()));
  assert(&group == &PE_Groups::current_group());
}

// Description:
// Renvoie le groupe auquel est associe le schema.
const Comm_Group& Schema_Comm::get_group() const
{
  assert(ref_group_.non_nul());
  return ref_group_.valeur();
}

// Description: Definit la liste des processeurs a qui on va envoyer
// et de qui on va recevoir des donnees.
// Si me_to_me est non nul, on autorise l'envoi des messages a soi-meme,
// sinon non (argument optionnel : par defaut, me_to_me=0)
// Precondition:
//  * Le schema doit etre dans l'etat RESET.
//  * Les tableaux doivent verifier le principe "tu m'ecoutes quand je te parle"
//    (voir commentaire de la classe)
//  * Mon numero de proc ne doit pas etre dans les listes send_pe_list et
//    recv_pe_list (sauf si me_to_me=1)
void Schema_Comm::set_send_recv_pe_list(const ArrOfInt& send_pe_list,
                                        const ArrOfInt& recv_pe_list,
                                        const int     me_to_me)
{
  assert(status_ == RESET);
  send_pe_list_ = send_pe_list;
  recv_pe_list_ = recv_pe_list;
  me_to_me_ = me_to_me;
  // Verification du principe "tu m'ecoutes quand je te parle"
  const Comm_Group& group = ref_group_.valeur();
  assert(&group == &PE_Groups::current_group());
  if (group.check_enabled()) check_send_recv_pe_list();
}

// Description:
// Reserve les buffers de comm pour une nouvelle communication.
// Le schema passe de RESET a WRITING, on a maintenant le droit
// d'appeler send_buffer(). Interdiction d'appeler a nouveau begin_comm
// sur tous les objets de comm avant d'avoir fini cette communication
// avec end_comm().
void Schema_Comm::begin_comm() const
{
  // On verifie qu'une autre communication n'est pas en cours.
  assert (status_ == RESET && ref_group_.non_nul());
  status_ = WRITING;
  // On verifie que tous les membres du groupe executent ceci.
  // Si ca plante ici, c'est que tous les membres declares ne sont
  // pas en train de faire la barriere.
  const Comm_Group& group = ref_group_.valeur();
  assert(&group == &PE_Groups::current_group());
  if (group.check_enabled()) group.barrier(BEGIN_COMM_TAG);
}

static void exchange_data(const ArrOfInt& send_list,
                          const ArrOfInt& send_size,
                          const char * const * const send_buffers,
                          const ArrOfInt& recv_list,
                          const ArrOfInt& recv_size,
                          char * const * const recv_buffers,
                          const Comm_Group& group,
                          bool use_all_to_all)
{
// GF je rajoute le ifdef pour les versions sans MPI comm_group_mpi n'existep pas
#ifdef MPI_
  if (!use_all_to_all || !sub_type(Comm_Group_MPI, group))
#else
  //if (!use_all_to_all || !sub_type(Comm_Group_MPI, group))
  // on n utilise pas le all_to_all car n'exsite pas sans MPI
  if (1)
#endif
    {
      group.send_recv_start(send_list, send_size, send_buffers,
                            recv_list, recv_size, recv_buffers);
      group.send_recv_finish();
    }
  else
    {
#ifdef MPI_
      const int n = group.nproc();
      const int nsend = send_list.size_array();
      const int nrecv = recv_list.size_array();
      // Pack all send data in a single buffer:
      ArrOfInt a_send_size(n);
      ArrOfInt a_send_offset(n);
      ArrOfInt a_recv_size(n);
      ArrOfInt a_recv_offset(n);
      int i, offset;
      // Compute send_size array
      for (i = 0; i < nsend; i++)
        a_send_size[send_list[i]] = send_size[i];
      // Compute send_offset
      for (i = 0, offset = 0; i < n; i++)
        {
          a_send_offset[i] = offset;
          offset += a_send_size[i];
        }
      const int buf_size_send = offset;
      // Compute recv_size array
      for (i = 0; i < nrecv; i++)
        a_recv_size[recv_list[i]] = recv_size[i];
      // Compute recv_offset
      for (i = 0, offset = 0; i < n; i++)
        {
          a_recv_offset[i] = offset;
          offset += a_recv_size[i];
        }
      const int buf_size_recv = offset;
      // Allocate contiguous send and recv buffer:
      char *send_buffer = (char *) malloc(buf_size_send);
      char *recv_buffer = (char *) malloc(buf_size_recv);
      // Copy send data to send buffer
      for (i = 0; i < nsend; i++)
        memcpy(send_buffer + a_send_offset[send_list[i]], // dest
               send_buffers[i], // source
               send_size[i]); // size
      // Exchange data
      ref_cast(Comm_Group_MPI, group).all_to_allv(send_buffer, a_send_size.addr(), a_send_offset.addr(),
                                                  recv_buffer, a_recv_size.addr(), a_recv_offset.addr());
      // Copy recv data to recv_buffers
      for (i = 0; i < nrecv; i++)
        memcpy(recv_buffers[i], // dest
               recv_buffer + a_recv_offset[recv_list[i]], // source
               recv_size[i]); // size
      // [ABN] arrruhhhhhmmmmmm:
      free(send_buffer);
      free(recv_buffer);
#endif
    }
}

// Description:
// Transmet la taille des messages a envoyer aux processeurs qui
// vont les recevoir. La taille est le nombre de bytes des obuffers.
// send_pe_list et recv_pe_list doivent etre initialises.
// Le schema doit etre dans l'etat WRITING.
void Schema_Comm::echange_taille(const ArrOfInt& send_size,
                                 ArrOfInt& recv_size) const
{
  static ArrOfInt send_sz;
  static ArrOfInt recv_sz;
  send_sz.set_smart_resize(1);
  recv_sz.set_smart_resize(1);

  assert(status_ == WRITING && ref_group_.non_nul());
  const Comm_Group& group = ref_group_.valeur();
  assert(&group == &PE_Groups::current_group());

  // On verifie que tous les membres du groupe executent ceci.
  // Si ca plante ici, c'est que tous les membres declares ne sont
  // pas en train de faire la barriere.
  if (group.check_enabled()) group.barrier(ECHANGE_MESSAGES_COMM_TAG);

  const int n_send = send_pe_list_.size_array();
  const int n_recv = recv_pe_list_.size_array();

  recv_size.resize_array(n_recv);

  const char ** send_buffers = new const char* [n_send];
  char ** recv_buffers = new char* [n_recv];

  send_sz.resize_array(n_send);   // Taille d'un int (on echange une taille)
  send_sz = sizeof(int);
  int i;
  for (i = 0; i < n_send; i++)
    send_buffers[i] = (char*) (& send_size[i]);

  recv_sz.resize_array(n_recv);
  recv_sz = sizeof(int);
  recv_size.resize_array(n_recv);
  for (i = 0; i < n_recv; i++)
    recv_buffers[i] = (char*) (& recv_size[i]);

  exchange_data(send_pe_list_, send_sz, send_buffers,
                recv_pe_list_, recv_sz, recv_buffers,
                group,
                use_all_to_allv_);

  delete[] recv_buffers;
  delete[] send_buffers;
}

// Description:
// Cette methode lance l'echange de donnees entre tous les processeurs.
// La taille des messages recus doit etre deja connue.
// Le schema passe de WRITING a EXCHANGED.
void Schema_Comm::echange_messages(const ArrOfInt& send_size,
                                   const ArrOfInt& recv_size) const
{
  assert(status_ == WRITING && ref_group_.non_nul());
  const Comm_Group& group = ref_group_.valeur();
  assert(&group == &PE_Groups::current_group());

  // On verifie que tous les membres du groupe executent ceci.
  // Si ca plante ici, c'est que tous les membres declares ne sont
  // pas en train de faire la barriere.
  if (group.check_enabled()) group.barrier(ECHANGE_MESSAGES_COMM_TAG);

  const int n_send = send_pe_list_.size_array();
  const int n_recv = recv_pe_list_.size_array();
  const char ** send_buffers = new const char* [n_send];
  char ** recv_buffers = new char* [n_recv];

  int i;
  for (i = 0; i < n_recv; i++)
    {
      int pe = recv_pe_list_[i];
      int size = recv_size[i];
      InputCommBuffer& buf = ebuffer(pe);
      recv_buffers[i] = buf.reserve_buffer(size);
    }
  for (i = 0; i < n_send; i++)
    {
      int pe = send_pe_list_[i];
      OutputCommBuffer& buf = obuffer(pe);
      // On verifie que la taille des messages en emission est la meme que
      // celle enregistree dans send_size_, ce qui garantit que la taille
      // en reception est juste elle-aussi.
      assert(send_size[i] == buf.get_buffer_size());
      send_buffers[i] = buf.get_buffer();
    }

  exchange_data(send_pe_list_, send_size, send_buffers,
                recv_pe_list_, recv_size, recv_buffers,
                group,
                use_all_to_allv_);

  delete[] recv_buffers;
  delete[] send_buffers;

  // Creation des input streams a partir des buffers recus
  for (i = 0; i < n_recv; i++)
    {
      int pe = recv_pe_list_[i];
      InputCommBuffer& buf = ebuffer(pe);
      buf.create_stream();
    }

  // Cas particulier des messages envoyes a moi-meme:
  if (me_to_me_)
    {
      int pe = Process::me();
      InputCommBuffer& buf = ebuffer(pe);
      OutputCommBuffer& obuf = obuffer(pe);
      buf.create_stream_from_output_stream(obuf);
    }

  status_ = EXCHANGED;
}

// Description:
// Cette methode lance l'echange de donnees entre tous les processeurs.
// La taille des messages recus n'a pas besoin d'etre connue a priori,
// on la transmet.
// Le schema passe de WRITING a EXCHANGED.
void Schema_Comm::echange_taille_et_messages() const
{
  static ArrOfInt send_size;
  static ArrOfInt recv_size;
  const int n_send = send_pe_list_.size_array();
  send_size.resize_array(n_send);
  int i;
  for (i = 0; i < n_send; i++)
    {
      int pe = send_pe_list_[i];
      send_size[i] = obuffer(pe).get_buffer_size();
    }

  // Methode non optimale: en MPI on pourrait utiliser MPI_Probe mais
  // cette methode me parait peu sure.
  echange_taille(send_size, recv_size);
  echange_messages(send_size, recv_size);
}

// Description:
// Cette methode lance l'echange de donnees. On fournit la taille en octets
// des messages recus dans recv_size (tableau de la meme taille que recv_pe_list)
// En mode check_enabled, on verifie que la taille est correcte
void Schema_Comm::echange_messages(const ArrOfInt& recv_size) const
{
  const int n_send = send_pe_list_.size_array();
  ArrOfInt send_size(n_send);
  int i;
  for (i = 0; i < n_send; i++)
    {
      int pe = send_pe_list_[i];
      send_size[i] = obuffer(pe).get_buffer_size();
    }
  if (PE_Groups::current_group().check_enabled())
    {
      ArrOfInt check_recv_size;
      echange_taille(send_size, check_recv_size);
      if (!(check_recv_size == recv_size))
        {
          Cerr << "Error in Schema_Comm::echange_messages : bad recv_size" << finl;
          Process::exit();
        }
    }
  echange_messages(send_size, recv_size);
}

// Description:
// Vide les buffers et libere les ressources: on
// a fini de lire les donnees recues dans les buffers.
// Le schema passe de EXCHANGED a RESET
void Schema_Comm::end_comm() const
{
  assert(status_ == EXCHANGED && ref_group_.non_nul());
  const Comm_Group& group = ref_group_.valeur();
  assert(&group == &PE_Groups::current_group());

  // On verifie que tous les membres du groupe executent ceci.
  // Si ca plante ici, c'est que tous les membres declares ne sont
  // pas en train de faire la barriere.
  if (group.check_enabled()) group.barrier(END_COMM_TAG);

  int i, n;
  n = send_pe_list_.size_array();
  for (i = 0; i < n; i++)
    {
      int pe = send_pe_list_[i];
      obuffer(pe).clear();
    }
  n = recv_pe_list_.size_array();
  for (i = 0; i < n; i++)
    {
      int pe = recv_pe_list_[i];
      ebuffer(pe).clear();
    }

  if (me_to_me_)
    {
      int pe = Process::me();
      obuffer(pe).clear();
      ebuffer(pe).clear();
    }

  status_ = RESET;
}

#ifndef NDEBUG
static int check_PE_in_list(int num_pe, const ArrOfInt& list)
{
  int i;
  int n = list.size_array();
  for (i = 0; i < n && list[i] != num_pe; i++);
  return (i < n);
}
#endif

// Description: renvoie le buffer correspondant au processeur num_PE pour
// y entasser des donnees a envoyer. Le schema doit etre dans l'etat WRITING.
Sortie& Schema_Comm::send_buffer(int num_PE) const
{
  // Si l'assert suivant plante, c'est qu'on essaie de
  // mettre des donnees dans le buffer en dehors du bloc
  //   begin_comm();
  //    ...
  ///  echange_xxx();
  assert(status_ == WRITING && ref_group_.non_nul());

  // On verifie que le PE demande est bien dans la liste
  // des PEs declares en envoi.
  assert((me_to_me_&&num_PE==Process::me()) || check_PE_in_list(num_PE, send_pe_list_));
  return obuffer(num_PE);
}

// Description: renvoie le buffer correspondant au processeur num_PE pour
// y lire les donnees recues. Le schema doit etre dans l'etat EXCHANGED.
Entree& Schema_Comm::recv_buffer(int num_PE) const
{
  // Si l'assert suivant plante, c'est qu'on essaie de
  // lire des donnees dans les buffers en dehors du bloc
  //   echange_xxx();
  //    ...
  //   end_comm();
  assert(status_ == EXCHANGED && ref_group_.non_nul());
  // On verifie que le PE demande est bien dans la liste
  // des PEs declares en reception.
  assert((me_to_me_&&num_PE==Process::me()) || check_PE_in_list(num_PE, recv_pe_list_));
  return ebuffer(num_PE);
}

const ArrOfInt& Schema_Comm::get_send_pe_list() const
{
  assert(ref_group_.non_nul());
  return send_pe_list_;
}

const ArrOfInt& Schema_Comm::get_recv_pe_list() const
{
  assert(ref_group_.non_nul());
  return recv_pe_list_;
}

// Description:
//  renvoie une reference a un tableau qui contient, pour chaque
//  processeur de send_pe_list_, la taille en bytes des donnees
//  a envoyer.
// A FINIR !!!!
const ArrOfInt& Schema_Comm_statique::get_send_size() const
{
  assert(0);
  assert(status_ == EXCHANGED);
  return send_size_;
}

// Description:
//  renvoie une reference a un tableau qui contient, pour chaque
//  processeur de send_pe_list_, la taille en bytes des donnees
//  a recues.
// A FINIR !!!!
const ArrOfInt& Schema_Comm_statique::get_recv_size() const
{
  assert(0);
  assert(status_ == EXCHANGED);
  return recv_size_;
}

// Description: Verifie que les send/recv_pe_list verifient la propriete
// "tu m'ecoutes quand je te parle"
// Precondition:
// * Statut == RESET
// * Cette fonction doit etre appelee simultanement sur tous les membres du groupe.
void Schema_Comm::check_send_recv_pe_list() const
{
  assert(status_ == RESET);
  // On verifie que les indices de processeurs sont dans le groupe
  int fail1 = 0;
  const int np = Process::nproc();
  const int n1 = send_pe_list_.size_array();
  int i;
  for (i = 0; i < n1; i++)
    if (send_pe_list_[i] < 0 || send_pe_list_[i] >= np)
      fail1 = 1;
  const int n2 = recv_pe_list_.size_array();
  for (i = 0; i < n2; i++)
    if (recv_pe_list_[i] < 0 || recv_pe_list_[i] >= np)
      fail1 = 1;
  int fail2 = 0;
  ArrOfInt recv_list;
  if (!fail1)
    {
      reverse_send_recv_pe_list(send_pe_list_, recv_list);
      // Le tableau recv_pe_list_ n'est pas forcement trie alors que recv_list l'est toujours
      // On trie avant de comparer
      ArrOfInt copie(recv_pe_list_);
      copie.ordonne_array();
      fail2 = !(recv_list == copie);
    }
  if (Process::mp_sum(fail1+fail2))
    {
      if (Process::je_suis_maitre())
        Cerr << "Error in Schema_Comm::check_send_recv_pe_list(), see .log files" << finl;
      Process::Journal() << "Error in Schema_Comm::check_send_recv_pe_list() :\n"
                         << "send_list:\n" << send_pe_list_
                         << "recv_list:\n" << recv_pe_list_;
      if (fail1)
        Process::Journal() << "processor ranks not in current group: current group size = " << np;
      else if (fail2)
        Process::Journal() << "recv_list should be this one:\n" << recv_list << finl;
      else
        Process::Journal() << "OK on this processor" << finl;
      Process::barrier();
      Process::exit();
    }
}
