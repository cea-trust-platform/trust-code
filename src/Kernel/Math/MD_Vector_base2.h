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
// File:        MD_Vector_base2.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////
#ifndef MD_Vector_base2_included
#define MD_Vector_base2_included
#include <MD_Vector_base.h>
#include <ArrOfInt.h>

class MD_Vector_base2 : public MD_Vector_base
{
  Declare_base_sans_constructeur(MD_Vector_base2);
public:
  MD_Vector_base2();
  virtual int get_nb_items_reels() const
  {
    return nb_items_reels_;
  }
  virtual int get_nb_items_tot() const
  {
    return nb_items_tot_;
  }
  virtual const ArrOfInt& get_items_to_compute() const
  {
    return blocs_items_to_compute_;
  }
  virtual const ArrOfInt& get_items_to_sum() const
  {
    return blocs_items_to_sum_;
  }
  virtual int nb_items_seq_tot() const
  {
    return nb_items_seq_tot_;
  }
  virtual int nb_items_seq_local() const
  {
    return nb_items_seq_local_;
  }

  static inline void append_item_to_blocs(ArrOfInt& blocs, int item);

  // Nombre total d'items (reels+virtuels), utilise pour connaitre la taille des tableaux a creer
  int   nb_items_tot_;
  // Nombre d'items "reels" (propriete dimension(0) du tableau ou size() pour le vecteur)
  // En principe les items reels sont tous ceux appartenant aux elements reels, ils sont regroupes au
  // debut du tableau aux indices 0 <= i < nb_items_reels_.
  // Si ce nombre vaut -1, il n'y a pas de separation entre items reels et items virtuels
  //  (cas des tableaux P1Bulle multilocalisation pour lesquels les items reels et virtuels sont melanges)
  int   nb_items_reels_;

  // Nombre total (sur tous les procs) d'items sequentiels (c'est mp_sum(nb_items_seq_local_))
  int   nb_items_seq_tot_;
  // Nombre d'items sequentiels sur ce processeur (c'est le nombre d'items dans les blocs de blocs_items_to_sum_)
  int   nb_items_seq_local_;

  // ***** Les membres suivants sont utilises pour calculer des sommes, produits scalaires, normes ******
  // Indices de tous les items dont je suis proprietaire (ce sont les "items sequentiels", definis comme
  //  etant tous ceux dont la valeur n'est pas recue d'un autre processeur lors d'un echange_espace_virtuel).
  //  Pour faire une somme sur tous les items, il faut sommer les valeurs de tous les items
  //  de ces blocs. Le tableau contient debut_bloc1, fin_bloc1, debut_bloc2, fin_bloc2, etc...
  //  (fin_bloc est l'indice du dernier element + 1)
  //  (structure utilisee pour les sauvegardes sequentielles (xyz ou debog), les calculs de normes de vecteurs, etc)
  ArrOfInt blocs_items_to_sum_;
  // Indices de tous les items pour lesquels il faut calculer une valeur
  //  (utilise par DoubleTab::operator+=(const DoubleTab &) par exemple)
  // En theorie, il suffirait de prendre blocs_items_to_sum_, mais ce dernier est
  //  plein de trous et peut etre inefficace. En pratique, on calcule toutes les valeurs
  //  reeles.
  ArrOfInt blocs_items_to_compute_;
};

// Description: methode outil pour ajouter un item a un tableau du genre "blocs" contenant
//  des series de blocs... alonge le bloc precedent ou commence un nouveau bloc si l'item
//  n'est pas contigu avec le bloc precedent.
void MD_Vector_base2::append_item_to_blocs(ArrOfInt& blocs, int item)
{
  int n = blocs.size_array();
  assert(n%2 == 0); // le tableau contient des blocs, donc un nombre pair d'elements
  assert(n == 0 || item >= blocs[n-1]); // les items doivent etre ajoutes dans l'ordre croissant
  if (n == 0 || blocs[n-1] != item)
    {
      // nouveau bloc
      blocs.append_array(item);
      blocs.append_array(item+1);
    }
  else
    {
      blocs[n-1] = item + 1;
    }
}
#endif
