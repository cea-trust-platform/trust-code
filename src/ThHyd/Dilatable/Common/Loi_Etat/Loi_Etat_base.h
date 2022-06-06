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

#ifndef Loi_Etat_base_included
#define Loi_Etat_base_included

#include <Ref_Fluide_Dilatable_base.h>
#include <Champs_compris_interface.h>
#include <Ref_Probleme_base.h>
#include <Champs_compris.h>
#include <Champ_Don.h>

class Fluide_Dilatable_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Loi_Etat_base
//     Cette classe est la base de la hierarchie des lois d'etat.
//     Elle definit un fluide dilatable
// .SECTION voir aussi
//     Fluide_Dilatable_base
//     Classe abstraite dont toutes les lois d'etat doivent deriver.
//     Methodes abstraites:
//       void calculer_coeff_T()
//       void calculer_masse_volumique()
//
//////////////////////////////////////////////////////////////////////////////

class Loi_Etat_base : public Objet_U, public Champs_compris_interface
{
  Declare_base_sans_constructeur(Loi_Etat_base);

public :
  Loi_Etat_base();
  void assoscier_probleme(const Probleme_base& pb);
  void mettre_a_jour(double);
  void calculer_nu();
  Champ_Don& ch_temperature();
  const Champ_Don& ch_temperature() const;

  // Methodes virtuelles
  virtual void associer_fluide(const Fluide_Dilatable_base&);
  virtual void preparer_calcul();
  virtual void abortTimeStep();
  virtual void initialiser_inco_ch();
  virtual void calculer_mu();
  virtual void calculer_lambda();
  virtual void calculer_alpha();
  virtual void calculer_mu_sur_Sc();
  virtual void calculer_nu_sur_Sc();
  virtual void calculer_masse_volumique();
  virtual double calculer_H(double,double) const;
  virtual double Drho_DP(double,double) const ;
  virtual double Drho_DT(double,double) const ;
  virtual double De_DP(double,double) const ;
  virtual double De_DT(double,double) const ;
  //Methodes de l interface des champs postraitables
  const Champ_base& get_champ(const Motcle& nom) const override;
  void creer_champ(const Motcle& motlu) override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;

  // Methodes virtuelles pure
  const virtual Nom type_fluide() const =0;
  virtual void initialiser() =0;
  virtual void remplir_T() =0;
  virtual void calculer_Cp() =0;
  virtual double calculer_masse_volumique(double,double) const =0;
  virtual double inverser_Pth(double,double) =0;

  // Methodes inlines
  inline Champs_compris& champs_compris() { return champs_compris_; }
  inline const DoubleTab& temperature() const { return ch_temperature().valeurs(); }
  inline const DoubleTab& rho_n() const { return tab_rho_n; }
  inline const DoubleTab& rho_np1() const { return tab_rho_np1; }
  inline double Prandt() const { return Pr_; }

protected :
  REF(Fluide_Dilatable_base) le_fluide;
  REF(Probleme_base) le_prob_;
  Champ_Don temperature_;
  DoubleTab tab_rho_n, tab_rho_np1;    //rho a l'etape precedente et l'etape suivante
  double Pr_;
  int debug;

private :
  Champs_compris champs_compris_;
};

#endif /* Loi_Etat_base_included */
