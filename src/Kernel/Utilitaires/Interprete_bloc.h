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
// File:        Interprete_bloc.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Interprete_bloc_included
#define Interprete_bloc_included

#include <Interprete.h>
#include <Noms.h>
#include <liste.h>

class Interprete_bloc;
Declare_ref(Interprete_bloc);

/////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//  Interprete un bloc d'instructions dans le jeu de donnees.
//  Voir Interprete_bloc::interpreter()
/////////////////////////////////////////////////////////////
class Interprete_bloc : public liste
{
  Declare_instanciable(Interprete_bloc);
public:
  // Indique si le bloc se termine par une accolade, un EOF ou le mot FIN
  enum Bloc_Type { ACCOLADE, BLOC_EOF, FIN };
  Entree& interpreter_bloc(Entree& is,
                           Bloc_Type bloc_type,
                           int verifier_sans_interpreter);
  Objet_U& ajouter(const Nom& nom, DerObjU& object_to_add);
  Objet_U& objet_local(const Nom& nom);
  int    objet_local_existant(const Nom& nom);

  static Interprete_bloc& interprete_courant();
  static Objet_U&          objet_global(const Nom& nom);
  static int            objet_global_existant(const Nom& nom);
protected:
  // Noms des objets lus dans le bloc:
  Noms les_noms_;
  // Reference a l'interprete courant avant la creation de celui-ci
  REF(Interprete_bloc) pere_;
};
#endif
