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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Param.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Param_included
#define Param_included

#include <TRUSTTabs_forward.h>
#include <Objet_a_lire.h>

class Param;
class Entree;
class Motcle;
class Deriv_;
class ptrParam;     // Defined below in this file

// Description: cette classe permet de factoriser les readOn des Objet_U.
//  On peut lui ajouter un int, un double, un Objet_U, un flag
//  associer a un nom. ajouter_non_std permet d'appeler la methode
//  lire_motcle_non_standard de l'objet passe en parametre.
//  To give synonyms or translation for parameters, use the following syntax:
//  ajouter("keyword|french_keyword|japan_keyword",...)
//  Example: Schema_Temps_base.pp
class Param
{
public:
  enum Nature { OPTIONAL = 0, REQUIRED = 1 };
  Param(const char *);
  // ajout d'argument
  void ajouter(const char *,int* ,Param::Nature nat = Param::OPTIONAL);//int opt=1);
  void ajouter(const char *,double* ,Param::Nature nat = Param::OPTIONAL);//int opt=1);
  void ajouter(const char *, Objet_U* ,Param::Nature nat = Param::OPTIONAL);//int opt=1);
  void ajouter_arr_size_predefinie(const char *, ArrOfInt* ,Param::Nature nat = Param::OPTIONAL);//int opt=1);
  void ajouter_arr_size_predefinie(const char *, ArrOfDouble* ,Param::Nature nat = Param::OPTIONAL);//int opt=1);
  void ajouter_deriv(const char *, const char *prefixe, Deriv_ *, Param::Nature nat = Param::OPTIONAL);
  void ajouter_flag(const char *,int* ,Param::Nature nat = Param::OPTIONAL);//int opt=1);
  Param& ajouter_param(const char *, Param::Nature nat = Param::OPTIONAL);
  void ajouter_non_std(const char *,Objet_U* ,Param::Nature nat = Param::OPTIONAL);//int opt=1);
  void ajouter_condition(const char* condition, const char* message,const char*  name=0);
  void supprimer(const char *);
  void supprimer_condition(const char*  name);
  void dictionnaire(const char *, int);
  Param& dictionnaire_param(const char *, int);

  // ajout type (pour eli)
  inline void ajouter_int(const char * c,int* val,Param::Nature nat = Param::OPTIONAL) { ajouter(c,val,nat); }
  inline void ajouter_double(const char * c,double* val,Param::Nature nat = Param::OPTIONAL) { ajouter(c,val,nat); }
  inline void ajouter_objet(const char *c , Objet_U* obj,Param::Nature nat = Param::OPTIONAL) { ajouter(c,obj,nat); }

  int lire_avec_accolades_depuis(Entree& is);
  int lire_sans_accolade(Entree& is);
  inline int lire_avec_accolades(Entree& is) { return lire_avec_accolades_depuis(is); }
  //int verifier_avant_ajout(const char*);

  int read(Entree& is,int with_acco=1);
  void print(Sortie& s) const;

  inline const LIST(Nom)& get_list_mots_lus() const { return list_parametre_lu_ ; }

  double get_value(const Nom& mot_lu) const;
  int check();
protected:
  //int verifier_avant_ajout(char*);
  Param();
  Objet_a_lire& create_or_get_objet_a_lire(const char *);

  LIST(Objet_a_lire) list_parametre_a_lire_;
  Nom proprietaire_;
  LIST(Nom) list_parametre_lu_,list_conditions_,list_message_erreur_conditions_, list_nom_conditions_;

};

#endif
