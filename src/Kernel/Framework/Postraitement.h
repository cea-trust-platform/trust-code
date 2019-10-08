/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Postraitement.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/patch_168/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Postraitement_included
#define Postraitement_included

#include <Operateurs_Statistique_tps.h>
#include <Champs_Fonc.h>
#include <Sondes.h>
#include <Sondes_Int.h>
#include <EcrFicPartageBin.h>
#include <List_Ref_IntVect.h>
#include <Interprete.h>
#include <Postraitement_base.h>
#include <Format_Post.h>
#include <Liste_Noms.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Liste_Champ_Generique.h>
#include <Champ_Gen_de_Champs_Gen.h>
#include <Parser_U.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Postraitement
//     La classe est dotee -d une liste de champs generiques champs_post_complet_ qui contient
//                                 tous les champs generiques du post-traitement
//                           -d une liste d identifiants noms_champs_a_post_ qui contient l identifiant
//                            des champs a post-traiter
//
//     -Lecture des champs generiques declares dans le bloc "definition_champs" (ajoutes dans  champs_post_complet_)
//     et ajout dans cette liste des champs crees par macro (declenche par ancienne syntaxe dans "champs" et "statistiques")
//        Construction de la liste des identifiants noms_champs_a_post_ au cours des operations de lecture
//     -Mise a jour des champs generiques, en realite mise a jour des operateurs statistiques pour les champs qui en porte
//     -postraitement realise dans postraiter_champs()
//        Parcours des identifiants
//        Le champ generique correspondant a un identifiant est recupere par :             get_champ_post("identifiant")
//        Pour ce champ generique : on calcul les valeurs a ecrire par :                     get_champ(espace_stockage)
//                                  on recupere les informations complementaires par : get_property(), get_time() ...
//        Ecriture des valeurs calculees :                                             postraiter(...)
//
//
//  Les macros utilises pour la creation des champs generiques sont detaillees dans le .cpp de la classe
//  Voir creer_champ_post() et creer_champ_post_stat()
//
//.SECTION voir aussi
//     Hierarchie des champs generiques (Champ_Generique_base)
//
//  La syntaxe a respecter dans le jdd
//  Postraitement {
//    Sondes
//     {
//        ...
//     }
//    Definition_champs
//    {
//     //Specification des champs generiques
//    }
//    Champs
//    {
//     -Creation de champ generique par macro si utilisation ancienne syntaxe (ex : vitesse elem) et ajout
//     identifiant dans la liste noms_champs_a_post_
//     -Ajout dans la liste  noms_champs_a_post_ pour cas d un champ declare dans le bloc "definition_champs"
//    }
//    Statistiques
//    {
//        t_deb val_1 t_fin_val_2
//    -Creation de champ generique par macro si utilisation ancienne syntaxe (ex : Moyenne vitesse elem) et ajout
//     identifiant dans la liste noms_champs_a_post_
//    }
//  }
//
//////////////////////////////////////////////////////////////////////////////
class Postraitement : public Postraitement_base
{
  Declare_instanciable_sans_constructeur(Postraitement);
public:
  //
  // Methodes reimplementees :
  //
  void associer_nom_et_pb_base(const Nom&, const Probleme_base&);
  void postraiter(int forcer);
  void mettre_a_jour(double temps);
  void finir();
  int sauvegarder(Sortie& os) const;
  int reprendre(Entree& is);
  void completer();

  //
  // Methodes specifiques :
  //
  Postraitement();

  inline const Sondes& les_sondes() const;
  inline Probleme_base& probleme();
  inline const Probleme_base& probleme() const;

