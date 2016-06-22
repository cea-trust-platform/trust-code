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
// File:        Champ_Generique_Morceau_Equation.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Generique_Morceau_Equation_included
#define Champ_Generique_Morceau_Equation_included

#include <Champ_Gen_de_Champs_Gen.h>
#include <Ref_Equation_base.h>

class MorEqn;

// .DESCRIPTION class Champ_Generique_Morceau_Equation
// Champ destine a post-traiter une quantite liee a un morceau d equation
// Consulter la hierarchie de MorEqn pour connaitre les morceaux d equation
//// Syntaxe a respecter pour jdd
//
// "nom_champ" Morceau_Equation { type "type_moreqn" numero "numero_moreqn" option "type_option" [ compo "num_compo" ]
//                                                source Champ_Post_ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" }
//               }
// "nom_champ"     fixe par utilisateur sera le nom du champ generique
// "type_moreqn"   designe le type de morceau d equation (actuellement disponible "operateur")
// "numero_moreqn" designe le numero du morceau
//                   ex : cas operateur : 0 (diffusion) 1 (convection) [ 2 (gradient) 3 (divergence) pour le cas des "flux_bords" ]
// "type_option"   option choisie ("stabilite" ou "flux_bords")
// "num_compo"           numero de la composante a postraiter (a considerer uniquement pour "flux_bords" si plusieurs composantes)

class Champ_Generique_Morceau_Equation : public Champ_Gen_de_Champs_Gen
{

  Declare_instanciable_sans_constructeur(Champ_Generique_Morceau_Equation);

public:

  Champ_Generique_Morceau_Equation();
  void set_param(Param& param);
  virtual const Noms get_property(const Motcle& query) const;
  virtual Entity  get_localisation(const int index = -1) const;
  const   Motcle             get_directive_pour_discr() const;
  virtual const Champ_base&  get_champ(Champ& espace_stockage) const;

  const MorEqn& morceau() const;
  MorEqn& morceau();
  void completer(const Postraitement_base& post);
  void nommer_source();

protected:

  int numero_morceau_;              //numero du morceau d equation
  Nom type_morceau_;                //type du morceau d equation (ex : Operateur)
  Nom option_;                      //Pour selectionner la quantite a postraiter
  int compo_;                            //Pour identifier la composante a recuperer pour l option flux_bords
  REF(Equation_base) ref_eq_;       //REF vers l equation qui porte le morceau
  Motcle  localisation_;            //localisation correspondant au support du champ postraite
};

#endif

