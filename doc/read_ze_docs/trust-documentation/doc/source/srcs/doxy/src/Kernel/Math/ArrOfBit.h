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

#ifndef ArrOfBit_included
#define ArrOfBit_included

#include <assert.h>
#include <Objet_U.h>

class ArrOfBit : public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(ArrOfBit);
public:
  ArrOfBit(int n=0);
  ArrOfBit(const ArrOfBit& array);              // Constructeur par copie
  ~ArrOfBit() override;                                  // Destructeur
  ArrOfBit& operator=(const ArrOfBit& array);   // Operateur copie
  ArrOfBit& operator=(int i);
  inline int operator[](int i) const;
  inline void setbit(int i) const;
  inline int testsetbit(int i) const;
  inline void clearbit(int i) const;
  inline int size_array() const;
  ArrOfBit& resize_array(int n);
protected:
  int calculer_int_size(int taille) const;
  int taille;
  unsigned int *data;
  static const unsigned int SIZE_OF_INT_BITS;
  static const unsigned int DRAPEAUX_INT;
};

/*! @brief Renvoie 1 si le bit e est mis, 0 sinon.
 *
 */
inline int ArrOfBit::operator[](int e) const
{
  assert(e >= 0 && e < taille);
  unsigned int i = (unsigned int) e;
  unsigned int x = data[i >> SIZE_OF_INT_BITS];
  unsigned int flag = 1 << (i & DRAPEAUX_INT);
  int resultat = ((x & flag) != 0) ? 1 : 0;
  return resultat;
}

/*! @brief Met le bit e a 1.
 *
 */
inline void ArrOfBit::setbit(int e) const
{
  assert(e >= 0 && e < taille);
  unsigned int i = (unsigned int) e;
  unsigned int flag = 1 << (i & DRAPEAUX_INT);
  data[i >> SIZE_OF_INT_BITS] |= flag;
}

/*! @brief Renvoie la valeur du bit e, puis met le bit e a 1.
 *
 */
inline int ArrOfBit::testsetbit(int e) const
{
  assert(e >= 0 && e < taille);
  unsigned int i = (unsigned int) e;
  unsigned int flag = 1 << (i & DRAPEAUX_INT);
  int index = i >> SIZE_OF_INT_BITS;
  unsigned int old = data[index];
  data[index] = old | flag;
  return ((old & flag) != 0) ? 1 : 0;
}

/*! @brief Met le bit e a 0.
 *
 */
inline void ArrOfBit::clearbit(int e) const
{
  assert(e >= 0 && e < taille);
  unsigned int i = (unsigned int) e;
  unsigned int flag = 1 << (i & DRAPEAUX_INT);
  data[i >> SIZE_OF_INT_BITS] &= ~flag;
}

/*! @brief Renvoie la taille du tableau en bits
 *
 */
inline int ArrOfBit::size_array() const
{
  return taille;
}

#endif
