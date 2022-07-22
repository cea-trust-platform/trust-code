/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Op_Conv_negligeable.h>
#include <Champ_base.h>

Implemente_instanciable(Op_Conv_negligeable,"Op_Conv_negligeable",Operateur_Conv_base);


/*! @brief Imprime la vitesse sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Op_Conv_negligeable::printOn(Sortie& os) const
{
  return os << la_vitesse;
}


/*! @brief Lit la vitesse a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Op_Conv_negligeable::readOn(Entree& is)
{
  return is >> la_vitesse;
}


/*! @brief Associe la vitesse a l'operateur.
 *
 * @param (Champ_Inc& ch) le champ inconnue representant la vitesse
 */
void Op_Conv_negligeable::associer_vitesse(const Champ_base& ch)
{
  la_vitesse = ch;
}


/*! @brief Renvoie le champ inconnue representant la vitesse
 *
 * @return (Champ_Inc&) le champ inconnue representant la vitesse
 */
const Champ_base& Op_Conv_negligeable::vitesse() const
{
  return la_vitesse.valeur();
}

void Op_Conv_negligeable::ajouter_flux(const DoubleTab& inconnue, DoubleTab& contribution) const
{
  // nothing to do
}

void Op_Conv_negligeable::calculer_flux(const DoubleTab& inconnue, DoubleTab& flux) const
{
  flux = 0.0;
}

