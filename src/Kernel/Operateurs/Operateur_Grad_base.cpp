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
// File:        Operateur_Grad_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Operateur_Grad_base.h>
#include <DoubleTrav.h>

Implemente_base(Operateur_Grad_base,"Operateur_Grad_base",Operateur_base);


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Operateur_Grad_base::printOn(Sortie& os) const
{
  return os;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Operateur_Grad_base::readOn(Entree& is)
{
  return is;
}

// Description: Calcul sans les conditions aux limites ?
DoubleVect& Operateur_Grad_base::multvect(const DoubleTab& x, DoubleTab& b) const
{
  //
  //Cerr << que_suis_je()<<"::multvect n'est surcharge!!" << finl;
  // copie de X en mettant 0 dans le vecteur
  //  (on recupere un resultat tenant compte uniquement des conditions aux limites)
#if 0
  // Tentative de codage plus efficace: ne marche pas !
  // ecart sur SGE_3D_VEF
  DoubleTrav x0(x); // copie de la structure, initialisee a zero
  calculer(x0, b);
  operator_negate(b);
  ajouter(x, b);
#endif
  DoubleTrav x0(x);
  DoubleTrav b0(b);

  calculer(x0, b0);
  calculer(x,b);
  b-=b0;

  // On a soustrait au resultat la contribution des conditions aux limites.
  return b;
}
