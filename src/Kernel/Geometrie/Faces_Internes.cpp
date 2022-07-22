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

#include <Faces_Internes.h>

Implemente_liste(Faces_Interne);
Implemente_instanciable(Faces_Internes,"Faces_Internes",LIST(Faces_Interne));


/*! @brief
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Faces_Internes::printOn(Sortie& os) const
{
  return LIST(Faces_Interne)::printOn(os);
}


/*! @brief
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Faces_Internes::readOn(Entree& is)
{
  return LIST(Faces_Interne)::readOn(is);
}


/*! @brief Associe une zone a tous les objets Faces_Interne de la liste.
 *
 * @param (Zone& une_zone) la zone a associer aux Faces_Interne de la liste
 */
void Faces_Internes::associer_zone(const Zone& une_zone)
{
  LIST_CURSEUR(Faces_Interne) curseur(*this);
  while(curseur)
    {
      curseur->associer_zone(une_zone);
      ++curseur;
    }
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
  int nombre=0;
  CONST_LIST_CURSEUR(Faces_Interne) curseur(*this);
  while(curseur)
    {
      nombre+=curseur->nb_faces();
      ++curseur;
    }
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
  CONST_LIST_CURSEUR(Faces_Interne) curseur(*this);
  int nombre=0;
  while(curseur)
    {
      if (type == curseur->faces().type_face())
        nombre+=curseur->nb_faces();
      ++curseur;
    }
  return nombre;
}
