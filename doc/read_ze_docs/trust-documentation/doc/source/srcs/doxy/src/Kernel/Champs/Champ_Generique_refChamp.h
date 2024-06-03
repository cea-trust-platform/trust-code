/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Champ_Generique_refChamp_included
#define Champ_Generique_refChamp_included

#include <Postraitement.h>

/*! @brief Classe de champ particuliere qui encapsule une reference a un champ volumique de TRUST de type Champ_base.
 *
 *   La classe ne gere pas la memoire de Champ_base.
 *
 */

// Syntaxe a respecter pour jdd
//
// "nom_champ" refChamp { Pb_champ "nom_pb" "nom_champ_discret" }
// "nom_champ" fixe par utilisateur sera le nom du champ generique
// "nom_pb" nom du probleme auquel appartient le champ discret
// "nom_champ_discret" nom du champ cible ou une de ses composantes

class Champ_Generique_refChamp : public Champ_Generique_base
{
  Declare_instanciable(Champ_Generique_refChamp);
public:

  void initialize(const Champ_base& champ);
  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int            get_dimension() const override;
  void              get_property_names(Motcles& list) const override;
  const Noms        get_property(const Motcle& query) const override;
  Entity            get_localisation(const int index = -1) const override;

  const DoubleTab&  get_ref_values() const override;
  void              get_copy_values(DoubleTab&) const override;
  void              get_xyz_values(const DoubleTab& coords, DoubleTab& values, ArrOfBit& validity_flag) const override;

  const Domaine_Cl_dis_base&  get_ref_zcl_dis_base() const override;

  const DoubleTab&  get_ref_coordinates() const override;
  void              get_copy_coordinates(DoubleTab&) const override;
  const IntTab&     get_ref_connectivity(Entity index1, Entity index2) const override;
  void              get_copy_connectivity(Entity index1, Entity index2, IntTab&) const override;

  const Probleme_base& get_ref_pb_base() const override;
  virtual const Champ_base& get_ref_champ_base() const; //renvoie la reference au champ encapsule
  void              reset() override;
  void completer(const Postraitement_base& post) override;
  void              mettre_a_jour(double temps) override;

  //get_champ() particulier car n utilise pas d espace de stockage
  //Actualise le champ discret si champ calcule du probleme
  //et renvoie la reference
  const Champ_base& get_champ(Champ& espace_stockage) const override;
  const Champ_base& get_champ_without_evaluation(Champ& espace_stockage) const override;

  virtual void set_ref_champ(const Champ_base&);

  double get_time() const override;
  const  Motcle            get_directive_pour_discr() const override;
  void set_nom_champ(const Motcle&);
  void nommer_source(const Postraitement_base& post);
  int get_info_type_post() const override;

protected:

  REF(Champ_base) ref_champ_;

  //temporaire voir utilite
  Motcle localisation_;

  Nom nom_champ_;

};
#endif
