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
// File:        Operateur_Diff_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Operateur_Diff_base.h>
#include <Milieu_base.h>
#include <Champ_base.h>

Implemente_base(Operateur_Diff_base,"Operateur_Diff_base",Operateur_base);


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
Sortie& Operateur_Diff_base::printOn(Sortie& os) const
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
Entree& Operateur_Diff_base::readOn(Entree& is)
{
  return is;
}

// Description:
//  Associe la vraie diffusivite en m^2/s (en QC par exemple, l'operateur
//  est applique a rho*u, et on associe alors la viscosite dynamique a
//  l'operateur au lieu de la viscosite dynamique. Dans ce cas, il
//  faut associer la vraie diffusivite ici pour le calcul du pas de
//  temps de stabilite.
void Operateur_Diff_base::associer_diffusivite_pour_pas_de_temps(
  const Champ_base& diffu)
{
  if (je_suis_maitre())
    {
      Cerr << "Operateur_Diff_base::associer_diffusivite_pour_pas_de_temps\n";
      Cerr << " field name : " << diffu.le_nom();
      Cerr << " unit : "        << diffu.unite() << finl;
    }
  diffusivite_pour_pas_de_temps_ = diffu;
}

// Description:
//  Renvoie le champ_don correspondant a la vraie diffusivite du milieu
//  qui sert pour le calcul du pas de temps. Si l'operateur s'applique a rho*v
//  par exemple (cas du QC par exemple), la diffusivite associee est la
//  viscosite dynamique et non la viscosite cinematique. Il faut utiliser
//  la viscosite cinematique pour le calcul du pas de temps...
//  La vraie diffusivite (en m^2/s) est determinee comme suit:
//  * si diffusivite_pour_pas_de_temps_ a ete initialise => on l'utilise
//     (systeme mis en place pour le front-tracking, car le milieu
//      est invalide)
//  * sinon on utilise diffusivite()
const Champ_base& Operateur_Diff_base::diffusivite_pour_pas_de_temps() const
{
  assert( diffusivite_pour_pas_de_temps_.non_nul() );
  return diffusivite_pour_pas_de_temps_.valeur();
}
