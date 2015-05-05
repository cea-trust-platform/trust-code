/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Array_tools.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Array_tools.h>
#include <IntTab.h>
#include <ArrOfDouble.h>

// Description: retire les doublons du tableau array
//  (suppose que le tableau est trie dans l'ordre croissant)
//  On deplace les elements conserves pour qu'ils soient contigus
//  au debut du tableau.
//  ATTENTION: on ne resize pas le tableau, utiliser la valeur
//  de retour pour le faire ensuite!
//  (au cas ou on veuille appliquer cette fonction a un IntTab,
//   il faut faire resize() et non resize_array())
// Valeur de retour : nombre d'elements conserves dans le tableau
int array_retirer_doublons(ArrOfInt& array)
{
  const int size = array.size_array();
  if (size == 0)
    return 0;
  // Retire les doublons
  int new_size = 1;
  int last_value = array[0];
  for (int i = 1; i < size; i++)
    {
      // Le tableau doit etre trie !
      assert(array[i] >= last_value);
      if (array[i] != last_value)
        {
          array[new_size] = last_value = array[i];
          new_size++;
        }
    }
  return new_size;
}

// Description: Trie le tableau array dans l'ordre croissant
//  et retire les doublons.
void array_trier_retirer_doublons(ArrOfInt& array)
{
  // IntVect n'est pas traite correctement car on ne
  //  fait pas un resize() mais un resize_array().
  assert(!sub_type(IntVect, array));
  const int size = array.size_array();
  if (size == 0)
    return;
  // Tri dans l'ordre croissant
  array.ordonne_array();
  // Retire les doublons
  const int sz = array_retirer_doublons(array);
  array.resize_array(sz);
}

