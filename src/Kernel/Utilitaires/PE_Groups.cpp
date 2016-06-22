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
// File:        PE_Groups.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////
#include <PE_Groups.h>
#include <Comm_Group.h>
#include <Ref_Comm_Group.h>

// Les trois variables suivantes sauvegardent la pile des groupes
// (voir Comm_Group::enter_group() Comm_Group::current_group() Comm_Group::exit_group() )
// Le haut de la pile est toujours groupe_TRUST(), fourni a initialize()
// groups[0] pointe sur groupe_trio.
static REF(Comm_Group) groups[100];
static int ngroups = 0;
static int max_ngroups = 100;
const Comm_Group * PE_Groups::current_group_ = 0;

int PE_Groups::check_current_group()
{
  assert(ngroups > 0);
  assert(current_group_ == &(groups[ngroups-1].valeur()));
  return 1;
}

// Description: Creation d'un nouveau groupe de processeurs
//  (utilisation possible n'importe ou dans le code)
//  Il faut l'appeler simultanement sur tous les processeurs du groupe current_group()
//  avec le meme tableau liste_pe. liste_pe est la liste des rangs dans le groupe
//  courant des processeurs que l'on veut inclure dans le groupe. Le premier de la
//  liste sera le maitre du groupe. La liste ne doit pas comporter de doublon et
//  doit contenir au moins un processeur.
//  La methode type et initialize l'objet group.
//  Il faut ensuite appeler enter_group() et exit_group() (autant de fois qu'on veut)
void PE_Groups::create_group(const ArrOfInt& liste_pe, DERIV(Comm_Group) & group, int force_Comm_Group_NoParallel)
{
  if (liste_pe.size_array()==1 && force_Comm_Group_NoParallel)
    {
      // On cree un groupe non parallele si c'est possible et si c'est impose
      group.typer("Comm_Group_NoParallel");
    }
  else
    {
      // On cree un groupe du meme type que le groupe_TRUST
      group.typer(groups[0].valeur().que_suis_je());
    }
  group.valeur().init_group(liste_pe);
}

// Description: Si le processeur local appartient au groupe, le groupe courant
//  pour ce processeur devient "group" et on renvoie 1, sinon on renvoie 0.
//  Une reference au groupe actuel est sauvegardee et sera restauree quand on
//  appellera exit_group().
//  Cette methode doit etre appelee simultanement sur tous les processeurs
//  du groupe "group".
//  Attention: a chaque enter_group() doit correspondre un exit_group().
//  Cependant, il n'est pas interdit d'entrer plusieurs fois de suite dans le
//  meme groupe, ni d'entrer dans un groupe plus grand que le groupe actuel.
//  Exemple : group1 et group2 forment une partition du groupe_TRUST()
//
//   int sync_point(int x)
//   {
//     PE_Groups::enter_group(groupe_TRUST());
//     int i = mp_sum(x);
//     PE_Groups::exit_group();
//     return i;
//   }
//   if (PE_Groups::enter_group(group1)) {
//     s1 = mp_sum(x); // Somme sur le groupe 2
//     // Point de synchro avec l'autre groupe:
//     s_all = sync_point(x);
//   } else if (PE_Groups::enter_group(group2)) {
//     s2 = mp_sum(x);
//     // Point de synchro avec l'autre groupe:
//     s_all = sync_point(x);
//   } else {
//     Cerr << "Error: processor " << me() << " is not within a subgroup.";
//     exit();
//   }
//   PE_Groups::exit_group(); // Sort du sous-groupe
int PE_Groups::enter_group(const Comm_Group& group)
{
  assert(&group != &current_group());
  if (ngroups >= max_ngroups-1)
    {
      Cerr << "Comm_Group::enter_group : fatal, too many groups" << finl;
      Process::exit();
    }
  int my_rank_in_group = rank_translate(current_group().rank(), current_group(), group);
  if (my_rank_in_group >= 0)
    {
      // Sauvegarde du pointeur sur le groupe actuel et changement du
      // current_group() :
      groups[ngroups] = group;
      current_group_ = &group;
      ngroups++;
      // Attention, on a change de current_group() !

      // On verifie que tous les processeurs du nouveau groupe sont la:
      if (Comm_Group::check_enabled())
        current_group().barrier(0);

      return 1;
    }
  else
    {
      return 0;
    }
}

// Description: Retourne dans le groupe ou l'on etait avant le dernier
//  enter_group() reussi (dont le resultat a ete 1).
//  Cette methode doit etre appelee simultanement sur tous les processeurs
//  du current_group() actif juste avant exit_group()).
void PE_Groups::exit_group()
{
  if (Comm_Group::check_enabled())
    current_group().barrier(0);

  if (ngroups <= 1)
    {
      Cerr << "Comm_Group::exit_group() error : trying to exit from TRUST main group." << finl;
      Process::exit();
    }
  ngroups--;
  current_group_ = &(groups[ngroups-1].valeur());
}

// Description:
//  Calcule le rank dans le groupe courant du processeur de rang "rank" dans le "group".
//  Il faut que 0 <= rank < group.nproc()
//  Si le processeur en question n'appartient pas au groupe courant, renvoie -1.
int PE_Groups::rank_translate(int rank, const Comm_Group& group,
                              const Comm_Group& dest_group)
{
  const int world_rank = group.world_ranks_[rank];
  const int local_rank = dest_group.local_ranks_[world_rank];
  return local_rank;
}

// Description:
// Renvoie une reference au groupe de tous les processeurs TRUST
const Comm_Group& PE_Groups::groupe_TRUST()
{
  assert(ngroups > 0); // Initialized ?
  return groups[0].valeur();
}

// Description:
//  Methode a appeler au debut de l'execution (MAIN.cpp)
//  Elle initialise current_group() avec groupe_trio_u
void PE_Groups::initialize(const Comm_Group& groupe_trio_u)
{
  assert(ngroups == 0);
  ngroups = 1;
  groups[0] = groupe_trio_u;
  current_group_ = &groupe_trio_u;
}

// Description:
//  Methode a appeler en fin d'execution, une fois qu'on est revenu
//  dans le groupe_TRUST() et juste avant de detruire de Comm_Group
//  principal.
void PE_Groups::finalize()
{
  assert(ngroups == 1);
  groups[0].reset();
  ngroups = 0;
  current_group_ = 0;
}
const int& PE_Groups::get_nb_groups()
{
  return ngroups ;
}
