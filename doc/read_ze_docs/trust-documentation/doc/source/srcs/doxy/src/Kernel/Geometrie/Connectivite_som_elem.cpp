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
#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>
#include <TRUSTTabs.h>

/*! @brief construction de la structure som_elem pour le domaine donnee On cree pour chaque sommet i la liste des elements adjacents a ce sommet
 *
 *   (c'est la liste des elements k tels que il existe j tel que les_elems(k,j) == i)
 *
 * @param (nb_sommets)
 * @param (les_elems)
 * @param (som_elem)
 * @param (include_virtual)
 */
void construire_connectivite_som_elem(const int       nb_sommets,
                                      const IntTab&      les_elems,
                                      Static_Int_Lists& som_elem,
                                      const int       include_virtual)
{
  // Nombre d'elements du domaine
  const int nb_elem = (include_virtual) ? les_elems.dimension_tot(0) : les_elems.dimension(0);
  // Nombre de sommets par element
  const int nb_sommets_par_element = les_elems.dimension(1);

  // Construction d'un tableau initialise a zero : pour chaque sommet,
  // nombre d'elements voisins de ce sommet
  ArrOfInt nb_elements_voisins(nb_sommets);

  // Premier passage : on calcule le nombre d'elements voisins de chaque
  // sommet pour creer la structure de donnees
  int elem, i;

  for (elem = 0; elem < nb_elem; elem++)
    {
      for (i = 0; i < nb_sommets_par_element; i++)
        {
          int sommet = les_elems(elem, i);
          // GF cas des polyedres
          if (sommet==-1) break;
          nb_elements_voisins[sommet]++;
        }
    }

  som_elem.set_list_sizes(nb_elements_voisins);

  // On reutilise le tableau pour stocker le nombre d'elements dans
  // chaque liste pendant qu'on la remplit
  nb_elements_voisins = 0;

  // Remplissage du tableau des elements voisins.
  for (elem = 0; elem < nb_elem; elem++)
    {
      for (i = 0; i < nb_sommets_par_element; i++)
        {
          int sommet = les_elems(elem, i);
          // GF cas des polyedres
          if (sommet==-1) break;
          int n = (nb_elements_voisins[sommet])++;
          som_elem.set_value(sommet, n, elem);
        }
    }

  // Tri de toutes les listes dans l'ordre croissant
  som_elem.trier_liste(-1);
}

/*! @brief Cherche les elements qui contiennent tous les sommets du tableau sommets_to_find (permet de trouver les elements
 *
 *   adjacents a une face ou une arete)
 *
 * @param (som_elem) pour chaque sommet, liste triee des elements adjacents (voir construire_connectivite_som_elem)
 * @param (sommets_to_find) une liste de sommets
 * @param (elements) resultat de la recherche: la liste des elements qui contiennent tous les sommets de sommets_to_find. Si sommets_to_find est vide, on renvoie un tableau vide. (en cas d'appels repetes a cette fonction, il est conseille de mettre le drapeau "smart_resize")
 */
void find_adjacent_elements(const Static_Int_Lists& som_elem,
                            const ArrOfInt& sommets_to_find,
                            ArrOfInt& elements)
{
  int nb_som_to_find = sommets_to_find.size_array();
  // on retire les sommets valant -1 (cas ou plusieurs types de faces)
  while (sommets_to_find[nb_som_to_find-1]==-1) nb_som_to_find--;
  if (nb_som_to_find == 0)
    {
      elements.resize_array(0);
      return;
    }
  // Algorithme: on initialise elements avec tous les elements adjacents
  //  au premier sommet de la liste.
  //  Puis pour chacun des autres sommets de la liste, on retire du tableau
  //  "elements" les elements qui ne sont pas voisins du sommet.
  //  A la fin, il ne reste que les elements qui sont dans toutes les listes.
  {
    // Initialisation avec les elements adjacents au premier sommet
    const int sommet = sommets_to_find[0];
    som_elem.copy_list_to_array(sommet, elements);
  }
  int nb_elem_found = elements.size_array();
  int i_sommet;
  for (i_sommet = 1; i_sommet < nb_som_to_find; i_sommet++)
    {
      const int sommet = sommets_to_find[i_sommet];
      // Calcul des elements communs entre elements[.] et som_elem(sommet,.)
      // Nombre d'elements communs entre elements et la nouvelle liste de sommets
      int nb_elems_restants = 0;
      // Nombre d'elements adjacents au "sommet"
      const int nb_elem_liste = som_elem.get_list_size(sommet);
      // On suppose que les listes d'elements sont triees dans l'ordre croissant
      // On parcourt simultanement les deux listes et on conserve les elements
      // communs.
      int i = 0;
      int j = 0;
      if (nb_elem_found == 0)
        break;
      if (nb_elem_liste > 0)
        {
          while (1)
            {
              const int elem_i = elements[i];
              const int elem_j = som_elem(sommet, j);
              if (elem_i == elem_j)
                {
                  // Element commun aux deux listes, on le garde
                  elements[nb_elems_restants] = elem_i;
                  nb_elems_restants++;
                }
              if (elem_i >= elem_j)
                {
                  j++;
                  if (j >= nb_elem_liste)
                    break;
                }
              if (elem_j >= elem_i)
                {
                  i++;
                  if (i >= nb_elem_found)
                    break;
                }
            }
        }
      else
        {
          nb_elems_restants = 0;
        }
      nb_elem_found = nb_elems_restants;
    }
  elements.resize_array(nb_elem_found);
}

