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
// File:        PE_Groups.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
#ifndef PE_Groups_included
#define PE_Groups_included
#include <Deriv_Comm_Group.h>
class ArrOfInt;
// .DESCRIPTION        :
//  Cette classe regroupe des fonctions permettant la creation, la
//  destruction et le changement du groupe de processeurs actif.
//  La creation du groupe principal doit etre dans le MAIN.cpp
//  et il faut initialiser PE_Groups en lui donnant le groupe principal.
//  Les autres groupes crees seront du meme type.
//  Voir aussi la class Comm_Group
class PE_Groups
{
public:
  inline static const Comm_Group& current_group();
  static void         create_group(const ArrOfInt& liste_pe, DERIV(Comm_Group) & group, int force_Comm_Group_NoParallel=0);
  static int       enter_group(const Comm_Group& group);
  static void         exit_group();
  static const int& get_nb_groups();
  static int       rank_translate(int rank, const Comm_Group& group,
                                  const Comm_Group& dest_group = current_group());
  static const Comm_Group& groupe_TRUST();

  static void         initialize(const Comm_Group& groupe_trio_u);
  static void         finalize();
private:
  static int check_current_group();
  // This pointer always points to the last element of the "groups" vector in the .cpp file
  static const Comm_Group * current_group_;
};

// Description: renvoie une reference au groupe de processeurs actif courant
inline const Comm_Group& PE_Groups::current_group()
{
  assert(check_current_group());
  return *current_group_;
}

#endif
