/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Champ_Generique_Transformation.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Generique_Transformation_included
#define Champ_Generique_Transformation_included

#include <Champ_Gen_de_Champs_Gen.h>
#include <Vect_Parser_U.h>

//
// .DESCRIPTION class Champ_Generique_Transformation
//
// Champ destine a post-traiter une "transformation" dependant de champs generiques et (ou) de x,y,z et t
// La classe porte un  :
//   -vecteur de Parseur (fxyz)
//   -la (les) expression(s) pour exprimer la transformation (les_fct)
//   -la methode de transformation choisie (methode_)

//// Syntaxe a respecter pour jdd
//
// "nom_champ" Transformation {
//                 "type_transfo" "type_info"
//                sources { ...{ ...source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret1" } } ,
//                          ...{ ...source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret2" } } ,
//                            ...
//                          }
//                  [localisation "loc"]
//             }
//
// "nom_champ" fixe par l'utilisateur sera le nom du champ generique.
// "type_transfo" peut etre :
//          "fonction"              permet d'appliquer une formule utilisant les valeurs des champs sources specifies et peut dependre de x,y,z et t.
//                              -> type_info : f(nom_ch1,nom_ch2...,x,y,z,t) expression analytique
//
//          "produit_scalaire"  permet de calculer le produit scalaire de deux vecteurs.
//                              -> type_info : vide car l'expression de transformation est construite automatiquement.
//                                 2 sources de nature vectoriel doivent etre specifiees.
//
//          "norme"              permet de calculer la norme d'un vecteur.
//                              -> type_info : idem a produit_scalaire
//                                 1 source de nature vectoriel doit etre specifiee.
//
//          "vecteur"              permet de construire un champ de nature vectoriel a partir des composantes specifiees.
//                              -> type_info : nom_pb nb_compo f1(nom_ch1,nom_ch2...,x,y,z,t) ...fn(nom_ch1,nom_ch2...,x,y,z,t)
//                                 nom_pb    : nom du probleme.
//                                 nb_compo  : nombre de composantes du champ vectoriel a creer.
//                                 f1,...,fn : expression des composantes du champ vectoriel a creeer.
//                                Si des sources sont specifiees, elles doivent etre de nature scalaire.
//                                 Rq : si aucune source n'est specifiee par l''utilisateur, la version actuelle attribue une
//                                     source par defaut qui se base sur le champ inconnu de la premiere equation du probleme.
//                                     A terme (apres revision de la conception) ce contournement sera a eliminer.
//
//          "composante"              permet de construire un champ scalaire par extraction d'une composante d'un champ vectoriel.
//                              -> type_info : num_compo : numero de la composante a extraire.
//                                1 source de nature vectoriel doit etre specifiee.
//
// "type_champ_gen" type d'un champ generique
// "nom_champ_discret1" designe le nom du champ constituant la premiere source
// "nom_champ_discret2" designe le nom du champ constituant la seconde source
// ...
// "loc" permet de specifier une localisation particuliere pour evaluer les valeurs de l espace de stockage
//  valeurs possibles : "elem", "som", "faces" et "elem_som".
//  Dans le cas ou aucune localisation n'est specifiee, la localisation retenue est celle du support de la premiere source.
//

class Champ_Generique_Transformation : public Champ_Gen_de_Champs_Gen
{

  Declare_instanciable(Champ_Generique_Transformation);

public:

  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void verifier_coherence_donnees();
  void verifier_localisation();
  virtual const Noms get_property(const Motcle& query) const;
  virtual const Champ_base&  get_champ(Champ& espace_stockage) const;
  virtual Entity  get_localisation(const int index = -1) const;
  virtual const Motcle get_directive_pour_discr() const;
  void completer(const Postraitement_base& post);
  void nommer_source();
  int preparer_macro();
  void creer_expression_macro();


protected:

  Nom methode_;                //Methode indiquant le type de transformation retenue
  Noms les_fct;                //Contient l expression de la combinaison
  mutable VECT(Parser_U) fxyz; //Parser utilise pour evaluer la valeur prise par la combinaison
  int nb_comp_;                //Nombre de composantes du champ evalue
  Motcle localisation_;        //Localisation du support d evaluation de l expression
  Nature_du_champ nature_ch;   //Nature du champ evalue

};

#endif
