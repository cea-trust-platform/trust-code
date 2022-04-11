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
// File:        Champ_Gen_de_Champs_Gen.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Gen_de_Champs_Gen_included
#define Champ_Gen_de_Champs_Gen_included

#include <Liste_Champ_Generique.h>
#include <List_Ref_Champ_Generique_base.h>

class Champ_Fonc;

// .DESCRIPTION
//  Classe de base des champs generiques ayant comme source d'autres champs generiques
//  L'utilisation des methodes de la classe repose sur un principe de recursivite
//  Une instruction sera de maniere generale appliquee (ou deleguer) a un champ generique
//  source qui lui meme appliquera (ou deleguera) l action

// Syntaxe type pour les classes filles instanciables
//   "nom_champ"  "type_champ_gen" { source "type_champ_gen" { source ... } }
//  ou
//   "nom_champ"  "type_champ_gen" { source_reference "nom_source_ref" }
// "nom_champ" fixe par l utilisateur sera le nom du champ generique
// "type_champ_gen" designe le type du champ generique selectionne
// nom_source_ref designe le nom d un champ generique deja defini

class Champ_Gen_de_Champs_Gen : public Champ_Generique_base
{
  Declare_base(Champ_Gen_de_Champs_Gen);
public:

  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual LIST(Champ_Generique)& get_set_sources();
  virtual Champ_Generique&        get_set_source();
  void   reset() override;
  int sauvegarder(Sortie& os) const override;
  int reprendre(Entree& is) override;
  void completer(const Postraitement_base& post) override;
  void   mettre_a_jour(double temps) override;

  virtual Champ_Fonc& creer_espace_stockage(const Nature_du_champ& nature,
                                            const int nb_comp,
                                            Champ_Fonc& es_tmp) const;

  virtual const Champ_Generique_base&      get_source(int i) const;
  virtual Champ_Generique_base&      set_source(int i) ;
  virtual int                      get_nb_sources() const;
  int            get_dimension() const override;
  void              get_property_names(Motcles& list) const override;
  double            get_time() const override;

  const Probleme_base& get_ref_pb_base() const override;
  const Discretisation_base&  get_discretisation() const override;
  const Motcle                      get_directive_pour_discr() const override;

  const Noms        get_property(const Motcle& query) const override;
  Entity            get_localisation(const int index = -1) const override;

  const DoubleTab&  get_ref_values() const override;
  void              get_copy_values(DoubleTab&) const override;
  void              get_xyz_values(const DoubleTab& coords, DoubleTab& values, ArrOfBit& validity_flag) const override;

  const Domaine&    get_ref_domain() const override;
  void              get_copy_domain(Domaine&) const override;

  const Zone_dis_base&  get_ref_zone_dis_base() const override;
  const Zone_Cl_dis_base&  get_ref_zcl_dis_base() const override;

  const DoubleTab&  get_ref_coordinates() const override;
  void              get_copy_coordinates(DoubleTab&) const override;
  const IntTab&     get_ref_connectivity(Entity index1, Entity index2) const override;
  void              get_copy_connectivity(Entity index1, Entity index2, IntTab&) const override;

  void nommer_sources();
  virtual void nommer_source();
  int get_info_type_post() const override;

  const Champ_Generique_base& get_champ_post(const Motcle& nom) const override;
  int comprend_champ_post(const Motcle& identifiant) const override;

  //Methode pour changer t_deb et t_fin pour des reprises de statistiques
  //ou pour les statistiques en_serie
  virtual void fixer_serie(const double t1,const double t2);
  virtual void fixer_tstat_deb(const double t1,const double t2);
  virtual void lire_bidon(Entree& is) const;


protected:

  LIST(Nom) noms_sources_ref_;
  LIST(REF(Champ_Generique_base)) sources_reference_; //permet de creer une source en faisant une reference a un
  //champ generique deja defini a partir de son nom (noms_ource_ref_)

private:
  LIST(Champ_Generique) sources_;        //Attribut qui designent les sources de "premier niveau"
  //Chacune de ses sources est susceptible de posseder une
  //ou plusieurs sources
};

#endif
