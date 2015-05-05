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
// File:        Neumann.h
// Directory:   $TRUST_ROOT/src/Kernel/Cond_Lim
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Neumann_included
#define Neumann_included


#include <Cond_lim_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Neumann
//    Cette classe est la classe de base de la hierarchie des conditions
//    aux limites de type Neumann.
//    Une condition aux limites de type Neumann impose la valeur de la derivee
//    d'un champ inconnue sur une frontiere, ce qui correspond a:
//      - flux impose pour l'equation de transport d'un scalaire
//      - contrainte imposee pour l'equation de quantite de mouvement
// .SECTION voir aussi
//     Cond_lim_base Neumann_homogene
//////////////////////////////////////////////////////////////////////////////
class Neumann : public Cond_lim_base
{

  Declare_base(Neumann);

public:

  virtual double flux_impose(int i) const;
  virtual double flux_impose(int i,int j) const;

};

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Neumann_homogene
//    Cette classe est la classe de base de la hierarchie des conditions
//    aux limites de type Neumann homogenes.
//    Une condition aux limites de type Neumann_homogene impose une valeur nulle
//    de la derivee d'un champ inconnue sur une frontiere, ce qui correspond a:
//      - flux nul impose pour l'equation de transport d'un scalaire
//      - contrainte nulle imposee pour l'equation de quantite de mouvement
// .SECTION voir aussi
//     Cond_lim_base Neumann
//////////////////////////////////////////////////////////////////////////////
class Neumann_homogene : public Cond_lim_base
{

  Declare_base(Neumann_homogene);

public:

  inline double flux_impose(int i) const;
  inline double flux_impose(int i,int j) const;
};


// Description:
//    Renvoie la valeur du flux impose sur la i-eme composante
//    du champ representant le flux a la frontiere.
//    Toujours 0 pour Neumann homogene.
// Precondition:
// Parametre: int
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour: double
//    Signification: la valeur du flux imposee sur la composante specifiee
//    Contraintes: toujours egal a 0
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Neumann_homogene::flux_impose(int ) const
{
  return 0;
}


// Description:
//    Renvoie la valeur du flux impose sur la (i,j)-eme composante
//    du champ representant le flux a la frontiere.
//    Toujours 0 pour Neumann homogene.
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
//    Signification: la valeur imposee sur la composante specifiee
//    Contraintes: toujours egal a 0
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Neumann_homogene::flux_impose(int ,int ) const
{
  return 0;
}



#endif
