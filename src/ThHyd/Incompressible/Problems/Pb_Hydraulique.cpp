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

#include <Pb_Hydraulique.h>
#include <Fluide_Ostwald.h>

Implemente_instanciable(Pb_Hydraulique,"Pb_Hydraulique",Pb_Fluide_base);


// Description:
//    Simple appel a: Pb_Fluide_base::printOn(Sortie&)
//    Ecrit le probleme sur un flot de sortie.
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
Sortie& Pb_Hydraulique::printOn(Sortie& os) const
{
  return Pb_Fluide_base::printOn(os);
}


// Description:
//    Simple appel a: Pb_Fluide_base::readOn(Entree&)
//    Lit le probleme a partir d'un flot d'entree.
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
Entree& Pb_Hydraulique::readOn(Entree& is)
{
  return Pb_Fluide_base::readOn(is);
}

// Description:
//    Renvoie le nombre d'equation,
//    Renvoie 1 car il y a seulement 1 equation a un probleme
//    hydraulique standard: l'equation de Navier Stokes
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'equation
//    Contraintes: toujours 1 car il n'y qu'une seule equation (Navier Stokes)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Pb_Hydraulique::nombre_d_equations() const
{
  return 1;
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_std
//    si i=0, sort (exit) sinon.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: i doit etre egal a 0, le probleme n'a qu'une equation
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation d'hydraulique de type Navier_Stokes_std
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Equation_base& Pb_Hydraulique::equation(int i) const
{
  if (i!=0)
    {
      Cerr << "\nError in Pb_Hydraulique::equation() : The problem has only one equation !" << finl;
      Process::exit();
    }
  return eq_hydraulique;
}

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_std
//    si i=0, sort (exit) sinon.
// Precondition:
// Parametre: int i
//    Signification: l'index de l'equation a renvoyer
//    Valeurs par defaut:
//    Contraintes: i doit etre egal a 0, le probleme n'a qu'une equation
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation d'hydraulique de type Navier_Stokes_std
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
Equation_base& Pb_Hydraulique::equation(int i)
{
  if (i!=0)
    {
      Cerr << "\nPb_Hydraulique::equation() : The problem has only one equation !" << finl;
      Process::exit();
    }
  return eq_hydraulique;
}



// Description:
//    Associe le milieu au probleme.
//    Le milieu doit etre de type fluide incompressible.
// Precondition:
// Parametre: Milieu_base& mil
//    Signification: le milieu physique a associer au probleme
//    Valeurs par defaut:
//    Contraintes: doit etre de type fluide incompressible
//                 reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: le milieu n'est pas du type Fluide_base
// Effets de bord:
// Postcondition:
void Pb_Hydraulique::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Fluide_base,mil))
    eq_hydraulique.associer_milieu_base(mil);
  else if (sub_type(Fluide_Ostwald,mil))
    {
      Cerr << "Je suis non newto" << finl;
      eq_hydraulique.associer_milieu_base(mil);
    }
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a "
           << finl;
      Cerr << "un probleme de type Pb_Hydraulique " << finl;
      exit();
    }
}

