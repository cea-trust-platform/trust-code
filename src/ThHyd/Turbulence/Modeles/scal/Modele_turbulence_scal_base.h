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

#ifndef Modele_turbulence_scal_base_included
#define Modele_turbulence_scal_base_included

#include <Turbulence_paroi_scal_base.h>
#include <TRUST_Deriv.h>
#include <TRUST_Ref.h>

class Convection_Diffusion_std;
class Schema_Temps_base;
class Equation_base;
class Champ_base;
class Motcle;
class Param;

/*! @brief Classe Modele_turbulence_scal_base Cette classe represente un modele de turbulence pour une equation de
 *
 *     convection-diffusion d'un scalaire couplee a Navier_Stokes. On utilise
 *     deux classes derivees de cette classe de base qui representent le
 *     modele de turbulence (k,eps) et le modele de turbulence sous maille.
 *     Ces deux modeles ont en commun le calcul d'une diffusivite turbulente.
 *
 * @sa Modele_turbulence_scal_Prandtl Modele_turb_scal_Prandtl_sous_maille, Classe abstraite, Methode abstraite, void mettre_a_jour(double ), Entree& lire(const Motcle&, Entree&)
 */
class Modele_turbulence_scal_base: public Champs_compris_interface, public Objet_U
{
  Declare_base(Modele_turbulence_scal_base);
public:
  virtual int preparer_calcul();
  virtual bool initTimeStep(double dt);
  virtual void mettre_a_jour(double) =0;
  inline const Champ_Fonc_base& conductivite_turbulente() const { return conductivite_turbulente_; }
  inline const Champ_Fonc_base& diffusivite_turbulente() const { return diffusivite_turbulente_; }
  inline const Turbulence_paroi_scal_base& loi_paroi() const;
  inline int loi_paroi_non_nulle() const;
  inline Turbulence_paroi_scal_base& loi_paroi();
  virtual void discretiser();
  //void discretiser_diff_turb(const Schema_Temps_base&, Domaine_dis_base&, Champ_Fonc_base&) const;
  void associer_eqn(const Equation_base&);
  virtual void completer();
  virtual void associer(const Domaine_dis_base&, const Domaine_Cl_dis_base&);
  void a_faire(Sortie&) const;
  int sauvegarder(Sortie&) const override;
  int reprendre(Entree&) override;
  inline Convection_Diffusion_std& equation();
  inline const Convection_Diffusion_std& equation() const;

  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom, Option opt = NONE) const override;
  bool has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const override;
  bool has_champ(const Motcle& nom) const override;

  int limpr_nusselt(double, double, double, double) const;
  virtual void imprimer(Sortie&) const;

  virtual void set_param(Param&);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

protected:
  OWN_PTR(Champ_Fonc_base)  conductivite_turbulente_, diffusivite_turbulente_;
  OBS_PTR(Convection_Diffusion_std) mon_equation_;
  OWN_PTR(Turbulence_paroi_scal_base) loipar_;
  double dt_impr_nusselt_ = DMAXFLOAT, dt_impr_nusselt_mean_only_ = DMAXFLOAT;
  int boundaries_ = 0;
  LIST(Nom) boundaries_list_;
  Nom nom_fichier_ = "";
  Champs_compris champs_compris_;
};

/*! @brief Renvoie la loi de turbulence sur la paroi (version const)
 *
 * @return (Turbulence_paroi_scal_base&) la loi de turbulence sur la paroi
 */
inline const Turbulence_paroi_scal_base& Modele_turbulence_scal_base::loi_paroi() const
{
  return loipar_.valeur();
}

/*! @brief Renvoie si oui ou non loi de paroi (version const)
 *
 * @return (int)
 */
inline int Modele_turbulence_scal_base::loi_paroi_non_nulle() const
{
  return loipar_.non_nul();
}

/*! @brief Renvoie la loi de turbulence sur la paroi
 *
 * @return (Turbulence_paroi_scal_base&) la loi de turbulence sur la paroi
 */
inline Turbulence_paroi_scal_base& Modele_turbulence_scal_base::loi_paroi()
{
  return loipar_.valeur();
}

inline Convection_Diffusion_std& Modele_turbulence_scal_base::equation()
{
  if (mon_equation_.non_nul() == 0)
    {
      Cerr << "\nError in Modele_turbulence_scal_base::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation_.valeur();
}

inline const Convection_Diffusion_std& Modele_turbulence_scal_base::equation() const
{
  if (mon_equation_.non_nul() == 0)
    {
      Cerr << "\nError in Modele_turbulence_scal_base::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation_.valeur();
}

#endif
