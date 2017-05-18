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
// File:        Champ_Post_Operateur_Eqn.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

// .NOM Champ_Post_Operateur_Eqn
// .ENTETE TRUST Kernel/Champs
// .LIBRAIRIE
// .FILE Champ_Post_Operateur_Eqn.h
// .FILE Champ_Post_Operateur_Eqn.cpp

#ifndef Champ_Post_Operateur_Eqn_included
#define Champ_Post_Operateur_Eqn_included

#include <Champ_Generique_Operateur_base.h>
#include <Operateur.h>
#include <Ref_Equation_base.h>
//
// .DESCRIPTION class Champ_Post_Operateur_Eqn
// Champ destine a post-traiter le gradient d un champ generique
// La classe porte un operateur statistique "gradient"

//// Syntaxe a respecter pour jdd
//
// "nom_champ" Champ_Post_Statistiques_Eqn {
//		source Champ_Post...{ ...source Champ_Post_ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" } }
//	       }
// "nom_champ" fixe par utilisateur sera le nom du champ generique
//Ce type de champ implique que le champ source possede des conditions limites
//Son application est restreinte a certains champs discrets (pression VDF et VEF ou temperature en VEF)

class Champ_Post_Operateur_Eqn : public Champ_Generique_Operateur_base
{

  Declare_instanciable(Champ_Post_Operateur_Eqn);

public:

  virtual const Noms get_property(const Motcle& query) const;
  virtual Entity  get_localisation(const int index = -1) const;
  virtual const Champ_base&  get_champ(Champ& espace_stockage) const;
  virtual const Champ_base&  get_champ_without_evaluation(Champ& espace_stockage) const;


  const Operateur_base& Operateur() const;
  Operateur_base& Operateur();
  void completer(const Postraitement_base& post);
  void nommer_source();
  //virtual Entree &   lire(const Motcle & motcle, Entree & is);
  void set_param(Param& param);


protected:
  int numero_source_,numero_op_;
  REF(Equation_base) ref_eq_;
  int sans_solveur_masse_;
  Entity localisation_inco_;
};

#endif
