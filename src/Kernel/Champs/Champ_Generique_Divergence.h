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
// File:        Champ_Generique_Divergence.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Generique_Divergence_included
#define Champ_Generique_Divergence_included

#include <Champ_Generique_Operateur_base.h>
#include <Operateur_Div.h>

//
// .DESCRIPTION class Champ_Generique_Divergence
//
// Champ destine a post-traiter la divergence d un champ generique
// La classe porte un operateur statistique "divergence"

//// Syntaxe a respecter pour jdd
//
// "nom_champ" Divergence {
//                source type_champ_gen" { ...source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" } }
//               }
// "nom_champ" fixe par utilisateur sera le nom du champ generique
// "type_champ_gen" type d'un champ generique
//
//Ce type de champ implique que le champ source possede des conditions limites
//Son application est restreinte a certains champs discrets (vitesse)

class Champ_Generique_Divergence : public Champ_Generique_Operateur_base
{

  Declare_instanciable(Champ_Generique_Divergence);

public:

  const Noms get_property(const Motcle& query) const override;
  Entity  get_localisation(const int index = -1) const override;
  const   Motcle             get_directive_pour_discr() const override;
  const Champ_base&  get_champ(Champ& espace_stockage) const override;
  const Champ_base&  get_champ_without_evaluation(Champ& espace_stockage) const override;

  inline const Operateur_base& Operateur() const override;
  inline Operateur_base& Operateur() override;
  void completer(const Postraitement_base& post) override;
  void nommer_source() override;

protected:

  Operateur_Div Op_Div_;

};

inline const Operateur_base& Champ_Generique_Divergence::Operateur() const
{
  return Op_Div_.valeur();
}

inline Operateur_base& Champ_Generique_Divergence::Operateur()
{
  return Op_Div_.valeur();
}

#endif
