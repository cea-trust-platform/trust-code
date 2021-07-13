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
// File:        Fluide_Dilatable_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Fluide_Dilatable_base_included
#define Fluide_Dilatable_base_included

#include <Fluide_base.h>
#include <Ref_Champ_Inc.h>
#include <Champ_Inc.h>
#include <Loi_Etat.h>
#include <EOS_Tools.h>

class Probleme_base;
class Zone_Cl_dis;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Fluide_Dilatable_base
//    Cette classe represente un d'un fluide dilatable,
//    heritant de fluide base
// .SECTION voir aussi
//     Milieu_base Fluide_base
//
//////////////////////////////////////////////////////////////////////////////

class Fluide_Dilatable_base : public Fluide_base
{
  Declare_base_sans_constructeur(Fluide_Dilatable_base);

public :
  Fluide_Dilatable_base();
  void verifier_coherence_champs(int& err,Nom& message);
  void set_Cp(double);
  void update_rho_cp(double temps);
  void creer_champs_non_lus();
  void initialiser_radiatives(const double& temps);
  int initialiser(const double& temps);

  const DoubleTab& temperature() const;
  const Champ_Don& ch_temperature() const;
  Champ_Don& ch_temperature();

  virtual void calculer_pression_tot();
  virtual void preparer_pas_temps();
  virtual void abortTimeStep();
  virtual void set_param(Param& param);
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  virtual void mettre_a_jour(double );
  virtual void preparer_calcul();
  virtual void completer(const Probleme_base&);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);

  // Virtuelles pure
  virtual void checkTraitementPth(const Zone_Cl_dis&)=0;
  virtual void prepare_pressure_edo()=0;
  virtual void write_mean_edo(double )=0;
  virtual void secmembre_divU_Z(DoubleTab& ) const=0;

  // Methodes de l interface des champs postraitables
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;

  // Methodes inlines
  inline const Nom type_fluide() const { return loi_etat_->type_fluide(); }
  inline const Loi_Etat& loi_etat() const { return loi_etat_; }
  inline Loi_Etat&  loi_etat() { return loi_etat_; }
  inline const Champ_Inc& inco_chaleur() const { return inco_chaleur_.valeur(); }
  inline Champ_Inc& inco_chaleur() { return inco_chaleur_.valeur(); }
  inline const Champ_Inc& vitesse() const { return vitesse_.valeur(); }
  inline const Champ_Don& pression_tot() const { return pression_tot_; }
  inline Champ_Don& pression_tot() { return pression_tot_; }
  inline const Champ_Don& mu_sur_Schmidt() const { return mu_sur_Sc; }
  inline Champ_Don& mu_sur_Schmidt() { return mu_sur_Sc; }
  inline const Champ_Don& nu_sur_Schmidt() const { return nu_sur_Sc; }
  inline Champ_Don& nu_sur_Schmidt() { return nu_sur_Sc; }
  inline const  DoubleTab& rho_n() const { return loi_etat_->rho_n(); }
  inline const  DoubleTab& rho_np1() const { return loi_etat_->rho_np1(); }
  inline void calculer_coeff_T();
  inline void initialiser_inco_ch() { loi_etat_->initialiser_inco_ch(); }
  inline void calculer_Cp() { loi_etat_->calculer_Cp(); }
  inline void calculer_lambda() { loi_etat_->calculer_lambda(); }
  inline void calculer_mu() { loi_etat_->calculer_mu(); }
  inline void calculer_nu() { loi_etat_->calculer_nu(); }
  inline void calculer_alpha() { loi_etat_->calculer_alpha(); }
  inline void calculer_mu_sur_Sc() { loi_etat_-> calculer_mu_sur_Sc(); }
  inline void calculer_nu_sur_Sc() { loi_etat_-> calculer_nu_sur_Sc(); }
  inline void calculer_masse_volumique() { loi_etat_->calculer_masse_volumique(); }
  inline void set_pression_th(double Pth) { Pth_n = Pth_ = Pth; }
  inline int getTraitementPth() const { return traitement_PTh; }
  inline double pression_th() const { return Pth_; } // Pression thermodynamique
  inline double pression_thn() const { return Pth_n; } // Pression thermodynamique a l'etape precedente
  inline double pression_th1() const { return Pth1; } // Pression thermodynamique calculee pour conserver la masse
  inline double calculer_H(double hh) const { return loi_etat_->calculer_H(Pth_,hh); }

  // Methodes inlines from EOS_Tools
  inline const DoubleTab& rho_discvit() const { return eos_tools_->rho_discvit(); }
  inline const DoubleTab& rho_face_n() const { return eos_tools_->rho_face_n(); }
  inline const DoubleTab& rho_face_np1() const { return eos_tools_->rho_face_np1(); }
  inline void calculer_rho_face(const DoubleTab& tab_rho) { eos_tools_->calculer_rho_face_np1(tab_rho); }
  inline void divu_discvit(DoubleTab& secmem1, DoubleTab& secmem2) { eos_tools_->divu_discvit(secmem1,secmem2); }
  inline double moyenne_vol(const DoubleTab& A) const { return eos_tools_->moyenne_vol(A); }

protected :
  int traitement_PTh; // flag pour le traitement de la pression thermo
  double Pth_, Pth_n, Pth1;
  REF(Champ_Inc) inco_chaleur_, vitesse_, pression_;
  Champ_Don pression_tot_,mu_sur_Sc,nu_sur_Sc,rho_gaz,rho_comme_v;
  Loi_Etat loi_etat_;
  EOS_Tools eos_tools_;
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
