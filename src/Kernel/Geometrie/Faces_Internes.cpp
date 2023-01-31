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

#include <Faces_Internes.h>

Implemente_instanciable(Faces_Internes, "Faces_Internes", LIST(Faces_Interne));

Sortie& Faces_Internes::printOn(Sortie& os) const { return LIST(Faces_Interne)::printOn(os); }

Entree& Faces_Internes::readOn(Entree& is) { return LIST(Faces_Interne)::readOn(is); }

/*! @brief Associe une domaine a tous les objets Faces_Interne de la liste.
 *
 * @param (Domaine& une_domaine) le domaine a associer aux Faces_Interne de la liste
 */
void Faces_Internes::associer_domaine(const Domaine& une_domaine)
{
  for (auto& itr : *this) itr.associer_domaine(une_domaine);
}

/*! @brief Renvoie le nombre total de faces contenues dans la liste des Faces_Interne, i.
 *
 * e. la somme de toutes
 *     les faces de tous les objet Faces_Interne contenu dans
 *     la liste.
 *
 * @return (int) le nombre total de faces contenues dans la liste des Faces_Interne
 */
int Faces_Internes::nb_faces() const
{
  int nombre = 0;
  for (const auto &itr : *this) nombre += itr.nb_faces();

  return nombre;
}

/*! @brief Renvoie le nombre total de faces de type specifie contenues dans la liste des Faces_Interne
 *
 *     i.e. la somme de toutes les faces de type specifie
 *     de tous les objet Faces_Interne contenu dans
 *     la liste.
 *
 * @param (Type_Face type) le type des faces a compter
 * @return (int) le nombre total de faces contenues dans la liste des Faces_Interne
 */
int Faces_Internes::nb_faces(Type_Face type) const
{
  int nombre = 0;
  for (const auto &itr : *this)
    if (type == itr.faces().type_face()) nombre += itr.nb_faces();

  return nombre;
}
