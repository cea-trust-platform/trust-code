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

#ifndef Probleme_base_included
#define Probleme_base_included

#include <Probleme_base_interface_proto.h>
#include <Champs_compris_interface.h>
#include <Champ_front_Parametrique.h>
#include <Champ_Parametrique.h>
#include <Correlation_base.h>
#include <Probleme_Couple.h>
#include <Postraitements.h>
#include <Equation_base.h>
#include <Save_Restart.h>
#include <Milieu_base.h>
#include <TRUST_List.h>
#include <Probleme_U.h>

class Loi_Fermeture_base;
class Schema_Temps_base;
class EcrFicPartageBin;
class Postraitement;
class Field_base;

/*! @brief classe Probleme_base C'est un Probleme_U qui n'est pas un couplage.
 *
 *      Le role d'un Probleme_base est la resolution sur un domaine des
 *      equations qui le compose.
 *      Ses membres sont les attributs et les methodes communs
 *      a toutes les classes qui representent des problemes.
 *      Un certains nombre d'objets sont associes au probleme pour
 *      le constituer:
 *        - 1 ou plusieurs equations: objet Equation
 *        - 1 Domaine discretise: objet Domaine_Dis
 *        - un postraitement: objet Postraitement
 *        - Une discretisation: objet Discretistaion
 *        - un schema en temps: objet Schema_temp
 *
 * @sa Probleme, Classe abstraite dont tous les problemes doivent deriver., Methodes abstraites:, int nombre_d_equations() const, const Equation_base& equation(int) const, Equation_base& equation(int)
 */
class Probleme_base : public Champs_compris_interface, public Probleme_U, public Probleme_base_interface_proto
{
  Declare_base_sans_destructeur(Probleme_base);
public:
  ~Probleme_base() { }
  virtual void associer();
  virtual Entree& lire_equations(Entree& is, Motcle& dernier_mot);
  virtual void completer();
  virtual int verifier();
  virtual int nombre_d_equations() const =0;
  virtual const Equation_base& equation(int) const =0;
  virtual Equation_base& equation(int) =0;

  // B.Mathieu: j'aurais voulu rendre ces deux methodes virtuelles, mais
  //  alors il faut les surcharger dans tous les problemes (function ... hidden by ...)
  // Rustine: je cree une methode virtuelle avec un autre nom.
  // WEC : pour supprimer equation(Nom), il faudrait toucher a environ 40 classes...
  const Equation_base& equation(const Nom&) const;
  Equation_base& equation(const Nom&);
  int sauvegarder(Sortie& ) const override;
  int reprendre(Entree& ) override;
  inline int is_sauvegarde_simple() const { return save_restart_.is_sauvegarde_simple(); }
  inline const Nom& restart_filename() const { return save_restart_.restart_filename(); }
  inline const Nom& checkpoint_filename() const { return save_restart_.checkpoint_filename(); }
  inline const Nom& checkpoint_format() const { return save_restart_.checkpoint_format(); }
  inline const Nom& yaml_filename() const { return save_restart_.yaml_filename(); }

  virtual const Equation_base& get_equation_by_name(const Nom&) const;
  virtual Equation_base& getset_equation_by_name(const Nom&);
  virtual const Milieu_base& milieu() const;
  virtual Milieu_base& milieu();
  virtual double calculer_pas_de_temps() const;
  virtual void mettre_a_jour(double temps) ;
  virtual void preparer_calcul() ;
  virtual void imprimer(Sortie& os) const; // Appelle imprimer sur chaque equation

  // Methodes d'acces aux membres prives.
  int associer_(Objet_U&) override;
  virtual void associer_sch_tps_base(const Schema_Temps_base&);
  virtual void associer_domaine(const Domaine&);
  virtual void associer_milieu_base(const Milieu_base&);
  virtual void discretiser(Discretisation_base&);
  virtual void discretiser_equations();
  virtual void finir();

  inline std::vector<OWN_PTR(Milieu_base)>& milieu_vect() { return le_milieu_; }
  inline const std::vector<OWN_PTR(Milieu_base)>& milieu_vect() const { return le_milieu_; }

  const Schema_Temps_base& schema_temps() const;
  Schema_Temps_base& schema_temps();
  const Domaine& domaine() const;
  Domaine& domaine();
  const Domaine_dis_base& domaine_dis() const;
  Domaine_dis_base& domaine_dis();
  bool is_dilatable() const;

  inline const Discretisation_base& discretisation() const;
  inline Postraitements& postraitements() { return les_postraitements_; }
  inline const Postraitements& postraitements() const { return les_postraitements_; }
  void init_postraitements();
  virtual int expression_predefini(const Motcle& motlu, Nom& expression);

  inline const char* reprise_format_temps() const { return save_restart_.reprise_format_temps(); }
  inline bool& reprise_effectuee() { return save_restart_.reprise_effectuee(); }
  inline bool reprise_effectuee() const { return save_restart_.reprise_effectuee(); }

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  bool has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const override;
  bool has_champ(const Motcle& nom) const override;
  /////////////////////////////////////////////////////

  //Pour acceder a un champ de la liste portee pas le postraitement.
  virtual const Champ_Generique_base& get_champ_post(const Motcle& nom) const;
  virtual bool has_champ_post(const Motcle& nom) const;
  virtual int comprend_champ_post(const Motcle& nom) const;

