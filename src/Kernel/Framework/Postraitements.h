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

#ifndef Postraitements_included
#define Postraitements_included

#include <List.h>

///////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Postraitements
//     Cette classe represente une liste de postraitements
// .SECTION voir aussi
//     Postraitement
///////////////////////////////////////////////////////////////////

class Probleme_base;
class Postraitement_base;
class Entree;
class Motcle;
Declare_deriv(Postraitement_base);
Declare_liste(DERIV(Postraitement_base));

class Postraitements : public LIST(DERIV(Postraitement_base))
{
  Declare_instanciable(Postraitements);

public:
  int lire_postraitements(Entree& is, const Motcle& motlu,
                          const Probleme_base& mon_pb);
  void postraiter();
  void traiter_postraitement();
  void mettre_a_jour(double temps);
  void init();
  void finir();
  int sauvegarder(Sortie& os) const override;
  int reprendre(Entree& is) override;
  void completer();
  void completer_sondes();
};

#endif
