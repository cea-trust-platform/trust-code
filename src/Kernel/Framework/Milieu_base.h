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

#ifndef Milieu_base_included
#define Milieu_base_included

#include <Champs_compris_interface.h>
#include <Ref_Champ_Don_base.h>
#include <Ref_Zone_dis_base.h>
#include <Porosites_champ.h>
#include <Interface_blocs.h>
#include <Champ_Inc_base.h>
#include <Champs_compris.h>
#include <Champ_Fonc.h>
#include <Champ_Don.h>
#include <Champ.h>

class Discretisation_base;
class Probleme_base;
class Champ_Don;
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

  inline DoubleVect& porosite_elem() { return static_cast<DoubleVect&>(porosites_champ->valeurs()); }
  inline const DoubleVect& porosite_elem() const { return static_cast<const DoubleVect&>(porosites_champ->valeurs()); }
  inline double porosite_elem(const int i) const { return porosites_champ->valeurs()(i,0); }
  inline DoubleVect& porosite_face() { return porosite_face_; }
  inline const DoubleVect& porosite_face() const { return porosite_face_; }
  inline double porosite_face(const int i) const { return porosite_face_[i]; }
  inline const DoubleVect& section_passage_face() const { return section_passage_face_; }
  inline double section_passage_face(int i) const { return section_passage_face_[i]; }

  virtual int est_deja_associe();
  virtual void set_param(Param& param);
  virtual void preparer_calcul();
  virtual void verifier_coherence_champs(int& err, Nom& message);
  virtual void creer_champs_non_lus();
  virtual void discretiser(const Probleme_base& pb, const Discretisation_base& dis);
  virtual int is_rayo_semi_transp() const;
  virtual int is_rayo_transp() const;
  virtual void mettre_a_jour(double temps);
  virtual bool initTimeStep(double dt);
  virtual void abortTimeStep();
  virtual int initialiser(const double temps);
  virtual void associer_gravite(const Champ_Don_base&);
  virtual const Champ& masse_volumique() const;
  virtual Champ& masse_volumique();
  virtual const Champ_Don& diffusivite() const;
  virtual Champ_Don& diffusivite();
  virtual const Champ_Don& conductivite() const;
  virtual Champ_Don& conductivite();
  virtual const Champ_Don& capacite_calorifique() const;
  virtual Champ_Don& capacite_calorifique();
  virtual const Champ_Don& beta_t() const;
  virtual Champ_Don& beta_t();
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
  //////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  /////////////////////////////////////////////////////

  void set_id_composite(const int i);
  int id_composite = -1;

protected:
  REF(Zone_dis_base) zdb_;
  Champ rho; //peut etre un Champ_Don ou un Champ_Inc
  Champ_Don g, alpha, lambda, Cp, beta_th, porosites_champ;
  Champ_Fonc rho_cp_elem_,rho_cp_comme_T_;
  Champs_compris champs_compris_;
  DoubleVect porosite_face_, section_passage_face_ /* pour F5 */;
  Nom nom_;

  enum Type_rayo { NONRAYO, TRANSP, SEMITRANSP };
  Type_rayo indic_rayo_;

  mutable std::map<std::string, const Equation_base *> equation_;
  virtual void calculer_alpha();
  void ecrire(Sortie& ) const;
  void creer_alpha();
  void creer_derivee_rho();

  // Utile pour F5
  void discretiser_porosite(const Probleme_base& pb, const Discretisation_base& dis);
  void set_param_porosite(Param& param);
  void mettre_a_jour_porosite(double temps);
  void fill_section_passage_face();
  int initialiser_porosite(const double temps);


private:
  void update_porosity_values();
  // attribue utile pour porosites (pas porosites_champ) ... a voir si utile sinon a virer ...
  Porosites porosites_;
  bool is_user_porosites_ = false, is_field_porosites_ = false;
  const bool& is_user_porosites() { return is_user_porosites_; }
  const bool& is_field_porosites() { return is_field_porosites_; }

  // ***************************************************************************
  // TODO : XXX:  TEMPORAIRE : on accepte pour le momemnt l'ancienne syntaxe,
  // i.e. typer_lire xxxx milieu, associer, typer_lire_grav, associer xxxx , ...
  // a voir plus tard quand ca devient absolete

  mutable int deja_associe;
  bool via_associer_ = false;
  inline bool g_via_associer() { return via_associer_; }
  void warn_old_syntax();
  REF(Champ_Don_base) g_via_associer_;
  // FIN partie TEMPORAIRE
  // ***************************************************************************
};

#endif /* Milieu_base_included */
