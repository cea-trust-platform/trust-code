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

#ifndef Modele_turbulence_hyd_base_included
#define Modele_turbulence_hyd_base_included

#include <Support_Champ_Masse_Volumique.h>
#include <Turbulence_paroi.h>
#include <Domaine_Cl_dis.h>
#include <Domaine_dis.h>

class Schema_Temps_base;
class Equation_base;
class Motcle;
class Param;

static constexpr double CMU = 0.09;

/*! @brief Classe Modele_turbulence_hyd_base Cette classe sert de base a la hierarchie des classes
 *
 *     qui representent un modele de turbulence pour les
 *     equations de Navier-Stokes. Il existe deja deux classes derivees
 *     qui representent le modele de turbulence (k,e) et le modele
 *     de turbulence sous maille. Ces deux modeles ont en commun
 *     le calcul d'une viscosite turbulente.
 *
 *     B.Mat. : la classe herite de support_champ_masse_volumique.
 *      en Front-Tracking (et plus tard en qc), elle fournit le
 *      mecanisme pour recuperer rho.
 *
 */
class Modele_turbulence_hyd_base: public Objet_U, public Support_Champ_Masse_Volumique, public Champs_compris_interface
{
  Declare_base(Modele_turbulence_hyd_base);
public:
  inline const Champ_Fonc& viscosite_turbulente() const { return la_viscosite_turbulente_; }
  inline Champ_Fonc& viscosite_turbulente() { return la_viscosite_turbulente_; }
  inline Equation_base& equation();
  inline const Equation_base& equation() const;
  inline const Turbulence_paroi& loi_paroi() const { return loipar_; }
  inline Turbulence_paroi& loi_paroi() { return loipar_; }
  bool utiliser_loi_paroi() const { return loi_paroi().non_nul() ? loipar_->use_shear() : false; }
  virtual bool calcul_tenseur_Re(const DoubleTab& nu_turb, const DoubleTab& grad, DoubleTab& Re) const { return false; }
  virtual void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual int preparer_calcul();
  virtual bool initTimeStep(double dt);
  virtual void mettre_a_jour(double) =0;
  virtual void discretiser();
  void discretiser_visc_turb(const Schema_Temps_base&, Domaine_dis&, Champ_Fonc&) const;
  void discretiser_corr_visc_turb(const Schema_Temps_base&, Domaine_dis&, Champ_Fonc&) const;
  void discretiser_K(const Schema_Temps_base&, Domaine_dis&, Champ_Fonc&) const; // Utilise par les modeles de tubulence dans TrioCFD
  virtual void completer() { /* Do nothing */ }
  void associer_eqn(const Equation_base&);
  virtual void associer(const Domaine_dis&, const Domaine_Cl_dis&) { /* Do nothing */ }
  int reprendre(Entree&) override;

  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom, Option opt = NONE) const override;

  inline Champs_compris& champs_compris() { return champs_compris_; }

  virtual void imprimer(Sortie&) const;
  void a_faire(Sortie&) const;
  int sauvegarder(Sortie&) const override;

  int limpr_ustar(double, double, double, double) const;
  inline double get_Cmu() const { return LeCmu_; }
  void lire_distance_paroi();

  void limiter_viscosite_turbulente();

protected:
  double LeCmu_ = CMU;
  Champ_Fonc la_viscosite_turbulente_, wall_length_;
  REF(Equation_base) mon_equation_;
  Turbulence_paroi loipar_;
  double dt_impr_ustar_  = 1.e20, dt_impr_ustar_mean_only_  = 1.e20;
  int boundaries_ = 0;
  LIST(Nom) boundaries_list_;
  Nom nom_fichier_ = "";
  Champs_compris champs_compris_;

private:
  double XNUTM_ = 1.E8, dt_diff_sur_dt_conv_ = -1;
  int calcul_borne_locale_visco_turb_ = 0;
  Champ_Fonc corr_visco_turb_;
  DoubleVect borne_visco_turb_;
};

/*! @brief Renvoie l'equation associee au modele de turbulence.
 *
 * (c'est une equation du type Equation_base)
 *
 * @return (Equation_base&) l'equation associee au modele de turbulence
 */
inline Equation_base& Modele_turbulence_hyd_base::equation()
{
  if (mon_equation_.est_nul())
    {
      Cerr << "Error in Modele_turbulence_hyd_base::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation_.valeur();
}

inline const Equation_base& Modele_turbulence_hyd_base::equation() const
{
  if (mon_equation_.est_nul())
    {
      Cerr << "Error in Modele_turbulence_hyd_base::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation_.valeur();
}

#endif /* Modele_turbulence_hyd_base_included */
