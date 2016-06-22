/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Static_Int_Lists.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#include <Static_Int_Lists.h>
#include <Vect_ArrOfInt.h>

// Description: detruit toutes les listes
void Static_Int_Lists::reset()
{
  index_.resize_array(0);
  valeurs_.resize_array(0);
}

// Description: detruit les listes existantes et en cree de nouvelles.
//  On cree autant de listes que d'elements dans le tableau sizes.
//  La i-ieme liste a une taille sizes[i]
//  Les valeurs sizes doivent etre positives ou nulles.
void Static_Int_Lists::set_list_sizes(const ArrOfInt& sizes)
{
  reset();

  const int nb_listes = sizes.size_array();
  index_.resize_array(nb_listes + 1);
  // Construction du tableau d'index
  index_[0];
  int i;
  for (i = 0; i < nb_listes; i++)
    {
      assert(sizes[i] >= 0);
      index_[i+1] = index_[i] + sizes[i];
    }
  const int somme_sizes = index_[nb_listes];
  valeurs_.resize_array(somme_sizes);
}

// Description: remplace les valeurs stockes par toutes les listes par celles
//  du tableau data. data doit avoir pour taille la somme des tailles de toutes
//  les listes.
void Static_Int_Lists::set_data(const ArrOfInt& data)
{
  assert(data.size_array() == valeurs_.size_array());
  valeurs_.inject_array(data);
}

#ifndef NDEBUG
// Verifie la coherence du tableau index et data
static int check_index_data(const ArrOfInt& index, const ArrOfInt& data)
{
  if (index.size_array() < 1)
    return 0;
  if (index[0] != 0)
    return 0;
  const int n = index.size_array() - 1; // nombre de listes
  for (int i = 0; i < n; i++)
    if (index[i+1] < index[i])
      return 0;
  if (index[n] != data.size_array())
    return 0;
  return 1;
}
#endif

// Description: remplace index et data.
void Static_Int_Lists::set_index_data(const ArrOfInt& index, const ArrOfInt& data)
{
  assert(check_index_data(index, data));
  index_ = index;
  valeurs_ = data;
}

// Description: tri par ordre croissant des valeurs de la i-ieme liste.
//  Si num_liste < 0, on trie toutes les listes.
void Static_Int_Lists::trier_liste(int num_liste)
{
  const int i_debut = (num_liste < 0) ? 0 : num_liste;
  const int i_fin   = (num_liste < 0) ? index_.size_array() - 1 : num_liste + 1;

  int i;
  ArrOfInt valeurs_liste;
  for (i = i_debut; i < i_fin; i++)
    {
      const int index = index_[i];
      const int size  = index_[i+1] - index;
      valeurs_liste.ref_array(valeurs_, index, size);
      valeurs_liste.ordonne_array();
    }
}

// Description: copie la i-ieme liste dans le tableau fourni
//  Le tableau array doit etre resizable.
void Static_Int_Lists::copy_list_to_array(int i, ArrOfInt& array) const
{
  const int n = get_list_size(i);
  array.resize_array(n, Array_base::NOCOPY_NOINIT);
  const int index = index_[i];
  array.inject_array(valeurs_, n, 0 /* destination index */, index /* source index */);
}

Sortie& Static_Int_Lists::printOn(Sortie& os) const
{
  os << index_   << space;
  os << valeurs_ << space;
  return os;
}

Entree& Static_Int_Lists::readOn(Entree& is)
{
  reset();
  is >> index_;
  is >> valeurs_;
  return is;
}

Sortie& Static_Int_Lists::ecrire(Sortie& os) const
{
  os << "nb lists       : " << get_nb_lists() << finl;
  os << "sizes of lists : ";
  for (int i=0; i<get_nb_lists(); ++i)
    {
      os << get_list_size(i) << " ";
    }
  os << finl;

  for (int i=0; i<get_nb_lists(); ++i)
    {
      os << "{ " ;
      const int sz = get_list_size(i);
      for (int j=0; j<sz; ++j)
        {
          os <<  valeurs_[(index_[i]+j)] << " ";
        }
      os << "}" << finl;
    }
  return os;
}

void Static_Int_Lists::set(const VECT(ArrOfInt) & src)
{
  const int nb_lists = src.size();
  index_.resize_array(nb_lists + 1, Array_base::NOCOPY_NOINIT);
  int idx = 0;
  int i;
  index_[0] = 0;
  for (i = 0; i < nb_lists; i++)
    {
      idx += src[i].size_array();
      index_[i+1] = idx;
    }

  valeurs_.resize_array(idx, Array_base::NOCOPY_NOINIT);
  idx = 0;
  for (i = 0; i < nb_lists; i++)
    {
      const ArrOfInt& a = src[i];
      int sz = a.size_array();
      valeurs_.inject_array(a, sz, idx /* dest index */, 0 /* source index */);
      idx += sz;
    }
}
