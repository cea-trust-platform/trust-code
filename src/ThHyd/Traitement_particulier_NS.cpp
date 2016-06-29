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
// File:        Traitement_particulier_NS.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS.h>

Implemente_deriv(Traitement_particulier_NS_base);
Implemente_instanciable(Traitement_particulier_NS,"Traitement_particulier_NS",DERIV(Traitement_particulier_NS_base));


// Description:
//    Impression de l'equation sur un flot de sortie.
//    Simple appel a Equation_base::printOn(Sortie&).
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Traitement_particulier_NS::printOn(Sortie& is) const
{
  return DERIV(Traitement_particulier_NS_base)::printOn(is);
}


// Description:
//    Lit les specifications de l'equation de Navier Stokes a
//    partir d'un flot d'entree.
//    Simple appel a Navier_Stokes_std::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: pas de modele de turbulence speficie
// Effets de bord:
// Postcondition: un modele de turbulence doit avoir ete specifie
Entree& Traitement_particulier_NS::readOn(Entree& is)
{
  return DERIV(Traitement_particulier_NS_base)::readOn(is);
}
