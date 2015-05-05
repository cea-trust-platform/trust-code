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
// File:        Champ_front_softanalytique.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Champs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_softanalytique_included
#define Champ_front_softanalytique_included

#include <Ch_front_var_stationnaire.h>
#include <Vect_Parser_U.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_softanalytique
//     Classe derivee de Champ_front_var qui represente les
//     champs aux frontieres analytiques en espace et constants dans
//     le temps
//   Nouveau champ frontiere
//   il permet de mettre en champ frontiere un parametre qui depend d'une
//   fonction. Cette fonction est rentree dans le jeu de donnees sous
//   la forme d'une chaine de caracteres. Pas besoin de recompilation
//   comme avec la classe Champ_front_analytique qui utilise une
//   fonction codee en dur.
// .SECTION voir aussi
//     Champ_front_base Champ_front_var
//////////////////////////////////////////////////////////////////////////////
class Champ_front_softanalytique : public Ch_front_var_stationnaire
{
  Declare_instanciable(Champ_front_softanalytique);

public:
  virtual int initialiser(double temps, const Champ_Inc_base& inco);
  void valeur_a(DoubleVect& position, DoubleVect& valeur);

private :
  VECT(Parser_U) fxyz;

};

#endif

