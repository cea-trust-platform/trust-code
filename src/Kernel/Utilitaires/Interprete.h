/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Interprete.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Interprete_included
#define Interprete_included

#include <Ref.h>

class Interprete;
Declare_ref(Interprete);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Classe de base des objets "interprete".
//   Ces objets definissent des actions a realiser lorsqu'on les
//   rencontre dans le jeu de donnees. L'action est declanchee par
//   un appel a la methode interpreter().
//   L'interprete peut ensuite lire des parametres supplementaires
//   dans l'entree pour realiser sa tache. En general, l'interprete
//   agit sur d'autres objets declares dans le jeu de donnees. Il peut
//   y acceder par les methodes objet() et est_un_objet_existant().
//   Voir par exemple la classe Lire ou la classe Associer
//   Voir aussi la classe Interprete_bloc qui lit une serie d'instructions
//   a executer dans le jeu de donnees.
// .SECTION voir aussi Interprete_bloc
//////////////////////////////////////////////////////////////////////////////
class Interprete : public Objet_U
{
  Declare_instanciable(Interprete);
public:
  virtual Entree& interpreter(Entree& is)
  {
    exit();
    return is;
  }

  static Objet_U& objet(const Nom&);
  static int objet_existant(const Nom&);
};
#endif
