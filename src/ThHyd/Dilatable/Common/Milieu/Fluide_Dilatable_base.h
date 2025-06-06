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

#ifndef Fluide_Dilatable_base_included
#define Fluide_Dilatable_base_included

#include <EDO_Pression_th_base.h>
#include <EOS_Tools_base.h>

#include <Loi_Etat_base.h>
#include <Fluide_base.h>
#include <TRUST_Deriv.h>
#include <TRUST_Ref.h>


class Probleme_base;

/*! @brief classe Fluide_Dilatable_base Cette classe represente un d'un fluide dilatable,
 *
 *     heritant de fluide base
 *
 * @sa Milieu_base Fluide_base
 */

class Fluide_Dilatable_base : public Fluide_base
{
  Declare_base(Fluide_Dilatable_base);
public :
  void verifier_coherence_champs(int& err,Nom& message) override;
  void set_Cp(double);
  void update_rho_cp(double temps) override;
  void creer_champs_non_lus() override;
  void initialiser_radiatives(const double temps);
  void warn_syntax_Sutherland();
  int initialiser(const double temps) override;

  const DoubleTab& temperature() const;
  const Champ_Don_base& ch_temperature() const;
  Champ_Don_base& ch_temperature();

  virtual void calculer_pression_tot();
  void preparer_pas_temps();
  void abortTimeStep() override;
  void set_param(Param& param) override;
  void discretiser(const Probleme_base& pb, const  Discretisation_base& dis) override;
  void mettre_a_jour(double ) override;
  virtual void update_pressure_fields(double );
  void preparer_calcul() override;
  virtual void completer(const Probleme_base&);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual void checkTraitementPth(const Domaine_Cl_dis_base&);
  void prepare_pressure_edo();
  virtual void write_mean_edo(double);
  virtual void write_header_edo();

  // Virtuelles pure
  virtual void secmembre_divU_Z(DoubleTab& ) const=0;
  virtual void Resoudre_EDO_PT()=0;

  // Methodes de l interface des champs postraitables
  const Champ_base& get_champ(const Motcle& nom) const override;
  void creer_champ(const Motcle& motlu) override;
  bool has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const override;
  bool has_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;

  // Methodes inlines
  inline const Nom type_fluide() const { return loi_etat_->type_fluide(); }
  inline const OWN_PTR(Loi_Etat_base)& loi_etat() const { return loi_etat_; }
  inline OWN_PTR(Loi_Etat_base)&  loi_etat() { return loi_etat_; }
  inline const Champ_Inc_base& inco_chaleur() const { return ch_inco_chaleur_.valeur(); }
  inline Champ_Inc_base& inco_chaleur() { return ch_inco_chaleur_.valeur(); }
  inline const Champ_Inc_base& vitesse() const { return ch_vitesse_.valeur(); }
  inline const Champ_Don_base& pression_tot() const { return ch_pression_tot_; }
  inline Champ_Don_base& pression_tot() { return ch_pression_tot_; }
  inline const Champ_Don_base& mu_sur_Schmidt() const { return ch_mu_sur_Sc; }
  inline Champ_Don_base& mu_sur_Schmidt() { return ch_mu_sur_Sc; }
  inline const Champ_Don_base& nu_sur_Schmidt() const { return ch_nu_sur_Sc; }
  inline Champ_Don_base& nu_sur_Schmidt() { return ch_nu_sur_Sc; }
  inline const Champ_Don_base& source_masse_espece() const { assert (ch_source_masse_esp_.non_nul()); return ch_source_masse_esp_; }
  inline Champ_Don_base& source_masse_espece() { assert (ch_source_masse_esp_.non_nul()); return ch_source_masse_esp_; }
  inline const Champ_Don_base& source_masse_projection() const { assert (ch_source_masse_proj_.non_nul()); return ch_source_masse_proj_; }
  inline Champ_Don_base& source_masse_projection() { assert (ch_source_masse_proj_.non_nul()); return ch_source_masse_proj_; }

  inline bool has_source_masse_espece_champ() const { return ch_source_masse_esp_.non_nul(); }
  inline bool has_source_masse_projection_champ() const { return ch_source_masse_proj_.non_nul(); }

