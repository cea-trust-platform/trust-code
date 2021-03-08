/****************************************************************************
* Copyright (c) 2021, CEA
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
class Front_VF;
class Zone_CoviMAC;
class Faces;
#include <Ref_IntTab.h>
#include <Ref_Front_VF.h>
#include <IntTab.h>
#include <Ref_Champ_Inc.h>
#include <MD_Vector_tools.h>
#include <Op_Diff_CoviMAC_base.h>
#include <vector>

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
  virtual void completer() { }; //non utilise
  virtual int initialiser(double temps);
  virtual void mettre_a_jour(double temps) { }; //non utilise

  REF(Front_VF) fvf, o_fvf; //frontiere dans l'autre probleme
  int i_fvf, i_o_fvf;  //indices de frontiere de chaque cote
  REF(Op_Diff_CoviMAC_base) diff, o_diff; //operateurs de diffusion de chaque cote

  /* elements / face de l'autre cote de la frontiere */
  const IntTab& fe_dist() const //liste (i_face, i_elem) dans la Zone distante
  {
    init_fe_dist();
    return fe_dist_;
  }

  /* positions et poids des points harmoniques : appele par Zone_CoviMAC::harmonic_points */
  void harmonic_points(DoubleTab& xh, DoubleTab& wh, DoubleTab& whm) const;

  /* remplissage de nu.grad T aux faces reelles de la CL : appele par Zone_CoviMAC::fgrad */
  void fgrad(DoubleTab& phif_w, IntTab& phif_d, IntTab& phif_e, DoubleTab& phif_c, IntTab& phif_pe, DoubleTab& phif_pc) const;

protected :
  Nom nom_autre_pb_, nom_bord_, nom_champ_; //nom du probleme distant, du bord, du champ
  double invh_paroi_; //resistance thermique (1 / h) de la paroi
  double t_last_maj_; //dernier temps auquel on a appele mettre_a_jour()

  void init_fe_dist() const; //initialisation
  mutable IntTab fe_dist_;
  mutable int fe_init_;
};
#endif
