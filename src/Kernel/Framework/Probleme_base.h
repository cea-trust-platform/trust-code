/****************************************************************************
* Copyright (c) 2020, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Probleme_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/55
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Probleme_base_included
#define Probleme_base_included

#include <Probleme_U.h>
#include <Ref_IntVect.h>
#include <Ref_Schema_Temps_base.h>
#include <Domaine_dis.h>
#include <Discretisation.h>
#include <Postraitements.h>
#include <Ref_Probleme_Couple.h>
#include <Champs_compris_interface.h>
#include <List_Ref_Field_base.h>
#include <Deriv_Sortie_Fichier_base.h>
#include <List_Ref_Loi_Fermeture_base.h>

#include <Sortie_Brute.h>


class Equation_base;
class Milieu_base;
class Postraitement;
class Champ_Fonc;
class Champ_Generique_base;
class EcrFicPartageBin;
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Probleme_base
//     C'est un Probleme_U qui n'est pas un couplage.
//     Le role d'un Probleme_base est la resolution sur un domaine des
//     equations qui le compose.
//     Ses membres sont les attributs et les methodes communs
//     a toutes les classes qui representent des problemes.
//     Un certains nombre d'objets sont associes au probleme pour
//     le constituer:
//       - 1 ou plusieurs equations: objet Equation
//       - 1 Domaine discretise: objet Domaine_Dis
//       - un postraitement: objet Postraitement
//       - Une discretisation: objet Discretistaion
//       - un schema en temps: objet Schema_temp
// .SECTION voir aussi Probleme
//     Classe abstraite dont tous les problemes doivent deriver.
//     Methodes abstraites:
//       int nombre_d_equations() const
//       const Equation_base& equation(int) const
//       Equation_base& equation(int)
//////////////////////////////////////////////////////////////////////////////
class Probleme_base : public Champs_compris_interface, public Probleme_U
{
  Declare_base_sans_constructeur(Probleme_base);

public:

  //////////////////////////////////////////////////
  //                                              //
  // Implementation de l'interface de Probleme_U  //
  //                                              //
  //////////////////////////////////////////////////

  // interface Problem

  virtual void initialize();
  virtual void terminate();

  // interface UnsteadyProblem

  virtual double presentTime() const;
  virtual double computeTimeStep(bool& stop) const;
  virtual bool initTimeStep(double dt);
  virtual bool solveTimeStep();
  virtual void validateTimeStep();
  virtual bool isStationary() const;
  virtual void setStationary(bool);
  virtual void abortTimeStep();

  // interface IterativeUnsteadyProblem

  virtual bool iterateTimeStep(bool& converged);

  // interface FieldIO

  virtual void getInputFieldsNames(Noms& noms) const;
  virtual void getOutputFieldsNames(Noms& noms) const;

  // interface Probleme_U

  virtual int postraiter(int force=1);
  virtual int limpr() const;
  virtual int lsauv() const;
  virtual void sauver() const;
  virtual void allocation() const;
  virtual bool updateGivenFields();
  virtual double futureTime() const;

  virtual REF(Field_base) findInputField(const Nom& name) const;
  virtual REF(Champ_Generique_base) findOutputField(const Nom& name) const;

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Fin de l'implementation de l'interface de Probleme_U  //
  //                                                       //
  ///////////////////////////////////////////////////////////

  Probleme_base();
  virtual void associer();
  virtual Entree& lire_equations(Entree& is);
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
  virtual const Equation_base& get_equation_by_name(const Nom&) const;
  virtual Equation_base& getset_equation_by_name(const Nom&);
  virtual const Milieu_base& milieu() const;
  virtual Milieu_base& milieu();
  virtual int sauvegarder(Sortie& ) const;
  virtual int reprendre(Entree& );
  virtual void imprimer(Sortie& os) const; // Appelle imprimer sur chaque equation
  virtual double calculer_pas_de_temps() const;
  virtual void mettre_a_jour(double temps) ;
  virtual void preparer_calcul() ;

  // Methodes d'acces aux membres prives.

  virtual void associer_sch_tps_base(const Schema_Temps_base&);
  virtual void associer_domaine(const Domaine&);
  virtual void associer_milieu_base(const Milieu_base&);
  virtual int associer_(Objet_U&);
  virtual void discretiser(const Discretisation_base&);
  virtual void finir();

  const Schema_Temps_base& schema_temps() const;
  Schema_Temps_base& schema_temps();
  const Domaine& domaine() const;
  Domaine& domaine();
  const Domaine_dis& domaine_dis() const;
  Domaine_dis& domaine_dis();
  int is_QC() const;

  virtual int allocate_file_size(long int& size) const;
  virtual int file_size_xyz() const;

  inline void nommer(const Nom&);
  inline const Nom& le_nom() const;
  inline const Discretisation_base& discretisation() const;
  inline Postraitements& postraitements()
  {
    return les_postraitements;
  };
  inline const Postraitements& postraitements() const
  {
    return les_postraitements;
  };
  void init_postraitements();
  virtual int expression_predefini(const Motcle& motlu, Nom& expression);
  inline const char* reprise_format_temps() const;
  inline int& reprise_effectuee()
  {
    return reprise_effectuee_;
  };
  inline int reprise_effectuee() const
  {
    return reprise_effectuee_;
  };

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  //Pour acceder a un champ de la liste portee pas le postraitement.
  virtual const Champ_Generique_base& get_champ_post(const Motcle& nom) const;
  virtual int comprend_champ_post(const Motcle& nom) const;
  virtual int verifie_tdeb_tfin(const Motcle& nom) const;

  // Fonctions de recherche de IntTab pour le postraitement
  virtual int a_pour_IntVect(const Motcle&, REF(IntVect)& ) const;
  virtual void lire_postraitement_interfaces(Entree& is);
  virtual void postraiter_interfaces(const Nom& nom_fich, Sortie& s, const Nom& format, double temps);

  virtual void addInputField(Field_base& f);
  void sauver_xyz(int) const;
  void set_coupled(int i)
  {
    coupled_=i;
  };
  int get_coupled() const
  {
    return coupled_;
  };

protected :

  Domaine_dis le_domaine_dis;
  Postraitements les_postraitements;
  REF(Schema_Temps_base) le_schema_en_temps;
  REF(Discretisation_base) la_discretisation;
  LIST(REF(Field_base)) input_fields; // List of input fields inside this problem.

  mutable DERIV(Sortie_Fichier_base) ficsauv_;
  mutable Sortie_Brute* osauv_hdf_;

  int reprise_effectuee_;
  int reprise_version_;
  Nom nom_fich;
  Nom format_sauv;
  int restart_file;

  // Flags used to control the calling order and raise exceptions
  bool initialized;  // true if initialize was called
  bool terminated;   // true if terminate was called
  bool dt_defined;   // true if computation interval is defined
  // set to true by initTimeStep, to false by validateTimeStep & abortTimeStep
  bool dt_validated; // true if last computation was validated
  // set to true by validateTimeStep, to false by initTimeStep

  mutable double tstat_deb_, tstat_fin_;

  static long int file_size;        // Espace disque pris par les sauvegarde XYZ
  static int bad_allocate;        // 1 si allocation reussi, 0 sinon
  static int nb_pb_total;        // Nombre total de probleme
  static int num_pb;                // numero du probleme
  mutable Nom error;                // Erreur d'allocation
  int coupled_;			// Flag to indicate it is a part of a coupled problem

  LIST(REF(Loi_Fermeture_base)) liste_loi_fermeture_; // liste des fermetures associees au probleme

};

// Description:
//    surcharge Objet_U::nommer(const Nom&)
//    Donne un nom au probleme
// Precondition:
// Parametre: Nom& name
//    Signification: le nom a donner au probleme
//    Valeurs par defaut:
//    Contraintes: reference const
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Probleme_base::nommer(const Nom& name)
{
  nom=name;
}

// Description:
//    surcharge Objet_U::le_nom()
//    Renvoie le nom du probleme
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du probleme
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Probleme_base::le_nom() const
{
  return nom;
}

// Description:
//    Renvoie la discretisation associee au probleme
// Precondition: La discretisation doit etre une REF(Discretisation_base) non nulle
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Discretisation_base&
//    Signification: discretisation associee au probleme
//    Contraintes: reference constante
// Exception: la discretisation n'est pas construite
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Discretisation_base& Probleme_base::discretisation() const
{
  if(!la_discretisation.non_nul())
    {
      Cerr << que_suis_je() << " has not been discretized!" << finl;
      exit();
    }
  return la_discretisation.valeur();
}

// Method which may be called from anywhere:
inline const char* time_format_from(const int& reprise_version)
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
  return time_format_from(reprise_version_);
}

#endif

