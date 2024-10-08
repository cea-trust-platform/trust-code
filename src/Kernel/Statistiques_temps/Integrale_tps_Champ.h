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

#ifndef Integrale_tps_Champ_included
#define Integrale_tps_Champ_included

#include <Champ_Generique_base.h>
#include <TRUSTTabs_forward.h>
#include <Champ_Fonc_base.h>
#include <TRUST_Deriv.h>
#include <TRUST_Ref.h>

class Domaine_dis_base;
class Champ_base;

class Integrale_tps_Champ : public Objet_U
{
  Declare_instanciable(Integrale_tps_Champ);
public:
  inline const Champ_Fonc_base& le_champ_calcule() const { return le_champ_.valeur(); }
  inline Champ_Fonc_base& le_champ_calcule() { return le_champ_.valeur(); }
  inline const OBS_PTR(Champ_Generique_base)& le_champ() const { return le_champ_ref_; }

  inline double temps_integrale() const { return tps_integrale_; } // le temps courant de l'integrale
  inline double t_debut() const { return t_debut_; } // le temps de debut d'integration
  inline double t_fin() const { return t_fin_; } //  le temps de fin d'integration
  inline double dt_integration() const { return dt_integr_calcul_; } // la duree d'integration deja effectuee
  inline void fixer_t_debut(double t) { t_debut_ = t; } // Fixe le temps de debut d'integration
  inline void fixer_t_fin(double t) { t_fin_ = t; } // Fixe le temps de fin d'integration
  inline void fixer_dt_integr(double t) { dt_integr_calcul_ = t; }
  inline void fixer_tps_integrale(double t) { tps_integrale_ = t; } // Fixe le temps courant de l'integrale (derniere date a laquelle on a mis l'integrale a jour)
  inline void associer(const Champ_base&, int, double, double) { }
  inline void associer(const Champ_Generique_base&, int, double, double);
  virtual void mettre_a_jour_integrale();

  virtual inline void mettre_a_jour(double)
  {
    mettre_a_jour_integrale();
    double le_temps = le_champ_ref_->get_time();
    le_champ_->changer_temps(le_temps);
  }

  void typer_champ(const Nom&);

protected:
  OWN_PTR(Champ_Fonc_base) le_champ_;
  OBS_PTR(Champ_Generique_base) le_champ_ref_;
  int puissance_ = -10;
  double t_debut_ = -100., t_fin_ = -100.;
  double tps_integrale_ = -100., dt_integr_calcul_ = -100.;
};

inline void Integrale_tps_Champ::associer(const Champ_Generique_base& le_ch, int n, double t0, double t1)
{
  le_champ_ref_ = le_ch;
  puissance_ = n;
  t_debut_ = t0;
  t_fin_ = t1;
  tps_integrale_ = t_debut_;
  dt_integr_calcul_ = 0;
}

#endif /* Integrale_tps_Champ_included */
