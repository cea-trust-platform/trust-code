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

#include <Bords.h>

Implemente_instanciable(Bords, "Bords", LIST(Bord));

Sortie& Bords::printOn(Sortie& os) const { return LIST(Bord)::printOn(os); }

Entree& Bords::readOn(Entree& is) { return LIST(Bord)::readOn(is); }

/*! @brief Associe un domaine a tous les bords de la liste.
 *
 * @param (Domaine& un_domaine) le domaine a associer aux bords de la liste
 */
void Bords::associer_domaine(const Domaine& un_domaine)
{
  for (auto &itr : *this) itr.associer_domaine(un_domaine);
}

/*! @brief Renvoie le nombre total de faces de tous les bords de la liste
 *
 * @return (int) le nombre total de faces de tous les bords de la liste
 */
int Bords::nb_faces() const
{
  int nombre = 0;

  for (const auto &itr : *this) nombre += itr.nb_faces();

  return nombre;
}

/*! @brief Renvoie le nombre total de faces du type specifie, pour tous les bords de la liste.
 *
 * @param (Type_Face type) le type des faces a comptabiliser
 * @return (int) le nombre total de faces du type specifie, pour tous les bords de la liste
 */
int Bords::nb_faces(Type_Face type) const
{
  int nombre = 0;

  for (const auto &itr : *this)
    if (type == itr.faces().type_face()) nombre += itr.nb_faces();

  return nombre;
}
