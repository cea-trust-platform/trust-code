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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Joint_Items.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Joint_Items.h>

// **********************************************************
//  Implementation de la classe Joint_Items
// **********************************************************

// Description: constructeur par defaut
Joint_Items::Joint_Items()
{
  nb_items_virtuels_ = -1;
  nb_items_reels_ = -1;
  flags_init_ = 0;
}

// Description: remise dans l'etat initial obtenu apres constructeur par defaut
void Joint_Items::reset()
{
  nb_items_virtuels_ = -1;
  nb_items_reels_ = -1;
  flags_init_ = 0;
  items_communs_.reset();
  items_distants_.reset();
  renum_items_communs_.reset();
}

// Description: Voir items_communs_
const ArrOfInt& Joint_Items::items_communs() const
{
  return items_communs_;
}

// Description:  Renvoie le tableau items_communs_ pour le remplir.
//  (BM: ce tableau n'est pas encore rempli)
ArrOfInt& Joint_Items::set_items_communs()
{
  flags_init_ |= 1;
  return items_communs_;
}

// Description: Voir items_distants_
const ArrOfInt& Joint_Items::items_distants() const
{
  assert(flags_init_ & 2);
  return items_distants_;
}

// Description:  Renvoie le tableau items_distants_ pour le remplir
// Voir Scatter::calculer_espace_distant,
//      Scatter::calculer_espace_distant_faces_frontieres,
//      Scatter::calculer_espace_distant_elements
ArrOfInt& Joint_Items::set_items_distants()
{
  flags_init_ |= 2;
  return items_distants_;
}

// Description:  Voir nb_items_virtuels_
// Voir Scatter::calculer_nb_items_virtuels
void Joint_Items::set_nb_items_virtuels(int n)
{
  flags_init_ |= 4;
  nb_items_virtuels_ = n;
}

// Description:  Voir nb_items_virtuels_
int Joint_Items::nb_items_virtuels() const
{
  assert(flags_init_ & 4);
  return nb_items_virtuels_;
}

// Description:  Voir renum_items_communs_
// Voir Scatter::calculer_colonne0_renum_faces_communes
//      Scatter::construire_correspondance_sommets_par_coordonnees
IntTab& Joint_Items::set_renum_items_communs()
{
  flags_init_ |= 8;
  return renum_items_communs_;
}

// Description:  Voir renum_items_communs_
const IntTab& Joint_Items::renum_items_communs() const
{
  assert(flags_init_ & 8);
  return renum_items_communs_;
}

// Description: Pas encore utilise
void Joint_Items::set_nb_items_reels(int n)
{
  assert(n >= 0);
  flags_init_ |= 16;
  nb_items_reels_ = n;
}

// Description: Pas encore utilise (prevu pour faciliter la creation
//  des tableaux distribues, mais les joints ne sont pas le bon endroit
//  pour stocker cette valeur: il faut pouvoir la stocker meme s'il n'y
//  a aucun processeur voisin).
int Joint_Items::nb_items_reels() const
{
  assert(flags_init_ & 16);
  return nb_items_reels_;
}
