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


#ifndef Convection_Diffusion_Temperature_included
#define Convection_Diffusion_Temperature_included

#include <Convection_Diffusion_std.h>
#include <Ref_Fluide_base.h>
#include <Fluide_base.h>
#include <Champ_Fonc.h>
#include <TRUSTTabs.h>

Declare_vect(RefObjU);

/*! @brief classe Convection_Diffusion_Temperature Cas particulier de Convection_Diffusion_std
 *
 *      quand le scalaire subissant le transport est la temperature.
 *
 * @sa Conv_Diffusion_std
 */

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
  int preparer_calcul() override;
  const Fluide_base& fluide() const;
  Fluide_base& fluide();
  const Milieu_base& milieu() const override { return fluide(); }
  Milieu_base& milieu() override { return fluide(); }
  void associer_milieu_base(const Milieu_base& ) override;
  const Champ_Don& diffusivite_pour_transport() const override { return milieu().conductivite(); }
  const Champ_base& diffusivite_pour_pas_de_temps() const override { return milieu().diffusivite(); }
  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  /////////////////////////////////////////////////////

  const Motcle& domaine_application() const override;

  DoubleTab& derivee_en_temps_inco(DoubleTab& ) override;
  DoubleTab& derivee_en_temps_inco_eq_base(DoubleTab& );
  void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) override ;
  void assembler_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  /* champ convecte : rho * cp * T */
  static void calculer_rho_cp_T(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  // std::pair<std::string, fonc_calc_t> get_fonc_champ_conserve() const override
  // {
  //   return { "rho_cp_T", calculer_rho_cp_T };
  // }

protected :

  Champ_Inc la_temperature;
  REF(Fluide_base) le_fluide;

  Champ_Fonc gradient_temperature;
  Champ_Fonc h_echange;

  // Parametres penalisation IBC
  int is_penalized;
  double eta;
  int choix_pena;
  int tag_indic_pena_global;
  IntTab indic_pena_global;
  IntTab indic_face_pena_global;
  VECT(RefObjU) ref_penalisation_L2_FTD;
  DoubleTabs tab_penalisation_L2_FTD;

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


inline void Convection_Diffusion_Temperature::associer_fluide(const Fluide_base& un_fluide) { le_fluide = un_fluide; }
inline const Champ_Inc& Convection_Diffusion_Temperature::inconnue() const { return la_temperature; }
inline Champ_Inc& Convection_Diffusion_Temperature::inconnue() { return la_temperature; }

#endif
