/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Array_tools.h>

/*! @brief Trie le tableau array dans l'ordre croissant et retire les doublons.
 *
 */
template <typename _TYPE_, typename _SIZE_>
void array_trier_retirer_doublons(TRUSTArray<_TYPE_,_SIZE_>& array)
{
  // IntVect n'est pas traite correctement car on ne fait pas un resize() mais un resize_array().
  assert(typeid(array) != typeid(TRUSTVect<_TYPE_, _SIZE_>));
  const _SIZE_ size = array.size_array();
  if (size == 0)
    return;
  // Tri dans l'ordre croissant
  array.ordonne_array();

  // Retire les doublons
  auto last = std::unique(array.addr(), array.addr()+size);
  _SIZE_ new_size =  static_cast<_SIZE_>(std::distance(array.addr(), last));
  array.resize_array(new_size);
}


/*! @brief calcule l'intersection entre les deux listes d'entiers liste1 et liste2.
 *
 * Le resultat est mis dans liste1.
 *   Les deux listes doivent etre triees et sans doublons. liste1 est
 *   triee en sortie.
 *
 */
template <typename _TYPE_, typename _SIZE_>
void array_calculer_intersection(TRUSTArray<_TYPE_,_SIZE_>& liste1, const TRUSTArray<_TYPE_,_SIZE_>& liste2)
{
  const _SIZE_ sz1 = liste1.size_array();
  const _SIZE_ sz2 = liste2.size_array();
  _SIZE_ j = 0; // Pointeur en lecture dans liste2
  _SIZE_ k = 0; // Pointeur en ecriture
  for (_SIZE_ i = 0; i < sz1; i++)
    {
      // On verifie que les listes sont triees dans l'ordre croissant
      assert((i >= sz1-1) || (liste1[i] < liste1[i+1]));
      assert((j >= sz2-1) || (liste2[j] < liste2[j+1]));
      const _TYPE_ valeur_i = liste1[i];
      // Avancer dans liste2 jusqu'a trouver ou depasser liste1[i]
      while (j < sz2 && liste2[j] < valeur_i)
        j++;
      if (j == sz2)
        break; // Bout de liste, on a fini
      if (liste2[j] == valeur_i)
        {
          liste1[k] = valeur_i;
          k++; // On garde cette valeur
        }
    }
  liste1.resize_array(k);
}

/*! @brief Retire de "sorted_array" les elements qui figurent dans "sorted_elements".
 *
 * Les deux tableaux doivent etre initialement ordonnes dans l'ordre croissant.
 *   Exemple:
 *    En entree sorted_array=[1,4,9,10,12,18], sorted_elements=[3,5,9,10,18,25]
 *    En sortie sorted_array=[1,4,12]
 *
 */
void array_retirer_elements(ArrOfInt& sorted_array, const ArrOfInt& sorted_elements_list)
{
  int i_read;      // Index dans sorted_array (en lecture)
  int i_write = 0; // Index dans sorted_array (la ou on ecrit)
  int j = 0;       // Index dans sorted_elements
  const int n = sorted_array.size_array();
  const int m = sorted_elements_list.size_array();
  if (m == 0)
    return;

  int j_value = sorted_elements_list[j];
  for (i_read = 0; i_read < n; i_read++)
    {
      // Tableau trie ?
      assert(i_read == 0 || sorted_array[i_read] > sorted_array[i_read-1]);
      const int i_value = sorted_array[i_read];

      // On avance dans la liste sorted_elements jusqu'a trouver ou depasser
      // l'element i_value
      while ((j_value < i_value) && (j < m))
        {
          j++;
          if (j == m)
            break;
          assert(sorted_elements_list[j] > j_value); // Tableau trie ?
          j_value = sorted_elements_list[j];
        }

      if (j == m || j_value != i_value)
        {
          // i_value ne figure pas dans le tableau sorted_elements, on le garde
          sorted_array[i_write] = i_value;
          i_write++;
        }
    }
  sorted_array.resize_array(i_write);
}

