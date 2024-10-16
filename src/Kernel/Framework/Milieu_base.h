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

#ifndef Milieu_base_included
#define Milieu_base_included

#include <Champs_compris_interface.h>
#include <Porosites_champ.h>
#include <Interface_blocs.h>
#include <Champ_Inc_base.h>
#include <Champs_compris.h>

#include <TRUST_Ref.h>



class Discretisation_base;
class Domaine_dis_base;
class Champ_Don_base;
class Probleme_base;
class Motcle;
class Param;

/*! @brief classe Milieu_base Cette classe est la base de la hierarchie des milieux (physiques)
 *
 *      Elle regroupe les fonctionnalites (communes) et les proprietes physiques
 *      d'un milieu. Cette classe contient les principaux champs donnees
 *      caracterisant le milieu:
 *         - masse volumique       (rho)
 *         - diffusivite           (alpha)
 *         - conductivite          (lambda)
 *         - capacite calorifique  (Cp)
 *         - dilatabilite          (beta_th)
 *
 * @sa Milieu Solide Fluide_Incompressible Constituant, Classe abstraite dont tous les milieux physiques doivent deriver., Methodes abstraites:, void tester_champs_lus(), void mettre_a_jour(double temps), void initialiser()
 */
class Milieu_base : public Champs_compris_interface, public Objet_U
{
  Declare_base_sans_constructeur(Milieu_base);
public:
  Milieu_base();
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int associer_(Objet_U&) override;
  void nommer(const Nom&) override;
  const Nom& le_nom() const override;

  inline DoubleVect& porosite_elem() { return static_cast<DoubleVect&>(ch_porosites_->valeurs()); }
  inline const DoubleVect& porosite_elem() const { return static_cast<const DoubleVect&>(ch_porosites_->valeurs()); }
  inline double porosite_elem(const int i) const { return ch_porosites_->valeurs()(i,0); }
  bool has_porosites() const { return ch_porosites_.non_nul(); }
  inline DoubleVect& porosite_face() { return porosite_face_; }
  inline const DoubleVect& porosite_face() const { return porosite_face_; }
  inline const Champ_Don_base& get_porosites_champ() const { return ch_porosites_; }
  inline double porosite_face(const int i) const { return porosite_face_[i]; }
  inline const DoubleVect& section_passage_face() const { return section_passage_face_; }
  inline double section_passage_face(int i) const { return section_passage_face_[i]; }

  // TODO : FIXME : DoubleVect peut etre ??
  inline DoubleTab& diametre_hydraulique_elem() { return ch_diametre_hyd_->valeurs(); }
  inline const DoubleTab& diametre_hydraulique_elem() const { return ch_diametre_hyd_->valeurs(); }
  inline DoubleVect& diametre_hydraulique_face() { return diametre_hydraulique_face_; }
  inline const DoubleVect& diametre_hydraulique_face() const { return diametre_hydraulique_face_; }
  inline double diametre_hydraulique_face(int i) const { return diametre_hydraulique_face_[i]; }

  virtual int est_deja_associe();
  virtual void set_param(Param& param);
  virtual void preparer_calcul();
  virtual void verifier_coherence_champs(int& err, Nom& message);
  virtual void creer_champs_non_lus();
  virtual void discretiser(const Probleme_base& pb, const Discretisation_base& dis);
  virtual int is_rayo_semi_transp() const { return 0; }
  virtual int is_rayo_transp() const { return 0; }
  virtual void mettre_a_jour(double temps);
  virtual bool initTimeStep(double dt) { return true; }
  virtual void abortTimeStep();
  virtual void resetTime(double time);
  virtual int initialiser(const double temps);
  virtual void associer_gravite(const Champ_Don_base&);
  virtual const Champ_base& masse_volumique() const;
  virtual Champ_base& masse_volumique();
  bool has_masse_volumique() const { return ch_rho_.non_nul(); }
  virtual const Champ_Don_base& diffusivite() const;
  virtual Champ_Don_base& diffusivite();
  bool has_diffusivite() const { return ch_alpha_.non_nul(); }
  virtual const Champ_Don_base& diffusivite_fois_rho() const;
  virtual Champ_Don_base& diffusivite_fois_rho();
  virtual const Champ_Don_base& conductivite() const;
  virtual Champ_Don_base& conductivite();
  bool has_conductivite() const { return ch_lambda_.non_nul(); }
  virtual const Champ_Don_base& capacite_calorifique() const;
  virtual Champ_Don_base& capacite_calorifique();
  bool has_capacite_calorifique() const { return ch_Cp_.non_nul(); }
  virtual const Champ_Don_base& beta_t() const;
  virtual Champ_Don_base& beta_t();
  bool has_beta_t() const { return ch_beta_th_.non_nul(); }
  virtual const Champ_Don_base& gravite() const;
  virtual Champ_Don_base& gravite();
  virtual int a_gravite() const;
  virtual void update_rho_cp(double temps);

