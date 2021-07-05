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
// File:        Fluide_Weakly_Compressible.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Weakly_Compressible/Milieu
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Weakly_Compressible.h>
#include <Zone_Cl_dis.h>
#include <Neumann_sortie_libre.h>

Implemente_instanciable_sans_constructeur(Fluide_Weakly_Compressible,"Fluide_Weakly_Compressible",Fluide_Dilatable);

Fluide_Weakly_Compressible::Fluide_Weakly_Compressible() {}

// Description:
//    Ecrit les proprietes du fluide sur un flot de sortie.
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
Sortie& Fluide_Weakly_Compressible::printOn(Sortie& os) const
{
  os << que_suis_je() << finl;
  Fluide_Dilatable::ecrire(os);
  return os;
}


// Description:
//   Lit les caracteristiques du fluide a partir d'un flot
//   d'entree.
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
Entree& Fluide_Weakly_Compressible::readOn(Entree& is)
{
  Fluide_Dilatable::readOn(is);
  return is;
}

void Fluide_Weakly_Compressible::checkTraitementPth(const Zone_Cl_dis& zone_cl)
{
  /*
   * traitement_PTh=0 => pression laissee cste.
   * traitement_PTh=1 => pression calculee pour conserver la masse
   */
  int pression_imposee=0;
  int size=zone_cl.les_conditions_limites().size();
  assert(size!=0);
  for (int n=0; n<size; n++)
    {
      const Cond_lim& la_cl = zone_cl.les_conditions_limites(n);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
        {
          pression_imposee=1;
        }
    }
  if (pression_imposee && traitement_PTh!=0)
    {
      if (Process::je_suis_maitre())
        {
          Cerr << "The Traitement_Pth option selected is not coherent with the boundaries conditions." << finl;
          Cerr << "Traitement_Pth constant must be used for the case of free outlet." << finl;
        }
      Process::exit();
    }
  if (!pression_imposee && traitement_PTh!=1)
    {
      if (Process::je_suis_maitre())
        {
          Cerr << "The Traitement_Pth option selected is not coherent with the boundaries conditions." << finl;
          Cerr << "Traitement_Pth conservation_masse must be used for the case without free outlet." << finl;
        }
      Process::exit();
    }
}

void Fluide_Weakly_Compressible::completer(const Probleme_base&)
{
  // TODO ...
}

void Fluide_Weakly_Compressible::prepare_pressure_edo()
{
  /* Do nothing */
}

void Fluide_Weakly_Compressible::calculer_rho_face(const DoubleTab& tab_rho)
{
  /* Do nothing */
}

void Fluide_Weakly_Compressible::write_mean_edo(double t)
{
  /* Do nothing */
}