template <typename _SIZE_>
static inline int same_line(const IntTab_T<_SIZE_>& v, _SIZE_ i, _SIZE_ j)
{
  const int ls = v.line_size();
  for (int k = 0; k < ls; k++)
    if (v(i,k) != v(j,k))
      return 0;
  return 1;
}

/*! @brief tri lexicographique du tableau tab (par ordre croissant de la premiere colonne, si premiere colonne identique, ordre croissant
 *
 *   de la deuxieme, etc).
 *   Le tableau ne doit pas etre un tableau distribue.
 *   Valeur de retour: nombre de colonnes du tableau (produit des tab.dimension(i) pour i>0)
 *
 */
template <typename _TYPE_, typename _SIZE_>
int tri_lexicographique_tableau(TRUSTTab<_TYPE_,_SIZE_>& tab)
{
  // On verifie que le tableau n'est pas un tableau distribue:
  assert(!tab.get_md_vector().non_nul());

  const _SIZE_ nb_lignes = tab.dimension(0);
  const int nb_colonnes = tab.line_size();
  if (nb_lignes != 0)
    {
      tab.ensureDataOnHost();
      if (nb_colonnes == 1)
        tab.ordonne_array();
      else if (nb_colonnes == 2)
        {
          using pairs = std::array<_TYPE_, 2>;
          _TYPE_ *ptr = tab.addr();
          pairs* tmp = reinterpret_cast<pairs*>(ptr);
          std::sort(tmp, tmp+nb_lignes);
        }
      else if (nb_colonnes == 3)
        {
          using triplets = std::array<_TYPE_, 3>;
          _TYPE_ *ptr = tab.addr();
          triplets* tmp = reinterpret_cast<triplets*>(ptr);
          std::sort(tmp, tmp+nb_lignes);
        }
      else if (nb_colonnes == 4)
        {
          using quadruplets = std::array<_TYPE_, 4>;
          _TYPE_ *ptr = tab.addr();
          quadruplets* tmp = reinterpret_cast<quadruplets*>(ptr);
          std::sort(tmp, tmp+nb_lignes);
        }
      else
        {
          Cerr << "tri_lexicographique_tableau not supported for TRUST tabs with more than 4 columns" << finl;
          Process::exit();
        }
    }
  return nb_colonnes;
}

/*! @brief Idem que tri_lexicographique_tableau mais on trie le tableau index qui contient les indices de lignes du tableau tab tel que tab(index[i], *) soit
 *
 *   croissant quant i augmente. Tri de tous les indices de index...
 *   Si le tableau index est de taille nulle, on en cree un de taille tab.dimension_tot(0)
 *   Sinon on suppose qu'il contient deja des indices de lignes dans tab.
 *   Valeur de retour: nombre de colonnes du tableau (produit des tab.dimension(i) pour i>0)
 *
 */
template <typename _TYPE_, typename _SIZE_>
int tri_lexicographique_tableau_indirect(const TRUSTTab<_TYPE_,_SIZE_>& tab, ArrOfInt_T<_SIZE_>& index)
{
  using int_t = _SIZE_;
  // On verifie que le tableau n'est pas un tableau distribue:
  assert(!tab.get_md_vector().non_nul());

  const int_t dimtab = tab.dimension_tot(0);
  if (index.size_array() == 0 && dimtab > 0)
    {
      index.resize_array(dimtab, RESIZE_OPTIONS::NOCOPY_NOINIT);
      for (int_t i = 0; i < dimtab; i++)
        index[i] = i;
    }

  const int_t nb_lignes = index.size_array();
  const int nb_colonnes = tab.line_size();
  const int nb_dim = tab.nb_dim();
  const double epsilon = Objet_U::precision_geom;

  if (nb_lignes != 0)
    {
      std::sort(index.begin(), index.end(), [&](int_t a, int_t b)
      {
        if(nb_dim == 1)
          return ( tab(a)<tab(b) );
        for (int i = 0; i < nb_colonnes; i++)
          {
            if ( std::fabs(tab(a,i)-tab(b,i)) > epsilon )
              return ( tab(a,i)<tab(b,i) );
          }
        return false;
      });
    }
  return nb_colonnes;
}

