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
// File:        Schema_Comm.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Schema_Comm_included
#define Schema_Comm_included
#include <TRUSTArray.h>
#include <Ref_Comm_Group.h>

// Ces objets stockent un graphe de conversation entre
// processeurs : chaque proc. possede une liste de processeurs a qui
// envoyer et une liste de processeurs de qui recevoir.
//
// Le groupe est l'ensemble des processeurs sur lesquels on GARANTIT
// que les methodes suivantes seront appelees SIMULTANEMENT sur tous
// les processeurs du groupe :
// - begin_comm()
// - set_send_recv_pe_list(...)
// - echange_taille_et_messages()
// - end_comm()
//
// En particulier : Il est interdit d'utiliser ces methodes a
// l'interieur d'une boucle qui n'est pas executee le meme nombre de
// fois par tous les processeurs du groupe, ni a l'interieur d'un
// "if() { }" dont le deroulement n'est pas identique sur tous les
// processeurs du groupe.
//
// Il est interdit de commencer une nouvelle communication pendant
// qu'une autre est en cours (une communication finit toujours par
// "end_comm()")
// Interdiction en particulier d'utiliser "envoyer", "recevoir" et les
// fichiers disques partages entre le premier appel a schema.send_buffer()
// et "terminer()". Faire tres attention a toutes les methodes qu'on utilise
// entre ces deux appels !
//
// Sur un PE donne, une sequence d'echange doit etre construite comme suit:
// schema.begin_comm()
// schema.send_buffer(pe1) << data_to_send;
// schema.send_buffer(pe2) << data_to_send;
// ...
// schema.echanger_taille_et_messages();
// schema.recv_buffer(pe2) >> data_to_recv;
// schema.recv_buffer(pe3) >> data_to_recv;
// ...
// schema.end_comm();
//
// La communication n'est pas forcement symetrique : un processeur peut envoyer
// un message a un processeur et recevoir d'un autre.
// En revanche, l'utilisateur garantit que les listes de processeurs fournies dans
//  send_pe_list et recv_pe_list verifient le principe "tu m'ecoutes quand je te parle !"
//  (c'est a dire qu'un processeur A appartient a send_pe_list sur le processeur B
//   si et seulement si le processeur B appartient a recv_pe_list sur le processeur A).

// Modif BM 20/06/2013: j'ajoute set_all_to_allv_flag. Si le drapeau est mis, le schema de
//  communication utilise MPI_alltoallv au lieu de ISend IRecv. Pour tenter de resoudre
//  les problemes rencontres sur supermuc dans la routine de lecture ecriture fichiers
//  (schema tout le monde ecrit au processeur 0 => erreur d'allocation des MPI_requests).

class Comm_Group;
class Entree;
class Sortie;
class OutputCommBuffer;
class InputCommBuffer;
class InOutCommBuffers;

class Schema_Comm
{
public:
  Schema_Comm();
  Schema_Comm(const Schema_Comm&);
  ~Schema_Comm();

  void set_group(const Comm_Group& group);  // Obsolete
  const Comm_Group& get_group() const;

  const Schema_Comm& operator= (const Schema_Comm&);
  void set_send_recv_pe_list(const ArrOfInt& send_pe_list, const ArrOfInt& recv_pe_list, const int me_to_me = 0);

  void begin_comm() const;                 // Statut passe a WRITING
  // Autorise si status_ == WRITING:
  Sortie& send_buffer(int num_PE) const;
  void echange_taille_et_messages() const; // Statut passe a EXCHANGED
  void echange_messages(const ArrOfInt& recv_size) const;  // Statut passe a EXCHANGED
  // Autorise si status_ == EXCHANGED:
  Entree& recv_buffer(int num_PE) const;
  void end_comm() const;                   // Statut passe a RESET
  // Accesseurs:
  const ArrOfInt& get_send_pe_list() const;
  const ArrOfInt& get_recv_pe_list() const;

  void set_all_to_allv_flag(int x) { use_all_to_allv_ = x; }

protected:
  void echange_taille(const ArrOfInt& send_size, ArrOfInt& recv_size) const;
  void echange_messages(const ArrOfInt& send_size, const ArrOfInt& recv_size) const;
  // Statut passe a EXCHANGED
  void check_send_recv_pe_list() const;

  // Un seul statut pour tous les echanges : on ne supporte pas les
  // acces concurrents a la classe car on veut limiter le nombre
  // de buffers et le nombre d'"outstanding requests".
  // Donc : interdiction de commencer une nouvelle comm si les buffers
  // sont en cours d'utilisation.
  enum Static_Status { UNINITIALIZED, RESET, WRITING, EXCHANGED };
  static Static_Status status_;
  static OutputCommBuffer& obuffer(int pe);
  static InputCommBuffer&   ebuffer(int pe);

  ArrOfInt send_pe_list_; // Liste des processeurs a qui envoyer
  ArrOfInt recv_pe_list_; // Liste des processeurs de qui recevoir
  int   me_to_me_;     // Drapeau: est-ce qu'on autorise a s'envoyer des messages a soi ?
  REF(Comm_Group) ref_group_;// Groupe de processeurs qui vont discuter

  int use_all_to_allv_; // Drapeau, quel type de communication faut-il utiliser ?
private:
  // Les pointeurs sont ranges dans une classe specifique (destructeur
  // des membres statiques appele automatiquement a la fin de l'execution
  // pour liberer la memoire).
  static InOutCommBuffers buffers_;
  static int n_buffers_;
};

class Schema_Comm_statique : public Schema_Comm
{
public:
  //void echange_taille();
  //void echange_messages() const;           // Statut passe a EXCHANGED
  const ArrOfInt& get_send_size() const;
  const ArrOfInt& get_recv_size() const;
protected:
  ArrOfInt send_size_;    // Taille des messages a envoyer en bytes
  ArrOfInt recv_size_;    // Taille des messages a recevoir en bytes
};

#endif
