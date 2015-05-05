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
// File:        ArrOfFloat.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ArrOfFloat_included
#define ArrOfFloat_included

#include <Objet_U.h>

class ArrOfFloat : public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(ArrOfFloat);
public:
  ArrOfFloat();
  ArrOfFloat(int n);
  ArrOfFloat(const ArrOfFloat& array);
  const ArrOfFloat& operator=(const ArrOfFloat& array);
  ~ArrOfFloat();

  inline float   operator[](int i) const;
  inline float& operator[](int i);
  void        ordonne_array();
  ArrOfFloat& inject_array(const ArrOfFloat& source,
                           int nb_elements = -1,
                           int first_element_dest = 0,
                           int first_element_source = 0);

  inline int size_array() const;
  float * addr();
  const float * addr() const;
  inline void resize_array(int size);
  ArrOfFloat& operator*= (const float) ;
  ArrOfFloat& operator/= (const float) ;
protected:
  void mem_resize(int size);
  void invalidate(int first_element, int nb_elements);

  float *data_;
  int size_;         // Nombre d'elements du tableau
  int memory_size_;  // Taille allouee ( on a memory_size_ >= size_ )
  static const int grow_factor_num_;
  static const int grow_factor_denom_;
};

// *******************************************************************

// Description: Renvoie la i-ieme valeur du tableau (0 <= i < taille)
inline float ArrOfFloat::operator[](int i) const
{
  assert(i >= 0 && i < size_);
  return data_[i];
}

// Description: Renvoie la i-ieme valeur du tableau (0 <= i < taille)
inline float& ArrOfFloat::operator[](int i)
{
  assert(i >= 0 && i < size_);
  return data_[i];
}

// Description: Renvoie le nombre de valeurs dans le tableau
inline int ArrOfFloat::size_array() const
{
  return size_;
}

// Description: Changement de taille du tableau, en cas d'augmentation,
// le contenu des nouveaux elements est indefini.
inline void ArrOfFloat::resize_array(int size)
{
  assert(size >= 0);
  if (size > memory_size_)
    mem_resize(size);
#ifndef NDEBUG
  // En mode debug, on remplit les nouvelles entrees avec des valeurs invalides
  if (size > size_)
    invalidate(size_, size - size_);
#endif
  size_ = size;
}

#endif
