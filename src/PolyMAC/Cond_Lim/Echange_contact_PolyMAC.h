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
// File:        Echange_contact_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Cond_Lim
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_contact_PolyMAC_included
#define Echange_contact_PolyMAC_included

#include <Echange_externe_impose.h>
#include <TRUSTTabs_forward.h>
#include <MD_Vector_tools.h>
#include <Ref_Champ_Inc.h>
#include <Ref_IntTab.h>
#include <vector>

class Zone_PolyMAC;
class Front_VF;
class Faces;

//  .DESCRIPTION classe : Echange_contact_PolyMAC
//  Outre le champ_front representant la temperature de paroi,
//  cette classe possede un autre champ_front avec autant de valeurs
//  temporelles qui represente la temperature dans l'autre probleme.
class Echange_contact_PolyMAC  : public Echange_externe_impose
{
  Declare_instanciable(Echange_contact_PolyMAC);
public :
  void completer() override;
  int initialiser(double temps) override;
  void calculer_correspondance();
  void update_coeffs();
  void update_delta() const;
  void mettre_a_jour(double ) override;
  inline Champ_front& T_autre_pb() { return T_autre_pb_; }
  inline const Champ_front& T_autre_pb() const { return T_autre_pb_; }
  inline const Nom& nom_autre_pb() const { return nom_autre_pb_; }

  //item(i, j) : indice du j-ieme item dont on a besoin pour la face i de la frontiere
  mutable IntTab item;

  //coeff(i, j) : coefficient de la face, puis coefficient de item(i, j - 1) (element, puis autres faces) dans la formule du flux a la face
  //delta(i, j, 0/1) -> idem pour la correction non-lineaire de Le Potier
  mutable DoubleTab coeff, delta_int, delta;
  int monolithic; //1 si on resout la thermique en monolithique
protected :
  int stab_; //1 si on utilise la stabilisation de Le Potier
  mutable int coeffs_a_jour_, delta_a_jour_; //dernier temps auquel on a mis a jour les coeffs
  double h_paroi;
  Champ_front T_autre_pb_;
  Nom nom_autre_pb_;
};

#endif