  void init();
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  int postraiter_sondes();
  int traiter_sondes();
  int postraiter_champs();
  int traiter_champs();
  int lire_champs_a_postraiter(Entree& );                //Lance eventuellement la creation de champs generiques par macro
  //et construit la liste noms_champs_a_post_ des champs post-traites
  int lire_champs_stat_a_postraiter(Entree&);        //idem pour statistiques
  int lire_champs_operateurs(Entree& is);                //Lecture d un champ generique, nomme et complete
  void complete_champ(Champ_Generique& champ,const Motcle& motlu);
  int postraiter_tableaux();
  int traiter_tableaux();
  int lire_tableaux_a_postraiter(Entree& );
  inline int lpost(double, double) const;
  inline int lpost_champ(double) const;
  inline int lpost_stat(double) const;
  inline int ind_post(int);
  inline double dt_post_ch() const
  {
    return dt_post_ch_;
  };
  inline Nom nom_fich() const
  {
    return nom_fich_;
  };
  // int contient_champ_fonc(const Motcle& );
  inline int lpost_tab(double) const;
  static inline LIST(Nom)& noms_fichiers_sondes()
  {
    return noms_fichiers_sondes_;
  };
  inline int& est_le_premier_postraitement_pour_nom_fich()
  {
    return est_le_premier_postraitement_pour_nom_fich_;
  };
  inline int& est_le_dernier_postraitement_pour_nom_fich()
  {
    return est_le_dernier_postraitement_pour_nom_fich_;
  };
  inline Operateurs_Statistique_tps& les_statistiques()
  {
    return les_statistiques_;
  };
  inline int sondes_demande()
  {
    return sondes_demande_;
  }
  inline int champs_demande()
  {
    return champs_demande_;
  }
  inline int stat_demande() const
  {
    return stat_demande_;
  }
  inline int stat_demande_definition_champs() const
  {
    return stat_demande_definition_champs_;
  }
  inline int tableaux_demande()
  {
    return tableaux_demande_;
  }


  inline LIST(Nom)& noms_champs_a_post();
  inline const Liste_Champ_Generique& champs_post_complet() const;

  //On distingue le postraitement d un tableau et d un tenseur
  int postraiter(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int& ncomp,
                 const double& temps_champs,double& temps_courant,
                 Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs,int tenseur);

  int postraiter_tableau(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int& ncomp,
                         const double& temps_champs,double& temps_courant,
                         Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs);

  int postraiter_tenseur(const Domaine& dom,const Noms& unites,const Noms& noms_compo,const int& ncomp,
                         const double& temps_champs,double& temps_courant,
                         Nom nom_post,const Nom& localisation,const Nom& nature,const DoubleTab& valeurs);


  virtual const Champ_Generique_base& get_champ_post(const Motcle& nom) const;

  Nom set_expression_champ(const Motcle& motlu1,const Motcle& motlu2,
                           const Motcle& motlu3,const Motcle& motlu4,
                           const int& trouve);

  //Methodes macro pour generer la creation de :
  //-Champ_Generique_Interpolation
  void creer_champ_post(const Motcle& motlu1,const Motcle& motlu2,Entree& s);
  //-Champ_Generique_Interpolation_Statistiques
  void creer_champ_post_stat(const Motcle& motlu1,const Motcle& motlu2,const Motcle& motlu3,const Motcle& motlu4,const double& t_deb, const
                             double& t_fin,Entree& s);
  //-Champ_Generique_Morceau_Equation
  void creer_champ_post_moreqn(const Motcle& type,const Motcle& option,const int& num_eq,const int& num_morceau,const int& compo,Entree& s);

  //Methode macro pour le cas des champs med
  void creer_champ_post_med(const Motcle& motlu1,const Motcle& motlu2,Entree& s);

  //Methode comprend_champ_post() qui indique si l identifiant correspond au nom d un Champ_Generique
  //ou a l une de ses composantes-les sources sont testees recusrivement
  int comprend_champ_post(const Motcle& identifiant) const;

  //temporaire a reviser
  void verifie_nom_et_sources(const Champ_Generique& champ);
  static Nom get_nom_localisation(const Entity& loc);

  int champ_fonc(Motcle& nom_champ, REF(Champ_base)& mon_champ, REF(Operateur_Statistique_tps_base)&
                 operateur_statistique) const;

  inline int& compteur_champ_stat();
  inline const double& tstat_deb() const;
  inline const double& tstat_fin() const;
  int cherche_stat_dans_les_sources(const Champ_Gen_de_Champs_Gen& ch, Motcle nom);

protected:

  int est_le_premier_postraitement_pour_nom_fich_;
  int est_le_dernier_postraitement_pour_nom_fich_;
  double dt_post_ch_ ;           // ecriture des champs sur fichier nom_du_cas.lml tous les dt_post
  double dt_post_stat_;         // ecriture des statistiques sur fichier nom_du_cas.lml tous les dt_stat
  double dt_post_tab;           // ecriture des tableaux d'entiers sur fichier nom_du_cas.lml

