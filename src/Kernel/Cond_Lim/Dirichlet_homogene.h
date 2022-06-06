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


#ifndef Dirichlet_homogene_included
#define Dirichlet_homogene_included

#include <Cond_lim_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Dirichlet_homogene
//    Cette classe est la classe de base de la hierarchie des conditions
//    aux limites de type Dirichlet homogene.
//    Une condition aux limites de type Dirichlet homogene impose une valeur
//    nulle au champ inconnue sur une frontiere, ce qui correspond a:
//     - vitesse imposee a zero pour l'equation de Navier-Stokes
//     - scalaire impose a zero pour l'equation de transport d'un scalaire
// .SECTION voir aussi
//     Cond_lim_base Dirichlet
//////////////////////////////////////////////////////////////////////////////
class Dirichlet_homogene  : public Cond_lim_base
{

  Declare_base(Dirichlet_homogene);

public:

  inline double val_imp(int i) const;
  inline double val_imp(int i,int j) const;

};


// Description:
//    Renvoie la valeur imposee sur la i-eme composante
//    du champ a la frontiere.
//    Toujours 0 pour Dirichlet homogene.
// Precondition:
// Parametre: int
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes: toujours egal a 0
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Dirichlet_homogene::val_imp(int ) const
{
  return 0;
}


// Description:
//    Renvoie la valeur imposee sur la (i,j)-eme composante
//    du champ a la frontiere.
//    Toujours 0 pour Dirichlet homogene.
// Precondition:
// Parametre: int
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Parametre: int
//    Signification: indice suivant la deuxieme dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes: toujours egal a 0
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Dirichlet_homogene::val_imp(int ,int ) const
{
  return 0;
}



#endif
