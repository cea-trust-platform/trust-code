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

#ifndef Dirichlet_loi_paroi_included
#define Dirichlet_loi_paroi_included

#include <Frontiere_dis_base.h>
#include <Correlation_base.h>
#include <TRUST_Ref.h>
#include <Dirichlet.h>
#include <TRUSTTab.h>
#include <Param.h>

/*! @brief Classe Dirichlet_loi_paroi Classe de base pour les valeurs impose pour une condition aux limites des equations de turbulence
 *
 *     Le flux impose est calcule a partir de la correlation de loi de paroi adaptative.
 *
 * @sa Neumann
 */
class Dirichlet_loi_paroi: public Dirichlet
{
  Declare_base(Dirichlet_loi_paroi);
public:

  virtual int initialiser(double temps) override;
  virtual void liste_faces_loi_paroi(IntTab&);
  int compatible_avec_eqn(const Equation_base&) const override;
  virtual int avancer(double temps) override {return 1;} // Avancer ne fait rien car le champ est modifie dans mettre_a_jour
  void mettre_a_jour(double tps) override;

  virtual void associer_correlation(const Correlation_base& corr) { correlation_loi_paroi_ = corr; }
  void associer_fr_dis_base(const Frontiere_dis_base& fr) override { la_frontiere_dis = fr; }
  void associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& zcl) override { mon_dom_cl_dis = zcl; }

  // fonctions de cond_lim_base qui necessitent le champ_front qu'on met a zero car on fait abstraction du champ_front
  virtual void completer() override { }
  virtual void fixer_nb_valeurs_temporelles(int nb_cases) override { }
  virtual inline Frontiere_dis_base& frontiere_dis() override { return la_frontiere_dis; }
  virtual inline const Frontiere_dis_base& frontiere_dis() const override { return la_frontiere_dis; }
  virtual void changer_temps_futur(double temps, int i) override { }
  virtual void set_temps_defaut(double temps) override { }
  virtual void calculer_coeffs_echange(double temps) override { }
  void verifie_ch_init_nb_comp() const override { }

  virtual double val_imp(int i) const override {return d_(i,0);}
  virtual double val_imp(int i, int j) const override {return d_(i,j);}
  virtual double val_imp_au_temps(double temps, int i) const override
  {
    Process::exit(que_suis_je() + " : You shouldn't go through val_imp_au_temps but through val_imp ! ");
    return 1.;
  }
  virtual double val_imp_au_temps(double temps, int i, int j) const override
  {
    Process::exit(que_suis_je() + " : You shouldn't go through val_imp_au_temps but through val_imp ! ");
    return 1.;
  }

protected:
  virtual void me_calculer()=0;

  OBS_PTR(Correlation_base) correlation_loi_paroi_;
  OBS_PTR(Frontiere_dis_base) la_frontiere_dis;
  double mon_temps = -1.e8;
  DoubleTab d_;
};

#endif
