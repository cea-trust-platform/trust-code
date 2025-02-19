/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Convection_Diffusion_Temperature_base.h>

/*! @brief classe Convection_Diffusion_Temperature Cas particulier de Convection_Diffusion_std
 *
 *      quand le scalaire subissant le transport est la temperature.
 *
 * @sa Conv_Diffusion_std
 */
class Convection_Diffusion_Temperature : public Convection_Diffusion_Temperature_base
{
  Declare_instanciable(Convection_Diffusion_Temperature);
public:

  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  inline const Champ_Inc_base& inconnue() const override { return la_temperature; }
  inline Champ_Inc_base& inconnue() override { return la_temperature; }
  void discretiser() override;
  int preparer_calcul() override;
  void associer_milieu_base(const Milieu_base& ) override;
  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  bool has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const override;
  bool has_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  /////////////////////////////////////////////////////

  double get_time_factor() const override;
  const Motcle& domaine_application() const override;

  DoubleTab& derivee_en_temps_inco(DoubleTab& ) override;
  void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) override ;
  void assembler_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  /* champ convecte : rho * cp * T */
  static void calculer_rho_cp_T(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);

protected :
  OWN_PTR(Champ_Inc_base) la_temperature;
  OWN_PTR(Champ_Fonc_base) gradient_temperature, h_echange;

  // Parametres penalisation IBC
  int is_penalized = 0;
  double eta = 1.0;
  int choix_pena = 0, tag_indic_pena_global = -1;
  IntTab indic_pena_global = 0, indic_face_pena_global = 0;
  VECT(RefObjU) ref_penalisation_L2_FTD;
  DoubleTabs tab_penalisation_L2_FTD;

  DoubleTab& filtrage_si_appart_ibc(DoubleTab& , DoubleTab&);
  DoubleTab& penalisation_L2(DoubleTab&);
  int verifier_tag_indicatrice_pena_glob();
  int mise_a_jour_tag_indicatrice_pena_glob();
  void set_indic_pena_globale();
  void mise_en_place_domaine_fantome(DoubleTab&);
  void calcul_indic_pena_global(IntTab&, IntTab&);
  void transport_ibc(DoubleTrav&, DoubleTab&);
  void ecrire_fichier_pena_th(DoubleTab&, DoubleTab&, DoubleTab&, DoubleTab&);
};

#endif /* Convection_Diffusion_Temperature_included */
