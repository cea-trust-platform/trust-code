/****************************************************************************
* Copyright (c) 2018, CEA
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
// File:        Option_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Option_CoviMAC.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Option_CoviMAC,"Option_CoviMAC",Interprete);
// XD option_vdf interprete option_vdf 1 Class of CoviMAC options.

int Option_CoviMAC::mpfa_stencil = 0;
double Option_CoviMAC::precision_weight = 1;
double Option_CoviMAC::stability_weight = 1e-3;

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
Sortie& Option_CoviMAC::printOn(Sortie& os) const
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
Entree& Option_CoviMAC::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction principale de l'interprete Option_CoviMAC
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
Entree& Option_CoviMAC::interpreter(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_non_std("mpfa_stencil",(this)); // XD_ADD_P void Use MPFA-like stencil for flux interpolation
  param.ajouter_non_std("precision_weight",(this)); // XD_ADD_P double Weight of the interpolation error minimization [1].
  param.ajouter_non_std("stability_weight",(this)); // XD_ADD_P double Weight of the off-diagonal L2 norm [1e-3].
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Option_CoviMAC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="mpfa_stencil") mpfa_stencil = 1;
  else if (mot=="precision_weight") is >> precision_weight;
  else if (mot=="stability_weight") is >> stability_weight;
  else
    {
      Cerr << mot << "is not a keyword understood by " << que_suis_je() << "in methode lire_motcle_non_standard"<< finl;
      return -1;

    }
  return 1;
}