  inline const  DoubleTab& rho_n() const { return loi_etat_->rho_n(); }
  inline const  DoubleTab& rho_np1() const { return loi_etat_->rho_np1(); }
  inline void calculer_coeff_T();
  inline void initialiser_inco_ch() { loi_etat_->initialiser_inco_ch(); }
  inline void calculer_Cp() { loi_etat_->calculer_Cp(); }
  inline void calculer_lambda() { loi_etat_->calculer_lambda(); }
  inline void calculer_mu() { loi_etat_->calculer_mu(); }
  inline void calculer_nu() override { loi_etat_->calculer_nu(); }
  inline void calculer_alpha() override { loi_etat_->calculer_alpha(); }
  inline void calculer_mu_sur_Sc() { loi_etat_-> calculer_mu_sur_Sc(); }
  inline void calculer_nu_sur_Sc() { loi_etat_-> calculer_nu_sur_Sc(); }
  inline void calculer_masse_volumique() { loi_etat_->calculer_masse_volumique(); }
  inline void set_pression_th(double Pth) { Pth_n_ = Pth_ = Pth; }
  inline int getTraitementPth() const { return traitement_PTh_; }
  inline double pression_th() const { return Pth_; } // Pression thermodynamique
  inline const double& get_pression_th() const { return Pth_; } // Reference to pression thermodynamique (for PDI, can't share a copy...)
  inline double pression_thn() const { return Pth_n_; } // Pression thermodynamique a l'etape precedente
  inline double pression_th1() const { return Pth1_; } // Pression thermodynamique calculee pour conserver la masse
  inline double calculer_H(double hh) const { return loi_etat_->calculer_H(Pth_,hh); }

  // Methodes inlines from EOS_Tools
  inline Probleme_base& get_problem() { return le_probleme_.valeur(); }
  inline const DoubleTab& rho_discvit() const { return eos_tools_->rho_discvit(); }
  inline const DoubleTab& rho_face_n() const { return eos_tools_->rho_face_n(); }
  inline const DoubleTab& rho_face_np1() const { return eos_tools_->rho_face_np1(); }
  inline void calculer_rho_face(const DoubleTab& tab_rho) { eos_tools_->calculer_rho_face_np1(tab_rho); }
  inline void divu_discvit(DoubleTab& secmem1, DoubleTab& secmem2) { eos_tools_->divu_discvit(secmem1,secmem2); }
  inline double moyenne_vol(const DoubleTab& A) const { return eos_tools_->moyenne_vol(A); }

  bool is_dilatable() const override { return true; }

protected :
  virtual void remplir_champ_pression_tot(int n, const DoubleTab& PHydro, DoubleTab& PTot) = 0;
  void completer_edo(const Probleme_base& );

  int traitement_PTh_ = 0; // flag pour le traitement de la pression thermo
  double Pth_ = -1., Pth_n_ = -1., Pth1_ = -1.;
  OBS_PTR(Champ_Inc_base) ch_inco_chaleur_, ch_vitesse_, ch_pression_;
  OBS_PTR(Probleme_base) le_probleme_;
  OWN_PTR(Champ_Don_base) ch_pression_tot_, ch_mu_sur_Sc, ch_nu_sur_Sc, ch_rho_gaz_, ch_rho_comme_v_;
  OWN_PTR(Champ_Don_base) ch_source_masse_esp_, ch_source_masse_proj_; /* si besoin */
  OWN_PTR(Loi_Etat_base) loi_etat_;
  OWN_PTR(EOS_Tools_base) eos_tools_;
  OWN_PTR(EDO_Pression_th_base) EDO_Pth_;
  Nom output_file_;
};

inline void Fluide_Dilatable_base::calculer_coeff_T()
{
  loi_etat_->remplir_T();
  calculer_Cp();
  calculer_mu();
  calculer_lambda();
  calculer_nu();
  calculer_alpha();
  calculer_mu_sur_Sc();
  calculer_nu_sur_Sc();
}

#endif /* Fluide_Dilatable_base_included */