  int nb_pas_dt_post_;
  Parser_U fdt_post;

  Sondes les_sondes_;           // Sondes a traiter
  Sondes_Int les_sondes_int_;   // Sondes pour des tableaux d'entiers
  Operateurs_Statistique_tps les_statistiques_; // Liste d'operateurs statistiques a traiter

  LIST(Nom) noms_champs_a_post_;                 //contient les identifiants des champs a postraiter
  Liste_Champ_Generique champs_post_complet_;   //contient l ensemble des champs generiques dedies au post-traitement

  //attributs pour sauvegarde-reprise
  //redondant avec attributs des Champ_Generique_Statistiques
  //Tout le processus de sauvegarde reprise devrait etre gere par le champ statistique
  int nb_champs_stat_;
  double tstat_deb_, tstat_fin_, tstat_dernier_calcul_;

  //Otion a gerer par le champ statistique
  int lserie_;
  double dt_integr_serie_;

  LIST(REF(IntVect)) tableaux_a_postraiter_; // Liste de references a des tableaux a post-traiter
  LIST(Nom) noms_tableaux_;

  Format_Post format_post;

private :
  static LIST(Nom) noms_fichiers_sondes_;
  int sondes_demande_;
  int champs_demande_;
  int stat_demande_;
  int stat_demande_definition_champs_;
  int binaire;
  int tableaux_demande_;
  Nom nom_fich_;
  Nom format;
  double temps_;
  static Motcles formats_supportes;
  REF(Domaine) le_domaine;
  double dernier_temps; // temps du precedent appel a postraiter()
  Nom option_para;

  REF(Zone_dis_base) zone_dis_pour_faces;
};




// Description:
//    Renvoie les Sondes associees au postraitement.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sondes&
//    Signification: les Sondes associees au postraitement
//    Contraintes: reference cosntante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Sondes& Postraitement::les_sondes() const
{
  return les_sondes_;
}

inline LIST(Nom)& Postraitement::noms_champs_a_post()
{
  return noms_champs_a_post_;
}


inline const Liste_Champ_Generique& Postraitement::champs_post_complet() const
{
  return champs_post_complet_;
}

// Description:
//    Renvoie une reference sur le probleme associe.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Probleme_base&
//    Signification: le probleme associe
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Probleme_base& Postraitement::probleme()
{
  return mon_probleme.valeur();
}


// Description:
//    Renvoie une reference sur le probleme associe.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Probleme_base&
//    Signification: le probleme associe
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Probleme_base& Postraitement::probleme() const
{
  return mon_probleme.valeur();
}


inline int Postraitement::lpost(double temps_courant, double dt_post) const
{
  double epsilon = 1.e-8;
  if (dt_post<=temps_courant - dernier_temps)
    return 1;
  else
    {
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      double i, j;
      modf(temps_courant/dt_post + epsilon, &i);
      modf(dernier_temps/dt_post + epsilon, &j);
      return ( i>j );
    }
}

inline int Postraitement::ind_post(int nb_pas_dt)
{
  if (nb_pas_dt%nb_pas_dt_post_==0)
    return 1;
  else
    return 0;
}


// Description:
//    Test de postraitement en tenant compte de l'evolution en temps
//    du champ.
//    Renvoie VRAI si le champ necessite un postraitement, vu
//    le temps courant et le pas de temps fourni.
// Precondition:
// Parametre: double temps_courant
//    Signification: le temps courant
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double dt
//    Signification: le pas de temps qui vient d'etre accompli
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: valeur booleenne, VRAI si le pas de temp et le temps
//                   courant fournis indique qu'un postraitement est
//                   necessaire, FAUX sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Postraitement::lpost_champ(double temps_courant) const
{
  return lpost(temps_courant, dt_post_ch_);
}

inline int Postraitement::lpost_stat(double temps_courant) const
{
  return lpost(temps_courant, dt_post_stat_);
}

inline int Postraitement::lpost_tab(double temps_courant) const
{
  return lpost(temps_courant, dt_post_tab);
}

inline int& Postraitement::compteur_champ_stat()
{
  return nb_champs_stat_;
}
inline const double& Postraitement::tstat_deb() const
{
  return tstat_deb_;
}
inline const double& Postraitement::tstat_fin() const
{
  return tstat_fin_;
}


#endif

