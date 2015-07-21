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
// File:        VDF_discretisation.h
// Directory:   $TRUST_ROOT/src/VDF/Zones
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#ifndef VDF_discretisation_included
#define VDF_discretisation_included

//
// .DESCRIPTION class VDF_discretisation
// classe qui gere la dicretisation VDF du probleme
//    c'est ici que :
//    * l'on discretise les proprietes physiques du fluide et notemment celui du fluide
//       d'Ostwald( K, N, Mu, ...)
//    * l'on associe le champ d'Ostwald, le fluide et la zone_dis_base
//    ** ( nouvelles procedures :
//  void proprietes_physiques_fluide_Ostwald(Zone_dis& ,Fluide_Ostwald& ,
//           const Navier_Stokes_std& , const Champ_Inc& ) const;
//   a besoin de la classe Fluide_Ostwald pour avoir acces au fluide etudie
//                         Navier_Stokes_Std pour avoir acces a l'equation hydraulique
//                         (donc a la vitesse
// .SECTION voir aussi
// Discret_Thyd_Turb


#include <Discret_Thyd.h>

class Champ_Don;
class Navier_Stokes_std;
class Fluide_Incompressible;
class Fluide_Ostwald;
class Convection_Diffusion_Temperature;
class Schema_Temps_base;

class VDF_discretisation : public Discret_Thyd
{
  Declare_instanciable(VDF_discretisation);

public:
  //
  // Methodes surchargees de Discretisation_base
  //
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, int nb_pas_dt, double temps,
                         Champ_Inc& champ) const;
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, double temps,
                         Champ_Fonc& champ) const;
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, double temps,
                         Champ_Don& champ) const;

  void zone_Cl_dis(Zone_dis& z, Zone_Cl_dis& zcl) const;

  void proprietes_physiques_fluide_Ostwald(Zone_dis& ,Fluide_Ostwald& ,
                                           const Navier_Stokes_std& ,
                                           const Champ_Inc& ) const;
  void vorticite(Zone_dis& ,const Champ_Inc& , Champ_Fonc& ) const;
  void creer_champ_vorticite(const Schema_Temps_base& ,const Champ_Inc&, Champ_Fonc& ) const;
  void critere_Q(const Zone_dis& ,const Zone_Cl_dis&,const Champ_Inc&, Champ_Fonc& ) const;
  void reynolds_maille(const Zone_dis&, const Fluide_Incompressible&, const Champ_Inc&, Champ_Fonc&) const;
  void courant_maille(const Zone_dis&, const Schema_Temps_base&, const Champ_Inc&, Champ_Fonc&) const;
  void taux_cisaillement(const Zone_dis&, const Zone_Cl_dis&,const Champ_Inc&, Champ_Fonc&) const;
  void y_plus(const Zone_dis& ,const Zone_Cl_dis&,const Champ_Inc&, Champ_Fonc& ) const;
//  virtual void t_paroi(const Zone_dis& z,const Zone_Cl_dis& zcl, const Equation_base& eqn,Champ_Fonc& ch) const;

private:
  void discretiser_champ_fonc_don(const Motcle& directive, const Zone_dis_base& z,
                                  Nature_du_champ nature, const Noms& noms, const Noms& unites,
                                  int nb_comp, double temps, Objet_U& champ) const;
  void modifier_champ_tabule(const Zone_dis_base& zone_dis,Champ_Fonc_Tabule& ch_tab,const Champ_base& ch_inc) const;
};

#endif
