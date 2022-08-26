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

#include <ArrOfBit.h>
#include <string.h>

const unsigned int ArrOfBit::SIZE_OF_INT_BITS = 5;
const unsigned int ArrOfBit::DRAPEAUX_INT = 31;

ArrOfBit::ArrOfBit(entier n)
{
  taille = 0;
  data = 0;
  resize_array(n);
}

ArrOfBit::~ArrOfBit()
{
  if (data)
    delete[] data;
  data = 0;
}

ArrOfBit::ArrOfBit(const ArrOfBit& array)
{
  taille = 0;
  data = 0;
  operator=(array);
}

entier ArrOfBit::calculer_int_size(entier taille) const
{
  assert(taille >= 0);
  entier siz = taille >> SIZE_OF_INT_BITS;
  if (taille & DRAPEAUX_INT)
    siz++;
  return siz;
}

ArrOfBit& ArrOfBit::resize_array(entier n)
{
  if (taille == n)
    return *this;
  assert(n >= 0);
  if (n > 0)
    {
      entier oldsize = calculer_int_size(taille);
      entier newsize = calculer_int_size(n);
      unsigned int * newdata = new unsigned int[newsize];
      entier size_copy = (newsize > oldsize) ? oldsize : newsize;
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

ArrOfBit& ArrOfBit::operator=(const ArrOfBit& array)
{
  entier newsize = calculer_int_size(array.taille);
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

ArrOfBit& ArrOfBit::operator=(entier val)
{
  unsigned int valeur = val ? (~((unsigned int) 0)) : 0;
  entier size = calculer_int_size(taille);
  entier i;
  for (i = 0; i < size; i++)
    data[i] = valeur;
  return *this;
}

Sortie& ArrOfBit::printOn(Sortie& os) const
{
  os << taille << finl;
  entier i;
  // Un retour a la ligne tous les 32 bits, Une espace tous les 8 bits
  for (i = 0; i < taille; i++)
    {
      os << operator[](i);
      if ((i & 7) == 7)
        os << " ";
      if ((i & 31) == 31)
        os << finl;
    }
  // Un retour a la ligne si la derniere ligne n'etait pas terminee
  if (i & 31)
    os << finl;
  return os;
}

Entree& ArrOfBit::readOn(Entree& is)
{
  entier newsize;
  is >> newsize;
  resize_array(newsize);
  operator=(0);

  entier i;
  for (i = 0; i < taille; i++)
    {
      entier bit;
      is >> bit;
      if (bit) setbit(i);
    }
  return is;
}