  // Fonctions de recherche de IntTab pour le postraitement
  virtual int a_pour_IntVect(const Motcle&, OBS_PTR(IntVect)& ) const;
  virtual void lire_postraitement_interfaces(Entree& is);
  virtual void postraiter_interfaces(const Nom& nom_fich, Sortie& s, const Nom& format, double temps);

  virtual void addInputField(Field_base& f) { addInputField_impl(*this, f); }

  int is_coupled() const { return (int)pbc_.non_nul(); }

  int postraiter(int force = 1) override;
  int limpr() const override;
  int lsauv() const override;
  void sauver() const override;
  virtual void allocation() const final;

  //////////////////////////////////////////////////
  //                                              //
  // Implementation de l'interface de Probleme_U  //
  //                                              //
  //////////////////////////////////////////////////

  // interface Problem
  void initialize() override { initialize_impl(*this); }
  const bool& is_probleme_base_initialized() const { return is_probleme_initialized_impl(); }
  void terminate() override { terminate_impl(*this); }

  // interface UnsteadyProblem
  double presentTime() const override { return presentTime_impl(*this); }
  double computeTimeStep(bool& stop) const override { return computeTimeStep_impl(*this, stop); }
  bool initTimeStep(double dt) override { return initTimeStep_impl(*this, dt); }
  bool solveTimeStep() override { return solveTimeStep_impl(*this); }
  bool solveTimeStep_pbU() { return Probleme_U::solveTimeStep(); }
  bool isStationary() const override { return isStationary_impl(*this); }
  std::string newCompute() override;
  void validateTimeStep() override { validateTimeStep_impl(*this); }
  void setStationary(bool flag) override { schema_temps().set_stationnaires_atteints(flag); }
  void abortTimeStep() override { abortTimeStep_impl(*this); }
  void resetTime(double time) override;

  // interface IterativeUnsteadyProblem
  bool iterateTimeStep(bool& converged) override { return iterateTimeStep_impl(*this, converged); }

  // interface FieldIO
  void getInputFieldsNames(Noms& noms) const override { getInputFieldsNames_impl(*this, noms); }
  void getOutputFieldsNames(Noms& noms) const override { getOutputFieldsNames_impl(*this, noms); }

  // interface Probleme_U
  bool updateGivenFields() override { return updateGivenFields_impl(*this); }
  double futureTime() const override { return futureTime_impl(*this); }

  OBS_PTR(Field_base) findInputField(const Nom& name) const override { return findInputField_impl(*this, name); }
  OBS_PTR(Champ_Generique_base) findOutputField(const Nom& name) const override { return findOutputField_impl(*this, name); }

  virtual bool is_pb_med() { return false ; }
  virtual bool is_pb_rayo() { return false ; }

  void associer_pb_couple(const Probleme_Couple& pbc) { pbc_ = pbc; }
  const Probleme_Couple& get_pb_couple() const { return pbc_; }
  Probleme_Couple& get_pb_couple() { return pbc_; }
  LIST(OBS_PTR(SFichier))& get_set_out_files() const { return out_files_ ; }

  const Correlation_base& get_correlation(std::string nom_correlation) const
  {
    Motcle mot(nom_correlation);
    return correlations_.at(mot.getString());
  }

  int has_correlation(std::string nom_correlation) const
  {
    Motcle mot(nom_correlation);
    return (int)correlations_.count(mot.getString());
  }

protected :

  void warn_old_syntax();
  virtual void typer_lire_milieu(Entree& is) ;
  virtual void lire_solved_equations(Entree& is) { /* Do nothing */ }
  Entree& read_optional_equations(Entree& is, Motcle& mot);
  virtual Entree& lire_correlations(Entree& is);

  Save_Restart save_restart_;
  bool milieu_via_associer_ = false;
  std::vector<OWN_PTR(Milieu_base)> le_milieu_;
  OBS_PTR(Domaine_dis_base) le_domaine_dis_;   // Discretized domain. Just a REF since Domaine_dis_cache is the real owner.
  Postraitements les_postraitements_;
  OBS_PTR(Domaine) le_domaine_;
  OBS_PTR(Schema_Temps_base) le_schema_en_temps_;
  OBS_PTR(Discretisation_base) la_discretisation_;
  OBS_PTR(Probleme_Couple) pbc_;
  mutable LIST(OBS_PTR(SFichier)) out_files_; // Liste des SFichier a fermer (.out)
  std::map<std::string, OWN_PTR(Correlation_base)> correlations_;

  LIST(OBS_PTR(Loi_Fermeture_base)) liste_loi_fermeture_; // liste des fermetures associees au probleme
  LIST(OBS_PTR(Champ_Parametrique)) Champs_Parametriques_; //Champs parametriques a mettre a jour lorsque le calcul courant est fini
  LIST(OWN_PTR(Equation_base)) eq_opt_; //autres equations (turbulence, aire interfaciale...)
};

/*! @brief Renvoie la discretisation associee au probleme
 *
 * @return (Discretisation_base&) discretisation associee au probleme
 * @throws la discretisation n'est pas construite
 */
inline const Discretisation_base& Probleme_base::discretisation() const
{
  if(!la_discretisation_.non_nul())
    {
      Cerr << que_suis_je() << " has not been discretized!" << finl;
      exit();
    }
  return la_discretisation_.valeur();
}

#endif /* Probleme_base_included */
