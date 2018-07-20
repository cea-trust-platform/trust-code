/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Pb_Conduction_Milieu_Variable.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_Conduction_Milieu_Variable.h>

Implemente_instanciable(Pb_Conduction_Milieu_Variable,"Pb_Conduction_Milieu_Variable",Probleme_base);
// XD pb_conduction_milieu_variable Pb_base pb_conduction_milieu_variable -1 Resolution of the heat equation.
// XD   attr conduction_milieu_variable conduction_milieu_variable conduction_milieu_variable 1 Heat equation.

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Pb_Conduction_Milieu_Variable::printOn(Sortie& s ) const
{
  return s;
}


// Description:
//    Simple appel a: Probleme_base::readOn(Entree&)
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
Entree& Pb_Conduction_Milieu_Variable::readOn(Entree& is )
{
  return Probleme_base::readOn(is);
}


// Description:
//    Renvoie le nombre d'equations du probleme.
//    Toujours egal a 1 pour un probleme de conduction standart.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'equations du probleme
//    Contraintes: toujours egal a 1
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Pb_Conduction_Milieu_Variable::nombre_d_equations() const
{
  return 1;
}

// Description:
//    Renvoie l'equation de type Conduction_Milieu_Variable
//     si i = 0,
//    Provoque une erreur sinon car le probleme
//    n'a qu'une seule equation.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: i doit etre egal a zero
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation de type Conduction_Milieu_Variable
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Equation_base& Pb_Conduction_Milieu_Variable::equation(int i) const
{
  assert (i==0);
  return eq_conduction;
}

// Description:
//    Renvoie l'equation de type Conduction_Milieu_Variable
//     si i = 0,
//    Provoque une erreur sinon car le probleme
//    n'a qu'une seule equation.
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: i doit etre egal a zero
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation de type Conduction_Milieu_Variable
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Equation_base& Pb_Conduction_Milieu_Variable::equation(int i)
{
  assert (i==0);
  return eq_conduction;
}