  // equations associees au milieu
  virtual void associer_equation(const Equation_base* eqn) const;
  virtual const Equation_base& equation(const std::string& nom_inc) const;

  // controle du domaine de validite des inconnues des equations associees au milieu par defaut renvoie 1 (OK)
  virtual int check_unknown_range() const { return 1; }

  //Methodes de l interface des champs postraitables
  void creer_champ(const Motcle& motlu) override { }
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  bool has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const override;
  bool has_champ(const Motcle& nom) const override;

  const bool& has_hydr_diam() { return has_hydr_diam_; }
  void set_id_composite(const int i);
  int id_composite_ = -1;

  // Liste des champs des milieux:
  const LIST(OBS_PTR(Champ_Don_base))& champs_don() const { return champs_don_; }

protected:
  OBS_PTR(Domaine_dis_base) zdb_;
  OWN_PTR(Champ_base) ch_rho_; //peut etre un OWN_PTR(Champ_Don_base) ou un Champ_Inc
  OWN_PTR(Champ_Don_base) ch_g_, ch_alpha_, ch_lambda_, ch_alpha_fois_rho_, ch_Cp_, ch_beta_th_, ch_porosites_, ch_diametre_hyd_;
  OWN_PTR(Champ_Fonc_base)  ch_rho_Cp_elem_, ch_rho_Cp_comme_T_;
  Champs_compris champs_compris_;
  DoubleVect porosite_face_, section_passage_face_ /* pour F5 */, diametre_hydraulique_face_;
  Nom nom_;
  LIST(OBS_PTR(Champ_Don_base)) champs_don_;

  enum Type_rayo { NONRAYO, TRANSP, SEMITRANSP };
  Type_rayo indic_rayo_;

  mutable std::map<std::string, const Equation_base *> equation_;
  virtual void calculer_alpha();
  void ecrire(Sortie& ) const;
  void creer_alpha();
  //void creer_derivee_rho();

  // Utile pour F5
  void discretiser_porosite(const Probleme_base& pb, const Discretisation_base& dis);
  void discretiser_diametre_hydro(const Probleme_base& pb, const Discretisation_base& dis);
  virtual void set_additional_params(Param& param);
  virtual void calculate_face_porosity();
  virtual void calculate_face_hydr_diam();
  void mettre_a_jour_porosite(double temps);
  void fill_section_passage_face();
  int initialiser_porosite(const double temps);
  void check_gravity_vector() const;

private:
  // attribue utile pour porosites (pas porosites_champ) ... a voir si utile sinon a virer ...
  Porosites porosites_;
  bool is_user_porosites_ = false, is_field_porosites_ = false, has_hydr_diam_ = false;
  void verifie_champ_porosites();
  void nettoie_champ_porosites();
  const bool& is_user_porosites() { return is_user_porosites_; }
  const bool& is_field_porosites() { return is_field_porosites_; }

  mutable int deja_associe_;
  bool via_associer_ = false;
  void warn_old_syntax();
  OBS_PTR(Champ_Don_base) g_via_associer_;
};

#endif /* Milieu_base_included */
