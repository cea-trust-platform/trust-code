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

#include <Vect_impl.h>
#include <TRUSTTab.h>

// class inspired by TRUST_Vector but for IJK_Fields (that takes 2 template arguments)
template<template<typename,typename> class _TRUST_TABL_,typename _TYPE_, typename _TYPE_ARRAY_>
class IJK_Vector: public Vect_impl
{
protected:

  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    IJK_Vector *xxx = new IJK_Vector(*this);
    if (!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Objet_U* cree_une_instance() const override
  {
    _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_> *instan = new _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>();
    return instan;
  }

  Sortie& printOn(Sortie& s) const override { return Vect_impl::printOn(s); }

  Entree& readOn(Entree& s) override { return Vect_impl::readOn(s); }

public:
  IJK_Vector() : Vect_impl() { }
  IJK_Vector(int i) { build_vect_impl(i); }
  IJK_Vector(const IJK_Vector& avect) : Vect_impl(avect) { }

  /* Returns the ith VECT element */
  const _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>& operator[](int i) const { return static_cast<const _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>&>(Vect_impl::operator[](i)); }
  _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>& operator[](int i) { return static_cast<_TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>&>(Vect_impl::operator[](i)); }

  const _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>& operator()(int i) const { return operator[](i); }
  _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>& operator()(int i) { return operator[](i); }

  IJK_Vector& operator=(const IJK_Vector& avect)
  {
    Vect_impl::operator=(avect);
    return *this;
  }

  Entree& lit(Entree& s) { return Vect_impl::lit(s); }

  _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>& add(const _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>& data_to_add)
  {
    Vect_impl::add(data_to_add);
    return (*this)[size() - 1];
  }

  _TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>& add() { return add(_TRUST_TABL_<_TYPE_,_TYPE_ARRAY_>()); }
  void add(const IJK_Vector& data_to_add) { Vect_impl::add(data_to_add); }
};

#endif /* IJK_Vector_included */
