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
#include <Comm_Group.h>
#include <Ref_Comm_Group.h>
#include <communications.h>
#include <PE_Groups.h>
#include <Vect_Deriv_Comm_Group.h>
Implemente_ref(Comm_Group);
Implemente_deriv(Comm_Group);
Implemente_vect(DERIV(Comm_Group));
Implemente_base_sans_constructeur_ni_destructeur(Comm_Group,"Comm_Group",Objet_U);

int Comm_Group::check_enabled_ = 0;
int Comm_Group::static_group_number_ = 0;

Sortie& Comm_Group::printOn(Sortie& os) const
{
  exit();
  return os;
}

Entree& Comm_Group::readOn(Entree& is)
{
  exit();
  return is;
}

Comm_Group::Comm_Group()
{
  static const int group_increment = 32;

  // Les tags des communications sont differents
  // pour chaque groupe tant qu'il n'y a pas plus de 32 groupes.
  // Ainsi, chaque communication de chaque groupe aura un tag different.
  rank_ = -1;
  nproc_ = -1;
  group_number_ = static_group_number_;
  group_tag_increment_ = group_increment;
  static_group_number_++;
  group_communication_tag_ = group_number_ % group_increment;
}

// Description: Le constructeur par copie est interdit !
Comm_Group::Comm_Group(const Comm_Group& a): Objet_U(a)
{
  Cerr << "Comm_Group::Comm_Group(const Comm_Group &) error" << finl;
  exit();
}

// Description: La copie est interdite !
const Comm_Group& Comm_Group::operator=(const Comm_Group&)
{
  exit();
  return *this;
}

// Description: Destructeur (pour l'instant, rien a faire)
Comm_Group::~Comm_Group()
{
}

// Description:
//  Cette fonction doit etre appelee simultanement
//  par tous les PEs du groupe current_group avec les memes parametres.
//  Les processeurs de la pe_list sont les rangs dans current_group() des
//  processeurs du nouveau groupe. Le maitre du groupe est le premier de la liste,
//  le rang du processeur courant, s'il est dans le groupe est determine par
//  son rang dans la liste. Il ne doit pas y avoir de doublon.
//  Cette fonction est appelee par les methodes init_group des classes derivees.
void Comm_Group::init_group(const ArrOfInt& pe_list)
{
  // Tous les processeurs du groupe courant doivent arriver ici
  const Comm_Group& current = PE_Groups::current_group();
  current.barrier(0);

  nproc_ = pe_list.size_array();
  if (nproc_ == 0)
    {
      Cerr << "Comm_Group::set_group_properties : empty process list" << finl;
      exit();
    }
  rank_ = -1;
  const Comm_Group& groupe_trio = PE_Groups::groupe_TRUST();
  local_ranks_.resize_array(groupe_trio.nproc());
  local_ranks_ = -1;
  world_ranks_.resize_array(nproc_);

  for (int i = 0; i < nproc_; i++)
    {
      // rank du pe dans current_group()
      const int pe = pe_list[i];
      if (check_enabled())
        {
          int pe_check = pe;
          envoyer_broadcast(pe_check, 0);
          if (pe_check != pe)
            {
              Cerr << "Comm_Group::set_group_properties : processes have different pe_lists" << finl;
              exit();
            }
        }
      if (pe < 0 || pe >= current.nproc_)
        {
          Cerr << "Comm_Group::set_group_properties : process "
               << pe << " is not in current_group()" << finl;
          exit();
        }
      // rank du pe dans le groupe_TRUST
      const int world_rank = current.world_ranks_[pe];

      if (local_ranks_[world_rank] >= 0)
        {
          Cerr << "Comm_Group::set_group_properties : duplicate pe in pe_list" << finl;
          exit();
        }

      world_ranks_[i] = world_rank;
      local_ranks_[world_rank] = i;
      if (pe == current.rank_)
        rank_ = i;
    }
}

// Description:
//  Initialise le groupe_TRUST().
//  Cette methode est appelee par init_group_trio() des classes derivees
void Comm_Group::init_group_trio(int nproc_tot, int arank)
{
  assert(arank >= 0 && arank < nproc_tot);
  rank_ = arank;
  nproc_ = nproc_tot;
  local_ranks_.resize_array(nproc_);
  world_ranks_.resize_array(nproc_);
  for (int i = 0; i < nproc_; i++)
    {
      local_ranks_[i] = i;
      world_ranks_[i] = i;
    }
}

void Comm_Group::set_check_enabled(int flag)
{
  if (flag)
    check_enabled_ = 1;
  else
    check_enabled_ = 0;
}
