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
// File:        Espece.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Espece.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Espece,"Espece",Fluide_Quasi_Compressible);


// Description:
//    Ecrit les proprietes de l espece sur un flot de sortie.
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
Sortie& Espece::printOn(Sortie& os) const
{
  //os << que_suis_je() << finl;
  //Fluide_Incompressible::ecrire(os);
  return os;
}


// Description:
//   Lit les caracteristiques d une espece a partir d'un flot
//   d'entree.
//   Format:
//     Espece
//     {
//      Masse_molaire type_champ bloc de lecture de champ
//     }
// cf Milieu_base::readOn
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Espece::readOn(Entree& is)
{
  Milieu_base::readOn(is);
  return is;
}

void Espece::set_param(Param& param)
{
  param.ajouter("mu",&mu,Param::REQUIRED);
  param.ajouter("lambda",&lambda,Param::REQUIRED);
  param.ajouter("Cp",&Cp,Param::REQUIRED);
  param.ajouter("Masse_molaire",&Masse_mol_,Param::REQUIRED);
}

void Espece::verifier_coherence_champs(int& err,Nom& msg)
{
  Milieu_base::verifier_coherence_champs(err,msg);
}
