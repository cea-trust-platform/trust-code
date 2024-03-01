/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Frontiere_dis_base.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Conds_lim.h>
#include <Domaine.h>

Implemente_instanciable(Conds_lim,"Conds_lim",VECT(Cond_lim));
// XD condlimlu objet_lecture nul 0 Boundary condition specified.
// XD  attr bord chaine bord 0 Name of the edge where the boundary condition applies.
// XD  attr cl condlim_base cl 0 Boundary condition at the boundary called bord (edge).

// XD condlims listobj condlims -1 condlimlu 0 Boundary conditions.

Sortie& Conds_lim::printOn(Sortie& s ) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Conds_lim::readOn(Entree& s ) { return s ; }

/*! @brief Appel Cond_lim::completer() sur chaque condition aux llimites du vecteur.
 *
 */
void Conds_lim::completer(const Domaine_dis& z)
{
  if (!size()) return; //rien a faire
  // Completer les CL.
  for (auto& itr : *this) itr->completer();

  // WEC : Fixer le nombre de valeurs temporelles des champ_front
  // Cela pourrait etre fait dans Cond_lim_base::completer(),
  // mais il faudrait retoucher a plein de classes derivees...
  int nb_cases = (*this)[0]->domaine_Cl_dis().equation().schema_temps().nb_valeurs_temporelles();

  for (auto& itr : *this) itr->fixer_nb_valeurs_temporelles(nb_cases);

  Domaine& dom=(*this)[0].frontiere_dis().frontiere().domaine();
  dom.construire_renum_som_perio(*this, z);
}