/*! @brief Trie le tableau tab dans l'ordre lexicographique et retire les doublons (attention [1,2] n'est pas egal a [2,1])
 *
 */
template <typename _SIZE_>
void tableau_trier_retirer_doublons(IntTab_T<_SIZE_>& tab)
{
  const _SIZE_ nb_lignes = tab.dimension(0);
  if (nb_lignes == 0) return;
  int nb_colonnes = tab.line_size();

  if (nb_colonnes == 1)
    array_trier_retirer_doublons(tab);
  else
    {
      nb_colonnes = tri_lexicographique_tableau(tab);
      if (nb_colonnes == 2)
        {
          _SIZE_ j = 1; // Taille du tableau apres suppression
          _SIZE_ last_x = tab(0, 0);
          _SIZE_ last_y = tab(0, 1);
          for (_SIZE_ i = 1; i < nb_lignes; i++)
            {
              const _SIZE_ x = tab(i, 0);
              const _SIZE_ y = tab(i, 1);
              if (x != last_x || y != last_y)
                {
                  tab(j, 0) = last_x = x;
                  tab(j, 1) = last_y = y;
                  j++;
                }
            }
          tab.resize_dim0(j);
        }
      else
        {
          _SIZE_ j = 0; // Derniere ligne retenue
          for (_SIZE_ i = 1; i < nb_lignes; i++)
            {
              // Si la ligne i est differente de la ligne j, on la conserve:
              if (!same_line(tab, i, j))
                {
                  j++;
                  for (int k = 0; k < nb_colonnes; k++)
                    tab(j, k) = tab(i, k);
                }
            }
          tab.resize_dim0(j+1);
        }
    }
}

// Explicit instanciations
//template const IntVect_T<int> *fct_qsort_tab_ptr<int>;
template int tri_lexicographique_tableau_indirect(const TRUSTTab<int,int>& tab, ArrOfInt_T<int>& index);
template int tri_lexicographique_tableau_indirect(const TRUSTTab<double,int>& tab, ArrOfInt_T<int>& index);
template int tri_lexicographique_tableau(TRUSTTab<int,int>& tab);
template int tri_lexicographique_tableau(TRUSTTab<double,int>& tab);
template void tableau_trier_retirer_doublons(IntTab_T<int>& tab);
template void array_calculer_intersection(TRUSTArray<int,int>& liste1, const TRUSTArray<int,int>& liste2);
template void array_trier_retirer_doublons(TRUSTArray<int,int>& array);
template void array_trier_retirer_doublons(TRUSTArray<double,int>& array);

#if INT_is_64_ == 2
//template const IntVect_T<trustIdType> *fct_qsort_tab_ptr<trustIdType>;
template int tri_lexicographique_tableau_indirect(const TRUSTTab<int,trustIdType>& tab, ArrOfInt_T<trustIdType>& index);
template int tri_lexicographique_tableau_indirect(const TRUSTTab<trustIdType,trustIdType>& tab, ArrOfInt_T<trustIdType>& index);
template int tri_lexicographique_tableau_indirect(const TRUSTTab<double,trustIdType>& tab, ArrOfInt_T<trustIdType>& index);
template int tri_lexicographique_tableau(TRUSTTab<int,trustIdType>& tab);
template int tri_lexicographique_tableau(TRUSTTab<trustIdType,int>& tab);
template int tri_lexicographique_tableau(TRUSTTab<trustIdType,trustIdType>& tab);
template int tri_lexicographique_tableau(TRUSTTab<double,trustIdType>& tab);
template void tableau_trier_retirer_doublons(IntTab_T<trustIdType>& tab);
template void array_calculer_intersection(TRUSTArray<int,trustIdType>& liste1, const TRUSTArray<int,trustIdType>& liste2);
template void array_trier_retirer_doublons(TRUSTArray<int,trustIdType>& array);
template void array_trier_retirer_doublons(TRUSTArray<trustIdType,trustIdType>& array);
template void array_trier_retirer_doublons(TRUSTArray<double,trustIdType>& array);
#endif


