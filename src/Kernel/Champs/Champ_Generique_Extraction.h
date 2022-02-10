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
// File:        Champ_Generique_Extraction.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Generique_Extraction_included
#define Champ_Generique_Extraction_included

#include <Champ_Gen_de_Champs_Gen.h>
#include <Domaine_dis.h>

// .DESCRIPTION
//  Un champ generique qui effctue l extraction d un champ sur une frontiere

//// Syntaxe a respecter pour jdd
//
// "nom_champ" Extraction { domaine "nom_dom" nom_frontiere "nom_fr" [ methode ] "type_methode"
//                source "type_champ_gen" { ...source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" } }
//               }
// "nom_champ" fixe par utilisateur sera le nom du champ generique
// "nom_dom" nom du doamine a laquelle appartient la frontiere
// "nom_fr" nom de la frontiere sur laquelle on veut faire l extraction
// "type_methode" type de methode pour effectuer l extraction
//                  ("trace" methode par defaut ou "champ_frontiere" pour extraire le_champ_front)
// "type_champ_gen" type d'un champ generique

class Champ_Generique_Extraction : public Champ_Gen_de_Champs_Gen
{
  Declare_instanciable_sans_constructeur(Champ_Generique_Extraction);
public:
  Champ_Generique_Extraction();
  void set_param(Param& param) override;
  Entity  get_localisation(const int index = -1) const override;
  const Champ_base&  get_champ(Champ& espace_stockage) const override;
  const Champ_base&  get_champ_without_evaluation(Champ& espace_stockage) const override;
  const Noms        get_property(const Motcle& query) const override;
  void nommer_source() override;
  void completer(const Postraitement_base& post) override;
  const Domaine& get_ref_domain() const override;
  void get_copy_domain(Domaine&) const override;
  const Zone_dis_base& get_ref_zone_dis_base() const override;
  void discretiser_domaine();
  const  Motcle  get_directive_pour_discr() const override;

protected :

  Nom dom_extrac_;                //Nom du domaine d extraction
  Nom nom_fr_;                        //Nom de la frontiere sur laquelle on fait l extraction
  Nom methode_;                        //Type de methode pour extraire ("trace" ou "champ_frontiere")
  REF(Domaine) domaine_;        //Reference sur le domaine d extraction
  Domaine_dis le_dom_dis;        //Le domaine discretise correspondant a domaine_.valeur()
};

#endif