// Description: calcule l'intersection entre les deux listes d'entiers
//  liste1 et liste2. Le resultat est mis dans liste1.
//  Les deux listes doivent etre triees et sans doublons. liste1 est
//  triee en sortie.
void array_calculer_intersection(ArrOfInt& liste1, const ArrOfInt& liste2)
{
  const int sz1 = liste1.size_array();
  const int sz2 = liste2.size_array();
  int i; // Pointeur en lecture dans liste1
  int j = 0; // Pointeur en lecture dans liste2
  int k = 0; // Pointeur en ecriture
  for (i = 0; i < sz1; i++)
    {
      // On verifie que les listes sont triees dans l'ordre croissant
      assert((i >= sz1-1) || (liste1[i] < liste1[i+1]));
      assert((j >= sz2-1) || (liste2[j] < liste2[j+1]));
      const int valeur_i = liste1[i];
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

// Description:
//  Retire de "sorted_array" les elements qui figurent dans "sorted_elements".
//  Les deux tableaux doivent etre initialement ordonnes dans l'ordre croissant.
//  Exemple:
//   En entree sorted_array=[1,4,9,10,12,18], sorted_elements=[3,5,9,10,18,25]
//   En sortie sorted_array=[1,4,12]
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

// Description: cherche la "valeur" dans le tableau tab par recherche binaire
//  Le tableau tab doit etre trie dans l'ordre croissant
//  Si elle n'est pas trouvee, renvoie -1 (y compris si tab est vide),
//  sinon, renvoie un index i tel que tab[i] == valeur
//  (si la valeur figure plusieurs fois dans le tableau, on ne renvoie
//  pas forcement la premiere occurence).
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

static int fct_qsort_nbcolonnes;

int fct_qsort_tableau_2(const void *ptr1, const void *ptr2)
{
  const int *t1 = (const int *) ptr1;
  const int *t2 = (const int *) ptr2;
  int delta = t1[0] - t2[0];
  int delta2 = t1[1] - t2[1];
  return delta ? delta : delta2;
}

int fct_qsort_tableau_n(const void *ptr1, const void *ptr2)
{
  const int *t1 = (const int *) ptr1;
  const int *t2 = (const int *) ptr2;
  const int n = fct_qsort_nbcolonnes - 1;
  int i;
  for (i = 0; i < n; i++)
    {
      int delta = t1[i] - t2[i];
      if (delta)
        return delta;
    }
  return t1[i] - t2[i];
}

// Description: tri lexicographique du tableau tab (par ordre croissant
//  de la premiere colonne, si premiere colonne identique, ordre croissant
//  de la deuxieme, etc).
//  Le tableau ne doit pas etre un tableau distribue.
//  Valeur de retour: nombre de colonnes du tableau (produit des tab.dimension(i) pour i>0)
int tri_lexicographique_tableau(IntTab& tab)
{
  // On verifie que le tableau n'est pas un tableau distribue:
  assert(!tab.get_md_vector().non_nul());

  const int nb_lignes = tab.dimension(0);
  const int nb_colonnes = tab.line_size();

  if (nb_lignes != 0)
    {
      if (nb_colonnes == 1)
        tab.ordonne_array();
      else if (nb_colonnes == 2)
        qsort(tab.addr(), nb_lignes, nb_colonnes * sizeof(int), fct_qsort_tableau_2);
      else
        {
          fct_qsort_nbcolonnes = nb_colonnes;
          qsort(tab.addr(), nb_lignes, nb_colonnes * sizeof(int), fct_qsort_tableau_n);
        }
    }

  return nb_colonnes;
}

static const IntVect *fct_qsort_tab_ptr = 0;

int fct_qsort_tableau_1_indirect(const void *ptr1, const void *ptr2)
{
  const int t1 = *((const int *) ptr1);
  const int t2 = *((const int *) ptr2);
  const IntVect& tab = *fct_qsort_tab_ptr;
  int delta = tab[t1] - tab[t2];
  return delta;
}

int fct_qsort_tableau_2_indirect(const void *ptr1, const void *ptr2)
{
  const int t1 = *((const int *) ptr1) * 2;
  const int t2 = *((const int *) ptr2) * 2;
  const IntVect& tab = *fct_qsort_tab_ptr;
  int delta = tab[t1] - tab[t2];
  int delta2 = tab[t1+1] - tab[t2+1];
  return delta ? delta : delta2;
}

int fct_qsort_tableau_n_indirect(const void *ptr1, const void *ptr2)
{
  const int nc = fct_qsort_nbcolonnes;
  int t1 = *((const int *) ptr1) * nc;
  int t2 = *((const int *) ptr2) * nc;
  const IntVect& tab = *fct_qsort_tab_ptr;
  const int n = nc - 1;
  int i;
  for (i = 0; i < n; i++)
    {
      int delta = tab[t1++] - tab[t2++];
      if (delta)
        return delta;
    }
  return tab[t1] - tab[t2];
}

// Description:
//  Idem que tri_lexicographique_tableau mais on trie le tableau index qui
//  contient les indices de lignes du tableau tab tel que tab(index[i], *) soit
//  croissant quant i augmente. Tri de tous les indices de index...
//  Si le tableau index est de taille nulle, on en cree un de taille tab.dimension_tot(0)
//  Sinon on suppose qu'il contient deja des indices de lignes dans tab.
//  Valeur de retour: nombre de colonnes du tableau (produit des tab.dimension(i) pour i>0)
int tri_lexicographique_tableau_indirect(const IntTab& tab, ArrOfInt& index)
{
  // On verifie que le tableau n'est pas un tableau distribue:
  assert(!tab.get_md_vector().non_nul());

  const int dimtab = tab.dimension_tot(0);
  if (index.size_array() == 0 && dimtab > 0)
    {
      index.resize_array(dimtab, Array_base::NOCOPY_NOINIT);
      for (int i = 0; i < dimtab; i++)
        index[i] = i;
    }

  const int nb_lignes = index.size_array();
  const int nb_colonnes = tab.line_size();

  if (nb_lignes != 0)
    {
      if (fct_qsort_tab_ptr)
        {
          Cerr << "Internal error in tri_lexicographique_tableau_indirect !" << finl;
          Process::exit();
          // Aie ! on essaye de faire du multithread ??? acces concurrent au pointeur
        }
      fct_qsort_tab_ptr = &tab;
      if (nb_colonnes == 1)
        qsort(index.addr(), index.size_array(), sizeof(int), fct_qsort_tableau_1_indirect);
      else if (nb_colonnes == 2)
        qsort(index.addr(), index.size_array(), sizeof(int), fct_qsort_tableau_2_indirect);
      else
        {
          fct_qsort_nbcolonnes = nb_colonnes;
          qsort(index.addr(), index.size_array(), sizeof(int), fct_qsort_tableau_n_indirect);
        }
      fct_qsort_tab_ptr = 0;
    }

  return nb_colonnes;
}

void resize_tab_lines(IntTab& tab, const int n)
{
  tab.resize_dim0(n);
}

// Description: Trie le tableau tab dans l'ordre lexicographique
//  et retire les doublons (attention [1,2] n'est pas egal a [2,1])
void tableau_trier_retirer_doublons(IntTab& tab)
{
  const int nb_lignes = tab.dimension(0);
  if (nb_lignes == 0)
    return;

  const int nb_colonnes = tri_lexicographique_tableau(tab);

  // Retire les doublons
  if (nb_colonnes == 1)
    {
      const int new_size = array_retirer_doublons(tab);
      resize_tab_lines(tab, new_size);
    }
  else if (nb_colonnes == 2)
    {
      int j = 1; // Taille du tableau apres suppression
      int last_x = tab(0, 0);
      int last_y = tab(0, 1);
      for (int i = 1; i < nb_lignes; i++)
        {
          const int x = tab(i, 0);
          const int y = tab(i, 1);
          if (x != last_x || y != last_y)
            {
              tab(j, 0) = last_x = x;
              tab(j, 1) = last_y = y;
              j++;
            }
        }
      resize_tab_lines(tab, j);
    }
  else
    {
      int j = 0; // Derniere ligne retenue
      fct_qsort_nbcolonnes = nb_colonnes;
      for (int i = 1; i < nb_lignes; i++)
        {
          // Si la ligne i est differente de la ligne j, on la conserve:
          if (!fct_qsort_tableau_n(&tab(i, 0), &tab(j, 0)))
            {
              j++;
              for (int k = 0; k < nb_colonnes; k++)
                tab(j, k) = tab(i, k);
            }
        }
      resize_tab_lines(tab, j+1);
    }
}

static inline int same_line(const IntVect& v, int i, int j)
{
  const int ls = v.line_size();
  int k;
  for (k = 0; k < ls; k++)
    if (v[i*ls+k] != v[j*ls+k])
      break;
  return k == ls;
}

// Description: cherche par un tri lexicographique les lignes identiques de "tab"
//   et initialise les tailles et contenus de renum et renum_inverse.
//   renum est de taille tab.dimension_tot(0).
//    renum[i] contiendra l'indice de la ligne i dans le tableau reduit trie (contenant les lignes uniques)
//   renum_inverse contient, pour chaque ligne du tableau reduit trie, le plus petit indice de la ligne
//    correspondante dans tab.
//    on peut construire le tableau reduit trie en extayant les lignes tab( renum_inverse[i], ...)
void calculer_renum_sans_doublons(const IntTab& tab, ArrOfInt& renum, ArrOfInt& renum_inverse)
{
  // MODIF ELI LAUCOIN 31/01/2012 :
  // Je re-ecris completement cette fonction

  // index permet de parcourir le tableau tab dans l'ordre
  ArrOfInt index;
  tri_lexicographique_tableau_indirect(tab, index);
  const int n = index.size_array();

  // on redimensionne renum et renum_index
  renum.resize_array(n, Array_base::NOCOPY_NOINIT);
  renum_inverse.resize_array(n, Array_base::NOCOPY_NOINIT);

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
  renum_inverse.set_smart_resize(1); // pas de reallocation
  renum_inverse.resize_array(count+1);
  // FIN MODIF ELI LAUCOIN 31/01/2012
}

void array_smart_allocate(ArrOfInt& array, const int n)
{
  array.set_smart_resize(1);
  // get memory for the requested size
  array.resize_array(n, Array_base::NOCOPY_NOINIT);
  // and set actual size to zero
  array.resize_array(0);
}

void append_array_to_array(ArrOfInt& dest, const ArrOfInt& src)
{
  const int n1 = dest.size_array();
  const int n2 = src.size_array();
  dest.resize_array(n1+n2);
  dest.inject_array(src, n2 /* nb elem */, n1 /* dest index */, 0 /* src index */);
}

void append_array_to_array(ArrOfDouble& dest, const ArrOfDouble& src)
{
  const int n1 = dest.size_array();
  const int n2 = src.size_array();
  dest.resize_array(n1+n2);
  dest.inject_array(src, n2 /* nb elem */, n1 /* dest index */, 0 /* src index */);
}
