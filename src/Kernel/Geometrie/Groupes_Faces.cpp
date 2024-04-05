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

#include <Groupes_Faces.h>

Implemente_instanciable_32_64(Groupes_Faces_32_64, "Groupes_Faces", LIST(Groupe_Faces_32_64<_T_>));

template <typename _SIZE_>
Sortie& Groupes_Faces_32_64<_SIZE_>::printOn(Sortie& os) const { return LIST(Groupe_Faces_32_64<_SIZE_>)::printOn(os); }

template <typename _SIZE_>
Entree& Groupes_Faces_32_64<_SIZE_>::readOn(Entree& is) { return LIST(Groupe_Faces_32_64<_SIZE_>)::readOn(is); }

/*! @brief Associe un domaine a tous les objets Groupe_Faces de la liste.
 *
 * @param (Domaine& un_domaine) le domaine a associer aux Groupe_Faces de la liste
 */
template <typename _SIZE_>
void Groupes_Faces_32_64<_SIZE_>::associer_domaine(const Domaine_t& un_domaine)
{
  for (auto& itr : *this) itr.associer_domaine(un_domaine);
}

/*! @brief Renvoie le nombre total de faces contenues dans la liste des Groupe_Faces, i.
 *
 * e. la somme de toutes
 *     les faces de tous les objet Groupe_Faces contenu dans
 *     la liste.
 *
 * @return (int) le nombre total de faces contenues dans la liste des Groupe_Faces
 */
template <typename _SIZE_>
typename Groupes_Faces_32_64<_SIZE_>::int_t Groupes_Faces_32_64<_SIZE_>::nb_faces() const
{
  int_t nombre = 0;
  for (const auto &itr : *this) nombre += itr.nb_faces();

  return nombre;
}

/*! @brief Renvoie le nombre total de faces de type specifie contenues dans la liste des Groupe_Faces
 *
 *     i.e. la somme de toutes les faces de type specifie
 *     de tous les objet Groupe_Faces contenu dans
 *     la liste.
 *
 * @param (Type_Face type) le type des faces a compter
 * @return (int) le nombre total de faces contenues dans la liste des Groupe_Faces
 */
template <typename _SIZE_>
typename Groupes_Faces_32_64<_SIZE_>::int_t Groupes_Faces_32_64<_SIZE_>::nb_faces(Type_Face type) const
{
  int_t nombre = 0;
  for (const auto &itr : *this)
    if (type == itr.faces().type_face()) nombre += itr.nb_faces();

  return nombre;
}

/*! Mise a jour des indices des groupes de faces avec table inversee: reverse_index[ancien_numero] = nouveau numero:
 * necessaire lorsque les numeros des faces ont ete modifies
 */
template <typename _SIZE_>
void Groupes_Faces_32_64<_SIZE_>::renumerote(ArrOfInt_t& reverse_index)
{
  for (auto &itr : *this)
    {
      ArrOfInt_t& indices_faces = itr.get_indices_faces();
      const int_t nbfaces2    = indices_faces.size_array();
      assert(nbfaces2 == itr.nb_faces());
      for (int_t i = 0; i < nbfaces2; i++)
        {
          const int_t old = indices_faces[i]; // ancien indice local
          indices_faces[i] = reverse_index[old];
        }
    }
}


template class Groupes_Faces_32_64<int>;
#if INT_is_64_ == 2
template class Groupes_Faces_32_64<trustIdType>;
#endif