/*! @brief cherche par un tri lexicographique les lignes identiques de "tab" et initialise les tailles et contenus de renum et renum_inverse.
 *
 *    renum est de taille tab.dimension_tot(0).
 *     renum[i] contiendra l'indice de la ligne i dans le tableau reduit trie (contenant les lignes uniques)
 *    renum_inverse contient, pour chaque ligne du tableau reduit trie, le plus petit indice de la ligne
 *     correspondante dans tab.
 *     on peut construire le tableau reduit trie en extayant les lignes tab( renum_inverse[i], ...)
 *
 */
void calculer_renum_sans_doublons(const IntTab& tab, ArrOfInt& renum, ArrOfInt& renum_inverse)
{
  // MODIF ELI LAUCOIN 31/01/2012 :
  // Je re-ecris completement cette fonction

  // index permet de parcourir le tableau tab dans l'ordre
  ArrOfInt index;
  tri_lexicographique_tableau_indirect(tab, index);
  const int n = index.size_array();

  // on redimensionne renum et renum_index
  renum.resize_array(n, RESIZE_OPTIONS::NOCOPY_NOINIT);
  renum_inverse.resize_array(n, RESIZE_OPTIONS::NOCOPY_NOINIT);

  int count  = -1; // compteur de lignes dans le tableau reduit
  int latest = -1; // indice dans le tableau initial de la derniere ligne ajoutee dans le tableau reduit

  for (int i=0; i<n; ++i)
    {
      // on parcourt tab dans l'ordre croissant donne par index.
      // si la ligne courante est differente de la derniere ligne ajoutee dans le tableau reduit,
      // on incremente count et on met a jour latest.
      if ( ( latest < 0 ) || ( !(same_line(tab,index[i],index[latest]))) )
        {
          ++count;
          latest=i;
        }

      // on indique dans renum ou se trouve la ligne courante dans le tableau reduit
      renum[index[i]]      = count;

      // on ajoute la derniere ligne traitee au tableau reduit
      renum_inverse[count] = index[latest];
    }

  // on redimensionne renum_inverse a la taille du tableau reduit

  renum_inverse.resize_array(count+1);
  // FIN MODIF ELI LAUCOIN 31/01/2012
}

/*! @brief cherche la "valeur" dans le tableau tab par recherche binaire Le tableau tab doit etre trie dans l'ordre croissant
 *
*   Si elle n'est pas trouvee, renvoie -1 (y compris si tab est vide),
 *   sinon, renvoie un index i tel que tab[i] == valeur
 *   (si la valeur figure plusieurs fois dans le tableau, on ne renvoie
 *   pas forcement la premiere occurence).
 *
 *  Utilise dans Trio !
 */
int array_bsearch(const ArrOfInt& tab, int valeur)
{
  // attention tout est important !
  int i = 0;
  int j = tab.size_array(); // j = fin de tableau + 1 (important)
  while (j > i)
    {
      // Le tableau doit etre trie
      assert(j == tab.size_array() || tab[i] <= tab[j]);
      const int milieu = (i + j) / 2;
      const int val = tab[milieu];
      if (val > valeur)
        j = milieu; // prendre la valeur milieu et pas milieu - 1
      else if (val < valeur)
        i = milieu + 1; // prendre la valeur milieu + 1 et pas milieu
      else
        return milieu;
    }
  // Si on arrive ici, c'est que i==j, donc
  // - soit j == fin de tableau + 1 et on n'a pas trouve la valeur
  // - soit tab[j] a ete teste et n'est pas egale a valeur
  // Dans les deux cas, valeur n'est pas dans le tableau
  return -1;
}
