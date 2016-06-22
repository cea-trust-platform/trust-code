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
// File:        Champ_Generique_Predefini.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Generique_Predefini_included
#define Champ_Generique_Predefini_included

#include <Champ_Gen_de_Champs_Gen.h>


//
// .DESCRIPTION class Champ_Generique_Predefini
//

// Champ destine a encapsuler un champ generique dont l'expression est predefinie
// pour que les utilisateurs disposent de raccourci en terme de syntaxe
//
//// Syntaxe a respecter pour jdd
//
// "nom_champ" Predefini { Pb_champ "nom_pb" "nom_champ_a_creer" }
//
// "nom_champ" fixe par utilisateur sera le nom du champ generique construit (champ_)
// "nom_champ_a_creer" type de champ generique a creer (ex : energie_cinetique)


class Champ_Generique_Predefini : public Champ_Gen_de_Champs_Gen
{

  Declare_instanciable(Champ_Generique_Predefini);

public:

  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void completer(const Postraitement_base& post);
  const Champ_Generique_base&  get_source(int i) const;
  virtual const Noms get_property(const Motcle& query) const;
  virtual const Champ_base&  get_champ(Champ& espace_stockage) const;
  virtual void nommer(const Nom&);
  virtual const Nom& get_nom_post() const;
  void nommer_source();
  Nom construit_expression();

protected:

  Nom type_champ_;         //Type de champ predefini a lire (ex : energie_cinetique)
  Nom nom_pb_;
  Champ_Generique champ_;  //Le champ generique predefini par type_champ_

};

#endif

