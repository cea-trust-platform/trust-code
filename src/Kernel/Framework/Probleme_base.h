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

#ifndef Probleme_base_included
#define Probleme_base_included

#include <Probleme_base_interface_proto.h>
#include <Champs_compris_interface.h>
#include <Sortie_Fichier_base.h>
#include <Probleme_Couple.h>
#include <Postraitements.h>
#include <Sortie_Brute.h>
#include <TRUST_Deriv.h>
#include <Domaine_dis.h>
#include <TRUST_List.h>
#include <Probleme_U.h>
#include <TRUST_Ref.h>
#include <Milieu.h>

class Loi_Fermeture_base;
class Schema_Temps_base;
class EcrFicPartageBin;
class Equation_base;
class Postraitement;
class Field_base;
class Champ_Fonc;

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
  ~Probleme_base();
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

  const Schema_Temps_base& schema_temps() const;
  Schema_Temps_base& schema_temps();
  const Domaine& domaine() const;
  Domaine& domaine();
  const Domaine_dis& domaine_dis() const;
  Domaine_dis& domaine_dis();
  int is_dilatable() const;

  virtual int allocate_file_size(long int& size) const;
  virtual int file_size_xyz() const;

  inline void nommer(const Nom&) override;
  inline const Nom& le_nom() const override;
  inline const Discretisation_base& discretisation() const;
  inline Postraitements& postraitements() { return les_postraitements_; }
  inline const Postraitements& postraitements() const { return les_postraitements_; }
  void init_postraitements();
  virtual int expression_predefini(const Motcle& motlu, Nom& expression);
  inline const char* reprise_format_temps() const;
  inline bool& reprise_effectuee() { return restart_done_; }
  inline bool reprise_effectuee() const { return restart_done_; }

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  virtual bool has_champ(const Motcle& nom) const;
  /////////////////////////////////////////////////////

  //Pour acceder a un champ de la liste portee pas le postraitement.
  virtual const Champ_Generique_base& get_champ_post(const Motcle& nom) const;
  virtual int comprend_champ_post(const Motcle& nom) const;

  // Fonctions de recherche de IntTab pour le postraitement
  virtual int a_pour_IntVect(const Motcle&, REF(IntVect)& ) const;
  virtual void lire_postraitement_interfaces(Entree& is);
  virtual void postraiter_interfaces(const Nom& nom_fich, Sortie& s, const Nom& format, double temps);

  virtual void addInputField(Field_base& f) { addInputField_impl(*this, f); }
  void sauver_xyz(int) const;
  int is_coupled() const { return (int)pbc_.non_nul(); }

  int postraiter(int force = 1) override;
  int limpr() const override;
  int lsauv() const override;
  void sauver() const override;
  virtual void allocation() const;

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
  void validateTimeStep() override { validateTimeStep_impl(*this); }
  void setStationary(bool flag) override { schema_temps().set_stationnaires_atteints(flag); }
  void abortTimeStep() override { abortTimeStep_impl(*this); }
  void resetTime(double time) override { resetTime_impl(*this, time); }

  // interface IterativeUnsteadyProblem
  bool iterateTimeStep(bool& converged) override { return iterateTimeStep_impl(*this, converged); }

  // interface FieldIO
  void getInputFieldsNames(Noms& noms) const override { getInputFieldsNames_impl(*this, noms); }
  void getOutputFieldsNames(Noms& noms) const override { getOutputFieldsNames_impl(*this, noms); }

  // interface Probleme_U
  bool updateGivenFields() override { return updateGivenFields_impl(*this); }
  double futureTime() const override { return futureTime_impl(*this); }

  REF(Field_base) findInputField(const Nom& name) const override { return findInputField_impl(*this, name); }
  REF(Champ_Generique_base) findOutputField(const Nom& name) const override { return findOutputField_impl(*this, name); }

  virtual bool is_pb_med() { return false ; }
  virtual bool is_pb_FT() { return false ; }
  virtual bool is_pb_rayo() { return false ; }

  void associer_pb_couple(const Probleme_Couple& pbc) { pbc_ = pbc; }
  const Probleme_Couple& get_pb_couple() const { return pbc_; }
  Probleme_Couple& get_pb_couple() { return pbc_; }

protected :
  std::vector<Milieu> le_milieu_;
  REF(Domaine_dis) le_domaine_dis_;   // Discretized domain. Just a REF since Domaine_dis_cache is the real owner.
  Postraitements les_postraitements_;
  REF(Domaine) le_domaine_;
  REF(Schema_Temps_base) le_schema_en_temps_;
  REF(Discretisation_base) la_discretisation_;
  REF(Probleme_Couple) pbc_;
  void warn_old_syntax();
  virtual void typer_lire_milieu(Entree& is) ;
  void lire_sauvegarde_reprise(Entree& is, Motcle& motlu) ;

  mutable DERIV(Sortie_Fichier_base) ficsauv_;
  mutable Sortie_Brute* osauv_hdf_ = nullptr;

  bool milieu_via_associer_ = false;

  Nom restart_file_name_;  // Name of the file for save/restart
  Nom restart_format_;     // Format for the save restart
  bool restart_done_ = false;         // Has a restart been done?
  bool simple_restart_ = false;       // Restart file name
  int restart_version_ = 155;         // Version number, for example 155 (1.5.5) -> used to manage old restart files
  bool restart_in_progress_ = false;  //true variable only during the time step during which a resumption of computation is carried out

  static long int File_size_;        // Espace disque pris par les sauvegarde XYZ
  static int Bad_allocate_;        // 1 si allocation reussi, 0 sinon
  static int Nb_pb_total_;        // Nombre total de probleme
  static int Num_pb_;                // numero du probleme
  mutable Nom error_;                // Erreur d'allocation

  LIST(REF(Loi_Fermeture_base)) liste_loi_fermeture_; // liste des fermetures associees au probleme
};

/*! @brief surcharge Objet_U::nommer(const Nom&) Donne un nom au probleme
 *
 * @param (Nom& name) le nom a donner au probleme
 */
inline void Probleme_base::nommer(const Nom& name) { nom=name; }

/*! @brief surcharge Objet_U::le_nom() Renvoie le nom du probleme
 *
 * @return (Nom&) le nom du probleme
 */
inline const Nom& Probleme_base::le_nom() const { return nom; }

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

// Method which may be called from anywhere:
inline const char* time_format_from(const int reprise_version)
{
  // Depuis la 155 le format de la balise temps est en scientifique
  // pour eviter des erreurs (incoherence entre temps)
  if (reprise_version<155)
    return "%f";
  else
    return "%e";
}

inline const char* Probleme_base::reprise_format_temps() const
{
  return time_format_from(restart_version_);
}

#endif /* Probleme_base_included */
