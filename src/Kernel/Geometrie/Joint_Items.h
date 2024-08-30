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

#ifndef Joint_Items_included
#define Joint_Items_included

#include <TRUSTTab.h>

/*! @brief Joint_Items contient les informations de distribution parallele d'un item geometrique particulier avec un domaine
 *     voisin particulier (item = sommet, element, face, etc..)
 *
 *     Ces structures sont initialisees dans Scatter.
 *     Elles sont ensuite utilisees par exemple pour creer
 *     un tableau distribue indexe par les indices des items
 *     geometriques.
 *
 * @sa class Joint
 */
template <typename _SIZE_>
class Joint_Items_32_64
{
public:

  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;

  Joint_Items_32_64();
  void reset();

  // Pour utiliser ces accesseurs, il faut avoir initialise
  // les structures au prealable avec set_xxx.
  int nb_items_reels() const;
  const ArrOfInt_t& items_communs() const {  return items_communs_; }
  const ArrOfInt_t& items_distants() const;
  int nb_items_virtuels() const;
  const IntTab_t& renum_items_communs() const;

  // Methodes d'initialisation des structures
  void set_nb_items_reels(int n);
  ArrOfInt_t& set_items_communs();
  ArrOfInt_t& set_items_distants();
  void set_nb_items_virtuels(int n);
  IntTab_t& set_renum_items_communs();

private:
  // Nombre d'items reels (permet de construire un tableau distribue)
  // uniquement avec les infos du joint.
  int nb_items_reels_;

  // Liste des items communs avec le domaine voisine (la liste est
  // classee dans le meme ordre sur le domaine locale et sur le domaine
  // voisine => items_communs[i] sur joint_j de domaine_k represente la meme
  // entite geometrique que items_communs[i] sur joint_k de domaine_j)
  ArrOfInt_t items_communs_;

  // Liste des items distants a envoyer au domaine voisine
  // (l'ordre des items dans cette liste determine l'ordre d'apparition
  // de ces items dans l'espace virtuel du voisin)
  ArrOfInt_t items_distants_;

  // Nombre d'items virtuels recus du domaine voisine.
  //  on a "nb_items_virtuels_ sur joint_j de domaine_k"
  //     = "items_distants.size_array() sur joint_k de domaine_j"
  int nb_items_virtuels_;

  // Correspondance entre l'indice local d'un item commun et l'indice
  // du meme item sur le domaine voisine:
  // colonne 0 = indice sur le domaine voisine,
  // colonne 1 = indice sur le domaine locale
  // dimension(0) est egal a items_communs.size_array()
  // L'ordre des items dans le tableau n'est pas specifie
  IntTab_t renum_items_communs_;

  // Qu'est ce qui a ete initialise ?
  int flags_init_;
};

using Joint_Items = Joint_Items_32_64<int>;
using Joint_Items_64 = Joint_Items_32_64<trustIdType>;

#endif
