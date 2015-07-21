/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Comm_Group.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Comm_Group_included
#define Comm_Group_included

#include <assert.h>
#include <ArrOfInt.h>
#include <Deriv_Comm_Group.h>

// .DESCRIPTION        :
//  Cette classe decrit un groupe de processeurs sur lesquels
//  une portion de code s'execute simultanement. Elle fournit
//  toutes les methodes permettant d'echanger des donnees entre
//  les processeurs du groupe (mpsum, send, recv, ...),
//  et de synchroniser les processeurs (barrier).
//  Elle est specialisee selon le support reseau (MPI, PVM, ...)
//  Attention, ces methodes sont reservees a des operations de bas niveau
//  (noyau TRUST).
//  Dans le code courant, il faut utiliser les methodes classes de communications
//  de haut niveau :
//  (envoyer(), envoyer_broadcast(), class Schema_Comm, class Process, etc)
//  Pour creer un nouveau groupe et l'utiliser, voir class PE_Groups
//  Pour la procedure d'initialisation, voir PE_Groups::Initialize()
class Comm_Group : public Objet_U
{
  Declare_base_sans_constructeur_ni_destructeur(Comm_Group);
public:
  Comm_Group();
  virtual ~Comm_Group();
  virtual void   abort() const = 0;

  // COLL_SUM: somme sur tous les procs
  // COLL_MIN: minimum
  // COLL_MAX: max
  // COLL_PARTIAL_SUM calcule la somme partielle des valeurs sur les processeurs de rang
  // strictement inferieurs a me() (le resultat vaut toujours 0 sur le processeur 0).
  enum Collective_Op { COLL_SUM, COLL_MIN, COLL_MAX, COLL_PARTIAL_SUM };
  virtual void mp_collective_op(const double *x, double *resu, int n, Collective_Op op) const = 0;
  virtual void mp_collective_op(const double *x, double *resu, const Collective_Op *op, int n) const = 0;
  virtual void mp_collective_op(const int *x, int *resu, int n, Collective_Op op) const = 0;
  virtual void mp_collective_op(const int *x, int *resu, const Collective_Op *op, int n) const = 0;
  virtual void barrier(int tag) const = 0;

  // Calcule un nouveau tag de communication qui permet d'identifier les
  // echanges de facon unique pour l'ensemble des groupes.
  inline int get_new_tag() const;

  inline int rank() const;
  inline int nproc() const;

  // Veut-on faire des verifications supplementaires sur les communications ?
  // Ces verifications impliquent des communications en plus, ce qui modifie
  // le deroulement du programme. C'est donc un mecanisme separe des "assert".
  inline static int check_enabled();

  enum TypeHint { CHAR, INT, DOUBLE };
  // Demarre l'echange des buffers.
  // send_list / recv_list = liste de PEs (rangs dans le groupe courant)
  // send_size / recv_size = taille des messages en bytes
  // send_buffers / recv_buffers = adresse des buffers
  // Les buffers en reception doivent avoir une taille suffisante.
  // Note au sujet des const :
  //  send_buffers est completement const, on n'a le droit de rien modifier
  //  recv_buffers est const, recv_buffers[i] est const mais *(recv_buffers[i])
  //               n'est pas const car on y stocke les donnees recues.
  virtual void send_recv_start(const ArrOfInt& send_list,
                               const ArrOfInt& send_size,
                               const char * const * const send_buffers,
                               const ArrOfInt& recv_list,
                               const ArrOfInt& recv_size,
                               char * const * const recv_buffers,
                               TypeHint typehint = CHAR) const = 0;
  // Attend que les communications lancees par send_recv soient terminees.
  virtual void send_recv_finish() const = 0;

  // Methodes d'envoi / reception blocantes: a chaque send doit correpondre
  // simultanement un recv sur le processeur destination.
  virtual void send(int pe, const void *buffer, int size, int tag) const = 0; // Envoi bloquant
  virtual void recv(int pe, void *buffer, int size, int tag) const = 0; // Reception bloquante

  // Methodes de broadcast : a appeler sur tous les processeurs en meme temps
  virtual void broadcast(void *buffer, int size, int pe_source) const = 0;

  // Methodes all_to_all
  virtual void all_to_all(const void *src_buffer, void *dest_buffer, int data_size) const = 0;

  static void set_check_enabled(int flag);
protected:
  Comm_Group(const Comm_Group&);  // interdit !
  const Comm_Group& operator=(const Comm_Group&);   // interdit !
  virtual void       init_group(const ArrOfInt& pe_list);
  void               init_group_trio(int nproc, int rank);
  friend class PE_Groups;

private:
  static int check_enabled_;
  static int static_group_number_;

  // Rang du processeur local dans le groupe, -1 s'il n'est pas dans le groupe
  int rank_;
  // Nombre de processeurs dans le groupe
  int nproc_;
  // Pour chaque pe du calcul complet (taille du tableau = groupe_TRUST().nproc())
  //  indice au sein du groupe si le pe est dedans,
  //  -1 si le pe n'est pas dans le groupe
  ArrOfInt  local_ranks_;
  // Liste des processeurs du groupes (indices des processeurs dans groupe_TRUST())
  // (taille du tableau = nproc_)
  ArrOfInt  world_ranks_;

  // Mon numero de groupe (egal au static_group_number_ au moment de la
  // creation du groupe).
  int group_number_;
  // On incremente le group_communication_tag_ de cette quantite a chaque
  // operation. C'est un nombre premier, ce qui permet d'avoir des tags
  // differents pour chaque groupe pendant un bon bout de temps (jusqu'a ce
  // que le numero de tag depasse MAXINT...)
  int group_tag_increment_;
  // On incremente le tag a chaque operation, ce qui permet de verifier
  // que les processus sont bien synchronises.
  mutable int group_communication_tag_;
};

inline int Comm_Group::check_enabled()
{
  return check_enabled_;
}

// Description:
// Cette fonction renvoie un nouveau tag de communication pour le groupe.
// Effet de bord : incremente le membre group_communication_tag_.
inline int Comm_Group::get_new_tag() const
{
  // B.M. Cette fonctionnalite est finalement tres peu utile en pratique
  // et quand le compteur depasse une limite ca plante mpi. Je desactive:
  //group_communication_tag_ += group_tag_increment_;
  return group_communication_tag_;
}

// Description:
//  Renvoie le rang du processeur local dans le groupe *this.
//  ou -1 si je ne suis pas dans le groupe.
inline int Comm_Group::rank() const
{
  return rank_;
}

// Description:
//  Renvoie le nombre de processeurs dans le groupe *this
inline int Comm_Group::nproc() const
{
  assert(nproc_ >= 0);
  return nproc_;
}

#endif
