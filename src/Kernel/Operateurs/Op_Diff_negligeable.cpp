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
// File:        Op_Diff_negligeable.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_negligeable.h>
#include <Champ_base.h>
#include <Discretisation_base.h>
#include <Champ.h>

Implemente_instanciable(Op_Diff_negligeable,"Op_Diff_negligeable",Operateur_Diff_base);
Implemente_instanciable(Op_Dift_negligeable,"Op_Dift_negligeable",Op_Diff_negligeable);

Sortie& Op_Dift_negligeable::printOn(Sortie& os) const
{
  return  Op_Diff_negligeable::printOn(os);
}


Entree& Op_Dift_negligeable::readOn(Entree& is)
{
  return  Op_Diff_negligeable::readOn(is);
}

// Description:
//    Imprime la diffusivite sur un flot de sortie.
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
Sortie& Op_Diff_negligeable::printOn(Sortie& os) const
{
  return os << la_diffusivite;
}


// Description:
//    Lit la diffusivite a partir d'un flot d'entree.
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
Entree& Op_Diff_negligeable::readOn(Entree& is)
{
  return is >> la_diffusivite;
}


// Description:
//    Associe la diffusivite a l'operateur.
// Precondition:
// Parametre: Champ_Don& ch
//    Signification: le champ representant la diffusivite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Op_Diff_negligeable::associer_diffusivite(const Champ_base& ch)
{
  la_diffusivite=ch;
}


// Description:
//    Renvoie le champ representant la diffusivite.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don_base&
//    Signification: le champ representant la diffusivite
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_base& Op_Diff_negligeable::diffusivite() const
{
  return la_diffusivite.valeur();
}

void Op_Diff_negligeable::calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const
{
  if (Motcle(option)=="stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage->valeurs();
      es_valeurs = calculer_dt_stab();
    }
  else
    Operateur_base::calculer_pour_post(espace_stockage,option,comp);
}

Motcle Op_Diff_negligeable::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="stabilite")
    loc = "elem";
  else if (Motcle(option)=="flux_bords")
    loc = "face";

  return loc;
}

void Op_Diff_negligeable::ajouter_flux(const DoubleTab& inconnue, DoubleTab& contribution) const
{
  // nothing to do
}

void Op_Diff_negligeable::calculer_flux(const DoubleTab& inconnue, DoubleTab& flux) const
{
  flux = 0.0;
}

