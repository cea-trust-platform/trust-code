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

#include <arch.h>
#include <ArrOfBit.h>
#include <cstring>

Implemente_instanciable_sans_constructeur_ni_destructeur_32_64(ArrOfBit_32_64,"ArrOfBit",Objet_U);

/*! @brief Constructeur d'un tableau de taille n, non initialise
 */
template <typename _SIZE_>
ArrOfBit_32_64<_SIZE_>::ArrOfBit_32_64(int_t n)
{
  taille = 0;
  data = 0;
  resize_array(n);
}

/*! @brief Destructeur.
 */
template <typename _SIZE_>
ArrOfBit_32_64<_SIZE_>::~ArrOfBit_32_64()
{
  if (data)
    delete[] data;
  data = 0;
}

/*! @brief Constructeur par copie (deep copy)
 */
template <typename _SIZE_>
ArrOfBit_32_64<_SIZE_>::ArrOfBit_32_64(const ArrOfBit_32_64& array):Objet_U(array)
{
  taille = 0;
  data = 0;
  operator=(array);
}

/*! @brief Taille en "int" du tableau requis pour stocker un tableau de bits de taille donnees.
 */
template <typename _SIZE_>
typename ArrOfBit_32_64<_SIZE_>::int_t ArrOfBit_32_64<_SIZE_>::calculer_int_size(int_t la_taille) const
{
  assert(la_taille >= 0);
  int_t siz = la_taille >> SIZE_OF_INT_BITS;
  if (la_taille & DRAPEAUX_INT)
    siz++;
  return siz;
}

/*! @brief Change la taille du tableau et copie les donnees existantes.
 *
 * Si la taille est plus petite, les donnees sont
 *  tronquees, et si la taille est plus grande, les nouveaux elements
 *  ne sont pas initialises.
 */
template <typename _SIZE_>
ArrOfBit_32_64<_SIZE_>& ArrOfBit_32_64<_SIZE_>::resize_array(int_t n)
{
  if (taille == n)
    return *this;
  assert(n >= 0);
  if (n > 0)
    {
      int_t oldsize = calculer_int_size(taille);
      int_t newsize = calculer_int_size(n);
      unsigned int * newdata = new unsigned int[newsize];
      int_t size_copy = (newsize > oldsize) ? oldsize : newsize;
      if (size_copy)
        {
          memcpy(newdata, data, size_copy);
          delete[] data;
        }
      data = newdata;
      taille = n;
    }
  else
    {
      delete[] data; // data!=0 sinon taille==n et on ne serait pas ici
      data = 0;
      taille = 0;
    }
  return *this;
}

/*! @brief Operateur copie (deep copy).
 */
template <typename _SIZE_>
ArrOfBit_32_64<_SIZE_>& ArrOfBit_32_64<_SIZE_>::operator=(const ArrOfBit_32_64& array)
{
  int_t newsize = calculer_int_size(array.taille);
  if (taille != array.taille)
    {
      if (data)
        {
          delete[] data;
          data = 0;
        }
      if (newsize > 0)
        data = new unsigned int[newsize];
    }
  taille = array.taille;
  if (taille)
    memcpy(data, array.data, newsize * sizeof(unsigned int));
  return *this;
}

/*! @brief Si la valeur est non nulle, met la valeur 1 dans tous les elements du tableau, sinon met la valeur 0.
 */
template <typename _SIZE_>
ArrOfBit_32_64<_SIZE_>& ArrOfBit_32_64<_SIZE_>::operator=(int_t val)
{
  unsigned int valeur = val ? (~((unsigned int) 0)) : 0;
  int_t size = calculer_int_size(taille);
  for (int_t i = 0; i < size; i++)
    data[i] = valeur;
  return *this;
}

/*! @brief Ecriture du tableau.
 *
 * Format: n
 *  0 1 0 0 1 0 ... (n valeurs)
 *
 */
template <typename _SIZE_>
Sortie& ArrOfBit_32_64<_SIZE_>::printOn(Sortie& os) const
{
#ifndef LATATOOLS
  os << taille << finl;
  // Un retour a la ligne tous les 32 bits,
  // Une espace tous les 8 bits
  int_t i = 0;
  for (; i < taille; i++)
    {
      os << operator[](i);
      if ((i & 7) == 7)
        os << tspace;
      if ((i & 31) == 31)
        os << finl;
    }
  // Un retour a la ligne si la derniere ligne n'etait pas terminee
  if (i & 31)
    os << finl;
#endif
  return os;
}

/*! @brief Lecture du tableau.
 *
 * Format: n
 *  0 1 0 0 1 0 ... (n valeurs)
 *
 */
template <typename _SIZE_>
Entree& ArrOfBit_32_64<_SIZE_>::readOn(Entree& is)
{
#ifndef LATATOOLS
  int_t newsize;
  is >> newsize;
  resize_array(newsize);
  operator=(0);

  for (int_t i = 0; i < taille; i++)
    {
      int bit;
      is >> bit;
      if (bit) setbit(i);
    }
#endif
  return is;
}

/////////////////////////////////////////////////
//// Template instanciations
/////////////////////////////////////////////////

template class ArrOfBit_32_64<int>;
#if INT_is_64_ == 2
template class ArrOfBit_32_64<trustIdType>;
#endif

