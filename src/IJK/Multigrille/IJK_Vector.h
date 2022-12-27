/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef IJK_Vector_included
#define IJK_Vector_included

#include <TRUST_Vector.h>
#include <TRUSTTab.h>

/*! @brief classe IJK_Vector
 *
 *  - La classe template IJK_Vector derive de la classe template TRUST_Vector
 *
 *  - Elle demande 2 template arguments
 */
template<template<typename,typename> class _TRUST_TABL_,typename _TYPE_, typename _TYPE_ARRAY_>
class IJK_Vector: public TRUST_Vector<_TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>>
{
protected:

  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    IJK_Vector *xxx = new IJK_Vector(*this);
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

  Sortie& printOn(Sortie& s) const override { return TRUST_Vector<_TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>>::printOn(s); }
  Entree& readOn(Entree& s) override { return TRUST_Vector<_TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>>::readOn(s); }

public:
  IJK_Vector() : TRUST_Vector<_TRUST_TABL_<_TYPE_, _TYPE_ARRAY_>>() { }
  IJK_Vector(int i) : TRUST_Vector<_TRUST_TABL_<_TYPE_, _TYPE_ARRAY_>>(i) { }
  IJK_Vector(const IJK_Vector& avect) : TRUST_Vector<_TRUST_TABL_<_TYPE_, _TYPE_ARRAY_>>(avect) { }

  IJK_Vector& operator=(const IJK_Vector& avect)
  {
    TRUST_Vector<_TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>>::operator=(avect);
    return *this;
  }
};

#endif /* IJK_Vector_included */
