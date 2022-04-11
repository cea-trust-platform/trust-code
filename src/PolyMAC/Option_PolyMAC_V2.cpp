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
// File:        Option_PolyMAC_V2.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Option_PolyMAC_V2.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Option_PolyMAC_V2,"Option_PolyMAC_V2|Option_CoviMAC",Interprete);
// XD Option_PolyMAC_V2 interprete Option_PolyMAC_V2 1 Class of PolyMAC_V2 options.
// XD attr interp_ve1 entier interp_ve1 1 Flag to enable a first order velocity face-to-element interpolation (the default value is 0 which means a second order interpolation)

int Option_PolyMAC_V2::interp_ve1 = 0;

// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
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
Sortie& Option_PolyMAC_V2::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a: Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Option_PolyMAC_V2::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction principale de l'interprete Option_PolyMAC_V2
//    Lit la dimension d'espace du probleme.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Option_PolyMAC_V2::interpreter(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_non_std("interp_ve1",(this));     //chaine Use first-order face->cell velocity interpolation. By default, it is not activated
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Option_PolyMAC_V2::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="interp_ve1") interp_ve1 = 1;
  else return -1;
  return 1;
}
