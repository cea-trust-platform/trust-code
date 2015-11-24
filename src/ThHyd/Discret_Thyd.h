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
// File:        Discret_Thyd.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/33
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Discret_Thyd_included
#define Discret_Thyd_included

#include <Discret_Thermique.h>
#include <Champ_Don.h>
class Navier_Stokes_std;
class Fluide_Incompressible;
class Fluide_Ostwald;
class Equation_base;
class Champ_Don;
class Convection_Diffusion_Temperature;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Discret_Thyd
//    Cette classe est la classe de base representant une discretisation
//    spatiale appliquee aux problemes de thermo-hydrauliques.
//    Les methodes virtuelles pures sont a implementer dans les classes
//    derivees pour typer et discretiser les champs portes par les
//    equations liees a la discretisation.
// .SECTION voir aussi
//    Discret_Thermique
//    Classe abstraite
//    Methodes abstraites:
//      void vitesse(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const
//      void pression(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const
//      void concentration(const Schema_Temps_base&, Zone_dis&, Champ_Inc&,
//                         int =1) const
//      void proprietes_physiques_fluide_incompressible(Zone_dis& ,
//                         Fluide_Incompressible&, const Champ_Inc& ) const
//      void proprietes_physiques_fluide_Ostwald(Zone_dis& ,Fluide_Ostwald&,
//                                                 const Navier_Stokes_std& ,
//                                                   const Champ_Inc& ) const ;
//
//////////////////////////////////////////////////////////////////////////////
class Discret_Thyd : public Discret_Thermique
{
  Declare_base(Discret_Thyd);

public :

  void vitesse(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const;
  void translation(const Schema_Temps_base&, Zone_dis&, Champ_Fonc&) const;
  void entcor(const Schema_Temps_base&, Zone_dis&, Champ_Fonc&) const;
  void pression(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const;
  void pression_en_pa(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const;
  void divergence_U(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const;
  void gradient_P(const Schema_Temps_base&, Zone_dis&, Champ_Inc&) const;
  void concentration(const Schema_Temps_base&, Zone_dis&,
                     Champ_Inc&, int =1) const;
  virtual void creer_champ_vorticite(const Schema_Temps_base&,const Champ_Inc& ,
                                     Champ_Fonc& ) const;
  virtual void proprietes_physiques_fluide_Ostwald(const Zone_dis& ,Fluide_Ostwald&,
                                                   const Navier_Stokes_std& ,
                                                   const Champ_Inc& ) const ;
  //pour VEF implemente const =0;
  virtual void critere_Q(const Zone_dis& ,const Zone_Cl_dis&,  const Champ_Inc&, Champ_Fonc& ) const;
  inline virtual void reynolds_maille(const Zone_dis&, const Fluide_Incompressible&, const Champ_Inc&, Champ_Fonc&) const
  {
    Cerr << "Reynolds_maille keyword not available for this discretization." << finl;
    exit();
  };
  inline virtual void courant_maille(const Zone_dis&, const Schema_Temps_base&, const Champ_Inc&, Champ_Fonc&) const
  {
    Cerr << "Courant_maille keyword not available for this discretization." << finl;
    exit();
  };
  void porosite_volumique(const Zone_dis&, const Schema_Temps_base&, Champ_Fonc&) const;
  virtual void y_plus(const Zone_dis& ,const Zone_Cl_dis&,  const Champ_Inc&, Champ_Fonc& ) const;
  virtual void grad_T(const Zone_dis& z,const Zone_Cl_dis& zcl, const Champ_Inc& eqn,Champ_Fonc& ch) const;
  virtual void h_conv(const Zone_dis& z,const Zone_Cl_dis& zcl, const Champ_Inc& eqn,Champ_Fonc& ch, Motcle& nom, int temp_ref) const;
  inline virtual void taux_cisaillement(const Zone_dis&, const Zone_Cl_dis& ,const Champ_Inc&, Champ_Fonc&) const
  {
    Cerr << "Taux_cisaillement keyword not available for this discretization." << finl;
    exit();
  };
};

#endif
