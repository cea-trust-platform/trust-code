/****************************************************************************
* Copyright (c) 2023, CEA
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
#include <Frontiere.h>
Implemente_base(Frontiere_dis_base,"Frontiere_dis_base",Objet_U);


/*! @brief Surcharge Objet_U::printOn(Sortie&) NE FAIT RIEN
 *
 *     A surcharger dans les classes derivees.
 *     Imprime la frontiere discretisee sur un flot de sortie
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Frontiere_dis_base::printOn(Sortie& os ) const
{
  return os;
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Lit une frontiere discretisee a partir d'un flot d'entree
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Frontiere_dis_base::readOn(Entree& is )
{
  return is;
}


/*! @brief Associe l'objet frontiere geometrique a la frontiere discretisee.
 *
 * @param (Frontiere& fr) la frontiere geometrique
 */
void Frontiere_dis_base::associer_frontiere(const Frontiere& fr)
{
  la_frontiere=fr;
}

/*! @brief Renvoie la frontiere geometrique associee.
 *
 * (version const)
 *
 * @return (Frontiere&) la frontiere geometrique associee
 */
const Frontiere& Frontiere_dis_base::frontiere() const
{
  return la_frontiere.valeur();
}

/*! @brief Renvoie la frontiere geometrique associee.
 *
 * @return (Frontiere&) la frontiere geometrique associee
 */
Frontiere& Frontiere_dis_base::frontiere()
{
  return la_frontiere.valeur();
}

/*! @brief Renvoie le nom de la frontiere geometrique.
 *
 * @return (Nom&) le nom de la frontiere geometrique
 */
const Nom& Frontiere_dis_base::le_nom() const
{
  return la_frontiere->le_nom();
}

void Frontiere_dis_base::associer_Zone_dis(const Zone_dis_base& z)
{
  le_dom_dis=z;
}

const Zone_dis_base& Frontiere_dis_base::zone_dis() const
{
  return le_dom_dis.valeur();
}
