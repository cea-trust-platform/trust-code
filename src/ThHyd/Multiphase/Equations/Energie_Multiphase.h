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
// File:        Energie_Multiphase.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Equations
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Energie_Multiphase_included
#define Energie_Multiphase_included

#include <Convection_Diffusion_std.h>
#include <Fluide_base.h>
#include <Ref_Fluide_base.h>
#include <Operateur_Evanescence.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Energie_Multiphase
//     Cas particulier de Convection_Diffusion_std pour un fluide quasi conpressible
//     quand le scalaire subissant le transport est la temperature en gaz parfaits,
//     ou l'enthalpie en gaz reels.
//     (generalisation de Convection_Diffusion_Temperature pour les gaz reels)
// .SECTION voir aussi
//     Conv_Diffusion_std Convection_Diffusion_Temperature
//////////////////////////////////////////////////////////////////////////////
class Energie_Multiphase : public Convection_Diffusion_std
{
  Declare_instanciable_sans_constructeur(Energie_Multiphase);

public :

  Energie_Multiphase();
  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

  void associer_fluide(const Fluide_base& );
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
  void dimensionner_matrice_sans_mem(Matrice_Morse& matrice) override;

  /*
    interface {dimensionner,assembler}_blocs
    specificites : prend en compte l'evanescence (en dernier)
  */
  int has_interface_blocs() const override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) override;
  const Motcle& domaine_application() const override;
  void verifie_ch_init_nb_comp_cl(const Champ_Inc_base& ch_ref, const int nb_comp, const Cond_lim_base& cl) const override;

  /* champ convecte : alpha * rho * e */
  static void calculer_alpha_rho_e(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  std::pair<std::string, fonc_calc_t> get_fonc_champ_conserve() const override
  {
    return { "alpha_rho_e", calculer_alpha_rho_e };
  }

  static void calculer_alpha_rho_h(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  virtual std::pair<std::string, fonc_calc_t> get_fonc_champ_convecte() const
  {
    return { "alpha_rho_e_conv", calculer_alpha_rho_e };
  }

  Champ_Inc_base& champ_convecte() const override //par defaut le champ conserve
  {
    return champ_convecte_.valeur();
  }
  int has_champ_convecte() const override
  {
    return champ_convecte_.non_nul();
  }
  void init_champ_convecte() const override; //a appeller dans le completer() des operateurs/sources qui auront besoin de champ_convecte_

protected :

  Champ_Inc l_inco_ch;
  Operateur_Evanescence evanescence;
  REF(Fluide_base) le_fluide;
};




// Description:
//    Renvoie le champ inconnue representant l'inconnue (T ou H)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la temperature (GP)
//      ou l'enthalpie (GR)
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
inline const Champ_Inc& Energie_Multiphase::inconnue() const
{
  return l_inco_ch;
}


// Description:
//    Renvoie le champ inconnue representant l'inconnue (T ou H)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la temperature (GP)
//      ou l'enthalpie (GR)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Inc& Energie_Multiphase::inconnue()
{
  return l_inco_ch;
}

#endif
