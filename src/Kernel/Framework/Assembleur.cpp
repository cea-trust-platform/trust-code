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
// File:        Assembleur.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur.h>
#include <Assembleur_base.h>

Implemente_deriv(Assembleur_base);
Implemente_instanciable(Assembleur,"Assembleur",DERIV(Assembleur_base));


// Description:
//    Simple appel a Assembleur_base::printOn(Sortie&)
//    Imprime l'equation et ses composants sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie de sauvegarde
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Assembleur::printOn(Sortie& os) const
{
  return DERIV(Assembleur_base)::printOn(os);
}


// Description:
//    Simple appel a Assembleur_base::readOn(Entree&)
//    Imprime l'equation et ses composants sur un flot de sortie.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture d'une equation
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot d'entree est modifie
// Postcondition: l'equation est construite avec les elements lus.
Entree& Assembleur::readOn(Entree& is)
{
  return DERIV(Assembleur_base)::readOn(is);
}


// Description:
//    Appel a l'objet sous-jacent.
//    Permet de completer le systeme.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Assembleur::completer(const Equation_base& eqn)
{
  valeur().completer(eqn);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Permet d'assembler le systeme.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Assembleur::assembler(Matrice& mat)
{
  return valeur().assembler(mat);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Permet d'assembler le systeme.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Assembleur::assembler_QC(const DoubleTab& rho, Matrice& mat)
{
  return valeur().assembler_QC(rho,mat);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Permet d'associer l'objet a la zone discretisee
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Assembleur::associer_zone_dis_base(const Zone_dis_base& zdis)
{
  valeur().associer_zone_dis_base(zdis);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Permet d'associer l'objet a la Zone_Cl_dis
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Assembleur::associer_zone_cl_dis_base(const Zone_Cl_dis_base& zcldis)
{
  valeur().associer_zone_cl_dis_base(zcldis);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie la zone discretisee associee a l'objet.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: ne modifie pas l'objet
const Zone_dis_base& Assembleur::zone_dis_base() const
{
  return valeur().zone_dis_base();
}

// Description:
//    Appel a l'objet sous-jacent.
//    Renvoie la Zone_Cl_dis associee a l'objet.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: ne modifie pas l'objet
const Zone_Cl_dis_base& Assembleur::zone_Cl_dis_base() const
{
  return valeur().zone_Cl_dis_base();
}

// Description:
//    Appel a l'objet sous-jacent.
//    Permet de modifier le second membre du systeme
//    Modifie egalement la solution (dans le cas du systeme en pression)
//    en fonction des conditions aux limites
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Assembleur::modifier_secmem(DoubleTab& secmem)
{
  return valeur().modifier_secmem(secmem);
}
// Description:
//    Appel a l'objet sous-jacent.
//    Permet de modifier la pression
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Assembleur::modifier_solution(DoubleTab& sol)
{
  return valeur().modifier_solution(sol);
}

