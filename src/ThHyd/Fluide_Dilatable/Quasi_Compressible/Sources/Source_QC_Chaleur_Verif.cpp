/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Source_QC_Chaleur_Verif.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Sources
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_QC_Chaleur_Verif.h>
#include <Equation_base.h>

Implemente_instanciable_sans_constructeur(Source_QC_Chaleur_Verif,"Source_QC_Chaleur_Verif_QC_VDF_P0_VDF",Source_Chaleur_Fluide_Dilatable_base);
Source_QC_Chaleur_Verif::Source_QC_Chaleur_Verif() : mode(0) { }

// Description:
//    Imprime la source sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Source_QC_Chaleur_Verif::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

// Description:
//    Lecture de la source sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Source_QC_Chaleur_Verif::readOn(Entree& is)
{
  is >> mode;
  return is;
}

DoubleTab& Source_QC_Chaleur_Verif::ajouter(DoubleTab& resu) const
{
  resu = 0.;
  return resu;
}

Implemente_instanciable(Source_QC_Chaleur_Verif_VEF,"Source_QC_Chaleur_Verif_QC_VEF",Source_QC_Chaleur_Verif);
Entree& Source_QC_Chaleur_Verif_VEF::readOn(Entree& is) { return Source_QC_Chaleur_Verif::readOn(is); }
Sortie& Source_QC_Chaleur_Verif_VEF::printOn(Sortie& is) const { return Source_QC_Chaleur_Verif::printOn(is); }

Implemente_instanciable(Source_QC_Chaleur_Verif_VEF_P1NC,"Source_QC_Chaleur_Verif_QC_VEF_P1NC",Source_QC_Chaleur_Verif);
Entree& Source_QC_Chaleur_Verif_VEF_P1NC::readOn(Entree& is) { return Source_QC_Chaleur_Verif::readOn(is); }
Sortie& Source_QC_Chaleur_Verif_VEF_P1NC::printOn(Sortie& is) const { return Source_QC_Chaleur_Verif::printOn(is); }
