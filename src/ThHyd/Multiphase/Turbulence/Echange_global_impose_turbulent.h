/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Echange_global_impose_turbulent_included
#define Echange_global_impose_turbulent_included

#include <Echange_global_impose.h>
#include <Correlation_base.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>

/*! @brief Classe Echange_global_impose_turbulent
 *
 *  Classe de base pour les CL turbulentes de type Echange_global_impose
 *  Utilisee seulement pour la turbulence de Pb_Multiphase
 *
 */
class Echange_global_impose_turbulent : public Echange_global_impose
{
  Declare_base(Echange_global_impose_turbulent);
public:
  int compatible_avec_eqn(const Equation_base&) const override;
  virtual int initialiser(double temps) override;
  virtual int avancer(double temps) override { return 1; } // Avancer ne fait rien car le champ est modifie dans mettre_a_jour
  void mettre_a_jour(double tps) override;
  virtual void liste_faces_loi_paroi(IntTab&) override;

  void associer_fr_dis_base(const Frontiere_dis_base& fr) override { la_frontiere_dis = fr; }
  void associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& zcl) override { mon_dom_cl_dis = zcl; }

  // fonctions de cond_lim_base qui necessitent le champ_front qu'on met a zero car on fait abstraction du champ_front
  void fixer_nb_valeurs_temporelles(int nb_cases) override { }
  inline Frontiere_dis_base& frontiere_dis() override { return la_frontiere_dis; }
  inline const Frontiere_dis_base& frontiere_dis() const override { return la_frontiere_dis; }
  void changer_temps_futur(double temps, int i) override { }
  void set_temps_defaut(double temps) override { }
  void calculer_coeffs_echange(double temps) override { }
  void verifie_ch_init_nb_comp() const override { }

  Champ_front& T_ext() override
  {
    Process::exit(que_suis_je() + " : You shouldn't go through T_ext ! ");
    return Echange_impose_base::T_ext();
  }

  const Champ_front& T_ext() const override
  {
    Process::exit(que_suis_je() + " : You shouldn't go through T_ext ! ");
    return Echange_impose_base::T_ext();
  }

  inline virtual Champ_front& h_imp() override
  {
    Process::exit(que_suis_je() + " : You shouldn't go through h_imp ! ");
    return Echange_impose_base::h_imp();
  }

  inline virtual const Champ_front& h_imp() const override
  {
    Process::exit(que_suis_je() + " : You shouldn't go through h_imp ! ");
    return Echange_impose_base::h_imp();
  }

  double h_imp(int i) const override { return h_(i, 0); }
  double h_imp(int i, int j) const override { return h_(i, j); }
  double h_imp_grad(int i) const override { return h_grad_(i, 0); }
  double h_imp_grad(int i, int j) const override { return h_grad_(i, j); }
  double T_ext(int i) const override { return T_(i, 0); }
  double T_ext(int i, int k) const override { return T_(i, k); }

protected :
  virtual void me_calculer()=0;

  REF(Correlation) correlation_loi_paroi_;
  REF(Frontiere_dis_base) la_frontiere_dis;

  double mon_temps = -1.e8;

  DoubleTab h_, h_grad_, T_;
};

#endif /* Echange_global_impose_turbulent_included */
