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
// File:        Champ_Generique_Interpolation.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Generique_Interpolation_included
#define Champ_Generique_Interpolation_included

#include <Champ_Gen_de_Champs_Gen.h>
#include <Domaine_dis.h>

// .DESCRIPTION
//  Un champ generique qui est construit comme une interpolation
//  d'un autre champ generique (interpolation aux sommets ou aux elements).
//  L interpolation sera effectuee sur un domaine qui peut etre
//  le domaine de calcul ou un domaine different specifie par l utilisateur

//// Syntaxe a respecter pour jdd
//
// "nom_champ" Interpolation { [ domaine ] "dom_interp"
//                localisation "loc"
//                source "type_champ_ge" { ...source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" } }
//               }
// "nom_champ" fixe par utilisateur sera le nom du champ generique
// "dom_interp" nom du domaine d interpolation dans le cas ou il differe du domaine de calcul (optionnel)
// "loc" designe la localisation d interpolation "elem" ou "som"
// "type_champ_gen" type d'un champ generique

class Champ_Generique_Interpolation : public Champ_Gen_de_Champs_Gen
{
  Declare_instanciable(Champ_Generique_Interpolation);
public:
  void reset();
  void set_param(Param& param);
  //int lire_motcle_non_standard(const Motcle&, Entree&);
  virtual int     set_localisation(const Motcle& localisation, int exit_on_error = 1);
  virtual int     set_methode(const Motcle& methode, int exit_on_error = 1);
  virtual int     set_domaine(const Nom& nom_domaine, int exit_on_error = 1);
  virtual const Champ_base&  get_champ(Champ& espace_stockage) const;
  virtual const Champ_base&  get_champ_without_evaluation(Champ& espace_stockage) const;
  virtual const Champ_base&  get_champ_with_calculer_champ_post(Champ& espace_stockage) const;

  virtual const DoubleTab&  get_ref_values() const;
  virtual void              get_copy_values(DoubleTab&) const;
  //virtual void  get_xyz_values(const DoubleTab & coords, DoubleTab & values, ArrOfBit & validity_flag) const;

  virtual Entity  get_localisation(const int index = -1) const;
  virtual const Domaine&      get_ref_domain() const;
  virtual void get_copy_domain(Domaine&) const;

  virtual const DoubleTab&  get_ref_coordinates() const;
  virtual void              get_copy_coordinates(DoubleTab&) const;
  //virtual const IntTab&   get_ref_connectivity(Entity index1, Entity index2) const;
  //virtual void            get_copy_connectivity(Entity index1, Entity index2, IntTab &) const;

  virtual const Noms        get_property(const Motcle& query) const;
  void nommer_source();
  void completer(const Postraitement_base& post);
  const Zone_dis_base& get_ref_zone_dis_base() const;
  const   Motcle            get_directive_pour_discr() const;
  void discretiser_domaine();

  const Noms& fixer_noms_compo(const Noms& noms);
  const Noms& fixer_noms_synonyms(const Noms& noms);
  //L attribut compo_ de Champ_Generique_Interpolation n est rempli que pour les Champ_Generique_Interpolation
  //crees par macro et cela afin de reproduire les noms de composantes dans les lml
  Noms compo_,syno_;

private:
  Motcle               localisation_;                 // localisation d interpolation elem, som
  Motcle               methode_;                      // calculer_champ_post, etc...
  Nom                nom_domaine_lu_;        // Nom du domaine lu
  REF(Domaine)         domaine_;                      // domaine sur lequel on veut interpoler le champ (domaine natif si reference nulle)
  Domaine_dis le_dom_dis;                     // rempli si domaine d interpolation different du domaine natif
  // ex : Sonde utilise valeur_aux...() qui necessite de disposer d une zone discretisee
  int optimisation_sous_maillage_,optimisation_demande_;
  ArrOfInt renumerotation_maillage_;
};

#endif
