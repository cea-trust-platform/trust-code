/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_Generique_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Generique_base_included
#define Champ_Generique_base_included

#include <Domaine.h>
#include <Champ.h>

class ArrOfBit;
class Postraitement_base;
class Discretisation_base;
class Param;

enum Entity { NODE, SEGMENT, FACE, ELEMENT };

// Description:
// Classe de base des champs generiques pour importation d un champ discret
// et actions elementaires sur ce champ
//  (postraitement, etc)
// Attention: toutes les methodes sont PARALLELES, il faut les appeler
//  simultanement sur tous les processeurs (get_domain() peut par exemple
//  construire le domaine parallele avant de le renvoyer).
class Champ_Generique_base : public Objet_U
{
  Declare_base(Champ_Generique_base);
public:
  virtual void set_param(Param& param)=0;
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);
  virtual int  get_dimension() const;
  virtual double  get_time() const; //rend le temps du champ encapsule
  virtual const   Probleme_base& get_ref_pb_base() const;
  virtual const   Discretisation_base&  get_discretisation() const;
  virtual const   Motcle                get_directive_pour_discr() const;

  void nommer(const Nom& nom);
  virtual const Nom& get_nom_post() const;
  //rend -1 si identifiant est le nom, numero de composante sinon
  static inline int composante(const Nom& nom_test,const Nom& nom, const Noms& composantes,const Noms& synonyms);

  virtual void              get_property_names(Motcles& list) const;
  virtual const Noms        get_property(const Motcle& query) const;  //repond aux requetes nom, nom_cible, unites et composantes

  virtual Entity            get_localisation(const int index = -1) const; //localisation ELEMENT ou SOM sont post-traitables
  virtual int            get_nb_localisations() const;

  virtual const DoubleTab&  get_ref_values() const;
  virtual void              get_copy_values(DoubleTab&) const;
  virtual void              get_xyz_values(const DoubleTab& coords, DoubleTab& values, ArrOfBit& validity_flag) const;

  virtual const Domaine&    get_ref_domain() const; //rend une reference au domaine associe au champ
  virtual void              get_copy_domain(Domaine&) const;
  virtual const Zone_dis_base&  get_ref_zone_dis_base() const; //rend la zone discretisee liee au domaine
  virtual const Zone_Cl_dis_base&  get_ref_zcl_dis_base() const; //rend la zcl discretisee liee a l equation portant le champ cible

  virtual const DoubleTab&  get_ref_coordinates() const;
  virtual void              get_copy_coordinates(DoubleTab&) const;
  virtual const IntTab&     get_ref_connectivity(Entity index1, Entity index2) const;
  virtual void              get_copy_connectivity(Entity index1, Entity index2, IntTab&) const;

  // Remet l'objet dans l'etat obtenu par le constructeur par defaut
  virtual void  reset() = 0;
  virtual void completer(const Postraitement_base& post) = 0; //Complete l operateur eventuellement porte par le champ
  //et nomme les sources par defaut
  virtual void  mettre_a_jour(double temps) = 0;              //Mise a jour d un operateur eventuellement porte par le champ

  //  La methode get_champ() est en particulier appelee par la classe de postraitement lorsque
  //  le champ doit etre postraite (dt_post ecoule).
  //  L'appelant doit fournir un champ "espace_stockage" non type comme parametre
  //   "espace_stockage".
  //
  //  Soit elle renvoie un champ existant (voir Champ_Generique_refChamp)
  //   et elle n'utilise pas espace_stockage.
  //  Soit elle construit un nouveau champ qu'elle stocke dans espace_stockage,
  //   et la valeur de retour est espace_stockage.valeur()
  //  L'appelant recupere le resultat du calcul dans la valeur de retour,
  //   sachant qu'elle peut eventuellement referencer espace_stockage
  //   (donc, ne pas detruire espace_stockage trop tot).

  // Les etapes de creation de l espace de stockage sont :
  // espace_stockage.typer(type_champ)
  // espace_stockage.associer_zone_dis_base(une_zone_dis)
  // espace_stockage->fixer_nb_comp(nb_comp);
  // espace_stockage->fixer_nb_valeurs_nodales(nb_ddl);
  // Calcul des valeurs par instruction de la forme
  // espace_stockage.valeurs() = Operateur.calculer(source.valeurs())
  // espace_stockage.valeurs().echange_espace_virtuel()
  //return espace_stockage.valeur()

  virtual const Champ_base&   get_champ(Champ& espace_stockage) const = 0;
  virtual const Champ_base&   get_champ_without_evaluation(Champ& espace_stockage) const=0;
  /*
    {
      return get_champ(espace_stockage);
    };
  */

  //get_champ_post() renvoie le champ si l identifiant passe en parametre designe
  //le nom du champ ou l une de ses composantes
  virtual const Champ_Generique_base& get_champ_post(const Motcle& nom) const;

  //renvoie 1 si le champ est identifie, 0 sinon
  virtual int comprend_champ_post(const Motcle& identifiant) const;

  //get_info_type_post() renvoie 0 si l on doit postraiter un tableau, 1 pour un tenseur
  virtual int get_info_type_post() const = 0;

  //fixe l attribut identifiant_appel_ du champ pour indiquer si la requete
  //a ete lancee par le nom ou une composante du champ (cf Champ_Generique_Interpolation)
  void fixer_identifiant_appel(const Nom& identifiant);

protected:
  static void               assert_parallel(int);

  Nom nom_post_;
  Nom identifiant_appel_;

  Nom nom_pb_;
};

inline void Champ_Generique_base::fixer_identifiant_appel(const Nom& identifiant)
{
  identifiant_appel_ = identifiant;
}

inline int Champ_Generique_base::composante(const Nom& nom_test,const Nom& nom,const Noms& composantes, const Noms& syno)
{
  Motcle motlu(nom_test);
  if (motlu == Motcle(nom))
    return -1;
  for (int i=0; i<syno.size(); i++)
    if (syno[i]==motlu) return -1;
  int n = composantes.size();
  Motcles les_noms_comp(n);
  for (int i=0; i<n; i++)
    les_noms_comp[i] = composantes[i];
  int ncomp = les_noms_comp.search(motlu);
  if (ncomp == -1)
    {
      Cerr << "Error TRUST, the identifier : " << nom_test << finl
           << "did not designate the name of the field tested nor any of its components\n";
      Cerr <<" the components of field named "<<nom <<" are :"<< finl;
      for (int ii=0; ii<n; ii++)
        Cerr << composantes[ii] << " ";
      Cerr << finl;
      exit();
    }
  return ncomp;
}
class Champ_Generique_erreur
{

public:
  Nom mot1;

  Champ_Generique_erreur(const Nom mot2)
  {
    mot1 = mot2;
    Cerr<<"Error of type : "<<mot1<<finl;
    Process::exit();
  }

};

#endif
