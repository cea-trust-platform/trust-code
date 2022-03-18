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
// File:        Echange_contact_CoviMAC.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Cond_Lim
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_contact_CoviMAC_included
#define Echange_contact_CoviMAC_included

#include <Echange_externe_impose.h>
#include <Op_Diff_CoviMAC_Elem.h>
#include <TRUSTTabs_forward.h>
#include <MD_Vector_tools.h>
#include <Ref_Champ_Inc.h>
#include <Ref_Front_VF.h>
#include <Ref_IntTab.h>
#include <vector>

class Zone_CoviMAC;
class Front_VF;
class Faces;

////////////////////////////////////////////////////////////////

//
//  .DESCRIPTION classe : Echange_contact_CoviMAC
//  Outre le champ_front representant la temperature de paroi,
//  cette classe possede un autre champ_front avec autant de valeurs
//  temporelles qui represente la temperature dans l'autre probleme.

////////////////////////////////////////////////////////////////

class Echange_contact_CoviMAC  : public Echange_externe_impose
{
  Declare_instanciable(Echange_contact_CoviMAC);
public :
  int initialiser(double temps) override;
  void mettre_a_jour(double temps) override { }; //non utilise
  void verifie_ch_init_nb_comp() const override { }; //pas de contrainte sur les composantes de chaque cote

  REF(Front_VF) fvf, o_fvf; //frontiere dans l'autre probleme
  int i_fvf, i_o_fvf;  //indices de frontiere de chaque cote
  REF(Op_Diff_CoviMAC_Elem) diff, o_diff; //operateurs de diffusion de chaque cote

  /* faces, sommets de l'autre cote de la frontiere */
  void init_fs_dist() const; //initialisation de f_dist, s_dist
  mutable IntTab f_dist;     //face de l'autre cote de chaque face de la frontiere
  mutable std::map<int, int> s_dist; //s_dist[sommet de ce cote] = sommet de l'autre cote
  mutable int fs_dist_init_ = 0;

  double invh_paroi; //resistance thermique (1 / h) de la paroi

protected :
  Nom nom_autre_pb_, nom_bord_, nom_champ_; //nom du probleme distant, du bord, du champ
};
#endif
