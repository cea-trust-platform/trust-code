/****************************************************************************
* Copyright (c) 2019, CEA
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


#include <Conds_lim.h>
#include <Frontiere_dis_base.h>
#include <Domaine.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>

Implemente_vect(Cond_lim);
Implemente_instanciable(Conds_lim,"Conds_lim",VECT(Cond_lim));


// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime seulement le nom du tableau  des conditions aux limites.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Conds_lim::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree& s
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
Entree& Conds_lim::readOn(Entree& s )
{
  return s ;
}


// Description:
//    Appel Cond_lim::completer() sur chaque condition aux llimites
//    du vecteur.
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
void Conds_lim::completer(const Zone_dis& z)
{
  if (!size()) return; //rien a faire
  // Completer les CL.
  for (int i=0; i<size(); i++)
    (*this)[i].completer();

  // WEC : Fixer le nombre de valeurs temporelles des champ_front
  // Cela pourrait etre fait dans Cond_lim_base::completer(),
  // mais il faudrait retoucher a plein de classes derivees...
  int nb_cases = (*this)[0]->zone_Cl_dis().equation().schema_temps().nb_valeurs_temporelles();
  for (int i=0; i<size(); i++)
    (*this)[i]->fixer_nb_valeurs_temporelles(nb_cases);

  Domaine& dom=(*this)[0].frontiere_dis().frontiere().zone().domaine();
  dom.construire_renum_som_perio(*this, z);
}
