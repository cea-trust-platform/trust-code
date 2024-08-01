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

#include <Joint_Items.h>

// **********************************************************
//  Implementation de la classe Joint_Items_32_64
// **********************************************************

/*! @brief constructeur par defaut
 *
 */
template <typename _SIZE_>
Joint_Items_32_64<_SIZE_>::Joint_Items_32_64()
{
  nb_items_virtuels_ = -1;
  nb_items_reels_ = -1;
  flags_init_ = 0;
}

/*! @brief remise dans l'etat initial obtenu apres constructeur par defaut
 *
 */
template <typename _SIZE_>
void Joint_Items_32_64<_SIZE_>::reset()
{
  nb_items_virtuels_ = -1;
  nb_items_reels_ = -1;
  flags_init_ = 0;
  items_communs_.reset();
  items_distants_.reset();
  renum_items_communs_.reset();
}


/*! @brief Renvoie le tableau items_communs_ pour le remplir.
 *
 * (BM: ce tableau n'est pas encore rempli)
 *
 */
template <typename _SIZE_>
typename Joint_Items_32_64<_SIZE_>::ArrOfInt_t& Joint_Items_32_64<_SIZE_>::set_items_communs()
{
  flags_init_ |= 1;
  return items_communs_;
}

/*! @brief Voir items_distants_
 *
 */
template <typename _SIZE_>
const typename Joint_Items_32_64<_SIZE_>::ArrOfInt_t& Joint_Items_32_64<_SIZE_>::items_distants() const
{
  assert(flags_init_ & 2);
  return items_distants_;
}

/*! @brief Renvoie le tableau items_distants_ pour le remplir Voir Scatter::calculer_espace_distant,
 *
 *       Scatter::calculer_espace_distant_faces_frontieres,
 *       Scatter::calculer_espace_distant_elements
 *
 */
template <typename _SIZE_>
typename Joint_Items_32_64<_SIZE_>::ArrOfInt_t& Joint_Items_32_64<_SIZE_>::set_items_distants()
{
  flags_init_ |= 2;
  return items_distants_;
}

/*! @brief Voir nb_items_virtuels_ Voir Scatter::calculer_nb_items_virtuels
 *
 */
template <typename _SIZE_>
void Joint_Items_32_64<_SIZE_>::set_nb_items_virtuels(int n)
{
  flags_init_ |= 4;
  nb_items_virtuels_ = n;
}

/*! @brief Voir nb_items_virtuels_
 *
 */
template <typename _SIZE_>
int Joint_Items_32_64<_SIZE_>::nb_items_virtuels() const
{
  assert(flags_init_ & 4);
  return nb_items_virtuels_;
}

/*! @brief Voir renum_items_communs_ Voir Scatter::calculer_colonne0_renum_faces_communes
 *
 *       Scatter::construire_correspondance_sommets_par_coordonnees
 *
 */
template <typename _SIZE_>
typename Joint_Items_32_64<_SIZE_>::IntTab_t& Joint_Items_32_64<_SIZE_>::set_renum_items_communs()
{
  flags_init_ |= 8;
  return renum_items_communs_;
}

/*! @brief Voir renum_items_communs_
 *
 */
template <typename _SIZE_>
const typename Joint_Items_32_64<_SIZE_>::IntTab_t& Joint_Items_32_64<_SIZE_>::renum_items_communs() const
{
  assert(flags_init_ & 8);
  return renum_items_communs_;
}

/*! @brief Pas encore utilise
 *
 */
template <typename _SIZE_>
void Joint_Items_32_64<_SIZE_>::set_nb_items_reels(int n)
{
  assert(n >= 0);
  flags_init_ |= 16;
  nb_items_reels_ = n;
}

/*! @brief Pas encore utilise (prevu pour faciliter la creation des tableaux distribues, mais les joints ne sont pas le bon endroit
 *
 *   pour stocker cette valeur: il faut pouvoir la stocker meme s'il n'y
 *   a aucun processeur voisin).
 *
 */
template <typename _SIZE_>
int Joint_Items_32_64<_SIZE_>::nb_items_reels() const
{
  assert(flags_init_ & 16);
  return nb_items_reels_;
}


template class Joint_Items_32_64<int>;
#if INT_is_64_ == 2
template class Joint_Items_32_64<trustIdType>;
#endif


