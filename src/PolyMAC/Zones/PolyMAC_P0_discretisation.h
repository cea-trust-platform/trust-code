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
// File:        PolyMAC_P0_discretisation.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PolyMAC_P0_discretisation_included
#define PolyMAC_P0_discretisation_included
//
// .DESCRIPTION class PolyMAC_P0_discretisation (schema de base)
// Classe qui gere la dicretisation PolyMAC_P0 du probleme
//    c'est ici que :
//    * l'on discretise les proprietes physiques du fluide et notamment celui du fluide
//       d'Ostwald( K, N, Mu, ...)
//    * l'on associe le champ d'Ostwald et l'equation hydraulique, le fluide et la
//    zone_dis_base
//    ** ( nouvelles procedures :
//  void proprietes_physiques_fluide_Ostwald(Zone_dis& ,Fluide_Ostwald& ,
//           const Navier_Stokes_std& , const Champ_Inc& ) const;
//   a besoin de la classe Fluide_Ostwald pour avoir acces au fluide etudie
//                         Navier_Stokes_Std pour avoir acces a l'equation hydraulique
// .SECTION voir aussi
// Discret_Thyd_Turb


#include <PolyMAC_discretisation.h>

class PolyMAC_P0_discretisation : public PolyMAC_discretisation
{
  Declare_instanciable(PolyMAC_P0_discretisation);

public :
  //
  // Methodes surchargees de Discretisation_base
  //
  void grad_u(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse,Champ_Fonc& ch) const override;
  void taux_cisaillement(const Zone_dis&, const Zone_Cl_dis& ,const Champ_Inc&, Champ_Fonc&) const override;
  void creer_champ_vorticite(const Schema_Temps_base& ,const Champ_Inc&, Champ_Fonc& ) const override;

private:
  void discretiser_champ_fonc_don(
    const Motcle& directive, const Zone_dis_base& z,
    Nature_du_champ nature,
    const Noms& nom, const Noms& unite,
    int nb_comp, double temps,
    Objet_U& champ) const;
};

#endif
