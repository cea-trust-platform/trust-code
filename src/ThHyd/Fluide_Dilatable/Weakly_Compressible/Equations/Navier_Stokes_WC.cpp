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
// File:        Navier_Stokes_WC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Weakly_Compressible/Equations
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////

#include <Navier_Stokes_WC.h>
#include <Fluide_Weakly_Compressible.h>
#include <Discretisation_base.h>

Implemente_instanciable(Navier_Stokes_WC,"Navier_Stokes_WC",Navier_Stokes_Fluide_Dilatable_base);

// Description:
//    Simple appel a: Equation_base::printOn(Sortie&)
//    Ecrit l'equation sur un flot de sortie.
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
// Postcondition: la methode ne modifie pas l'objet la methode ne modifie pas l'objet
Sortie& Navier_Stokes_WC::printOn(Sortie& is) const
{
  return Navier_Stokes_Fluide_Dilatable_base::printOn(is);
}

// Description:
//    Appel Equation_base::readOn(Entree& is)
//    En sortie verifie que l'on a bien lu:
//        - le terme diffusif,
//        - le terme convectif,
//        - le solveur en pression
// Precondition: l'equation doit avoir un milieu fluide associe
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: terme diffusif non specifie dans jeu de donnees, specifier
//            un type negligeable pour l'operateur si il est a negliger
// Exception: terme convectif non specifie dans jeu de donnees, specifier
//            un type negligeable pour l'operateur si il est a negliger
// Exception: solveur pression non defini dans jeu de donnees
// Effets de bord:
// Postcondition:
Entree& Navier_Stokes_WC::readOn(Entree& is)
{
  Navier_Stokes_Fluide_Dilatable_base::readOn(is);
  assert(le_fluide.non_nul());
  if (!sub_type(Fluide_Weakly_Compressible,le_fluide.valeur()))
    {
      Cerr<<"ERROR : the Navier_Stokes_WC equation can be associated only to a weakly compressible fluid."<<finl;
      Process::exit();
    }
  return is;
}

// Description:
//    Complete l'equation base,
//    associe la pression a l'equation,
//    complete la divergence, le gradient et le solveur pression.
//    Ajout de 2 termes sources: l'un representant la force centrifuge
//    dans le cas axi-symetrique,l'autre intervenant dans la resolution
//    en 2D axisymetrique
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Navier_Stokes_WC::completer()
{
  Cerr<<"Navier_Stokes_WC::completer"<<finl;
  if (le_fluide->a_gravite())
    {
      Cerr << "Gravity = " << le_fluide->gravite() << finl;
      Cerr << "Creation of the buoyancy source term for the Navier_Stokes_WC equation : " << finl;
      Source t;
      Source& so=les_sources.add(t);
      Nom type_so = "Source_Gravite_Weakly_Compressible_";
      Nom disc = discretisation().que_suis_je();
      if (disc=="VEFPreP1B") disc = "VEF";
      type_so += disc;
      so.typer_direct(type_so);
      so->associer_eqn(*this);
      Cerr << so->que_suis_je() << finl;
    }
  Navier_Stokes_Fluide_Dilatable_base::completer();
}

int Navier_Stokes_WC::impr(Sortie& os) const
{
  return Navier_Stokes_Fluide_Dilatable_base::impr(os);
}
