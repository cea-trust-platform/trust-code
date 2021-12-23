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
// File:        Convection_Diffusion_Temperature.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Equations
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Convection_Diffusion_Temperature_included
#define Convection_Diffusion_Temperature_included

#include <Convection_Diffusion_std.h>
#include <Ref_Fluide_base.h>
#include <Champ_Fonc.h>
#include <DoubleTabs.h>
Declare_vect(RefObjU);
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_Temperature
//     Cas particulier de Convection_Diffusion_std
//     quand le scalaire subissant le transport est la temperature.
// .SECTION voir aussi
//     Conv_Diffusion_std
//////////////////////////////////////////////////////////////////////////////
class Convection_Diffusion_Temperature : public Convection_Diffusion_std
{
  Declare_instanciable_sans_constructeur(Convection_Diffusion_Temperature);

public :

  Convection_Diffusion_Temperature();
  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  inline void associer_fluide(const Fluide_base& );
  inline const Champ_Inc& inconnue() const override;
  inline Champ_Inc& inconnue() override;
  void discretiser() override;
  const Milieu_base& milieu() const override;
  const Fluide_base& fluide() const;
  Fluide_base& fluide();
  Milieu_base& milieu() override;
  void associer_milieu_base(const Milieu_base& ) override;
  int impr(Sortie& os) const override;
  const Champ_Don& diffusivite_pour_transport() const override;
  const Champ_base& diffusivite_pour_pas_de_temps() const override;
  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  /////////////////////////////////////////////////////

  const Motcle& domaine_application() const override;

  DoubleTab& derivee_en_temps_inco(DoubleTab& ) override;
  DoubleTab& derivee_en_temps_inco_eq_base(DoubleTab& );
  void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) override ;

protected :

  Champ_Inc la_temperature;
  REF(Fluide_base) le_fluide;

//  Champ_Fonc temperature_paroi;
  Champ_Fonc gradient_temperature;

  //Il faudrait creer une liste de Champ_Fonc si on veut utiliser
  //plusieurs temperature de reference
  Champ_Fonc h_echange;

  // Parametres penalisation IBC
  int is_penalized;
  double eta;
  int choix_pena;
  int tag_indic_pena_global;
  IntTab indic_pena_global;
  IntTab indic_face_pena_global;
  VECT(RefObjU) ref_penalisation_L2_FTD;
  VECT(DoubleTab) tab_penalisation_L2_FTD;

  DoubleTab& filtrage_si_appart_ibc(DoubleTab& , DoubleTab&);
  DoubleTab& penalisation_L2(DoubleTab&);
  int verifier_tag_indicatrice_pena_glob();
  int mise_a_jour_tag_indicatrice_pena_glob();
  void set_indic_pena_globale();
  void mise_en_place_zone_fantome(DoubleTab&);
  void calcul_indic_pena_global(IntTab&, IntTab&);
  void transport_ibc(DoubleTrav&, DoubleTab&);
  void ecrire_fichier_pena_th(DoubleTab&, DoubleTab&, DoubleTab&, DoubleTab&);
  // Fin parametres IBC


};


// Description:
//    Associe un fluide incompressible a l'equation.
// Precondition:
// Parametre: Fluide_base& un_fluide
//    Signification: le milieu fluide incompressible a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un milieu associe
inline void Convection_Diffusion_Temperature::associer_fluide(const Fluide_base& un_fluide)
{
  le_fluide = un_fluide;
}


// Description:
//    Renvoie le champ inconnue representant la temperatue.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la temperatue
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
inline const Champ_Inc& Convection_Diffusion_Temperature::inconnue() const
{
  return la_temperature;
}


// Description:
//    Renvoie le champ inconnue representant la temperatue.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la temperatue
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Inc& Convection_Diffusion_Temperature::inconnue()
{
  return la_temperature;
}

#endif
