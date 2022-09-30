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

#ifndef TRUST_Vector_included
#define TRUST_Vector_included

#include <Vect_impl.h>

/*! @brief 2 classes templates
 *
 *  - TRUST_Vector pour n'importe quelle classe
 *      Utilisation : using Vect_Milieu_base = TRUST_Vector<Milieu_base>
 *
 *  - TRUSTTabs_Vector pour les classes templates TRUSTArray, TRUSTVect, TRUSTTab et TRUSTTrav ...
 *      Utilisation : using Vect_ArrOfDouble = TRUSTTabs_Vector<TRUSTArray,double>
 *
 */

// MACRO to replace VECT(THECLASS) by Vect_THECLASS
#define VECT(_TYPE_) name2(Vect_,_TYPE_)

/////////////////////////////////////////////////////////////////////////////////////////
// =================================================================================== //
/////////////////////////////////////////////////////////////////////////////////////////

template<typename _CLASSE_>
class TRUST_Vector: public Vect_impl
{
protected:
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    TRUST_Vector *xxx = new TRUST_Vector(*this);
    if (!xxx) Cerr << "Not enough memory " << finl, Process::exit();
    return xxx->numero();
  }

  Objet_U* cree_une_instance() const override
  {
    _CLASSE_ *instan = new _CLASSE_();
    return instan;
  }

  Sortie& printOn(Sortie& s) const override { return Vect_impl::printOn(s); }
  Entree& readOn(Entree& s) override { return Vect_impl::readOn(s); }

public:
  TRUST_Vector() : Vect_impl() { }
  TRUST_Vector(int i) { build_vect_impl(i); }
  TRUST_Vector(const TRUST_Vector& avect) : Vect_impl(avect) { }

  /* Returns the ith VECT element */
  const _CLASSE_& operator[](int i) const { return static_cast<const _CLASSE_&>(Vect_impl::operator[](i)); }
  _CLASSE_& operator[](int i) { return static_cast<_CLASSE_&>(Vect_impl::operator[](i)); }

  const _CLASSE_& operator()(int i) const { return operator[](i); }
  _CLASSE_& operator()(int i) { return operator[](i); }

  TRUST_Vector& operator=(const TRUST_Vector& avect)
  {
    Vect_impl::operator=(avect);
    return *this;
  }

  Entree& lit(Entree& s) { return Vect_impl::lit(s); }

  _CLASSE_& add(const _CLASSE_& data_to_add)
  {
    Vect_impl::add(data_to_add);
    return (*this)[size() - 1];
  }

  _CLASSE_& add() { return add(_CLASSE_()); }
  void add(const TRUST_Vector& data_to_add) { Vect_impl::add(data_to_add); }
};

/////////////////////////////////////////////////////////////////////////////////////////
// =================================================================================== //
/////////////////////////////////////////////////////////////////////////////////////////

template<template<typename> class _TRUST_TABL_,typename _TYPE_>
class TRUSTTabs_Vector: public Vect_impl
{
protected:
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    TRUSTTabs_Vector *xxx = new TRUSTTabs_Vector(*this);
    if (!xxx) Cerr << "Not enough memory " << finl, Process::exit();
    return xxx->numero();
  }

  Objet_U* cree_une_instance() const override
  {
    _TRUST_TABL_<_TYPE_> *instan = new _TRUST_TABL_<_TYPE_>();
    return instan;
  }

  Sortie& printOn(Sortie& s) const override { return Vect_impl::printOn(s); }
  Entree& readOn(Entree& s) override { return Vect_impl::readOn(s); }

public:
  TRUSTTabs_Vector() : Vect_impl() { }
  TRUSTTabs_Vector(int i) { build_vect_impl(i); }
  TRUSTTabs_Vector(const TRUSTTabs_Vector& avect) : Vect_impl(avect) { }

  /* Returns the ith VECT element */
  const _TRUST_TABL_<_TYPE_>& operator[](int i) const { return static_cast<const _TRUST_TABL_<_TYPE_>&>(Vect_impl::operator[](i)); }
  _TRUST_TABL_<_TYPE_>& operator[](int i) { return static_cast<_TRUST_TABL_<_TYPE_>&>(Vect_impl::operator[](i)); }

  const _TRUST_TABL_<_TYPE_>& operator()(int i) const { return operator[](i); }
  _TRUST_TABL_<_TYPE_>& operator()(int i) { return operator[](i); }

  TRUSTTabs_Vector& operator=(const TRUSTTabs_Vector& avect)
  {
    Vect_impl::operator=(avect);
    return *this;
  }

  Entree& lit(Entree& s) { return Vect_impl::lit(s); }

  _TRUST_TABL_<_TYPE_>& add(const _TRUST_TABL_<_TYPE_>& data_to_add)
  {
    Vect_impl::add(data_to_add);
    return (*this)[size() - 1];
  }

  _TRUST_TABL_<_TYPE_>& add() { return add(_TRUST_TABL_<_TYPE_>()); }
  void add(const TRUSTTabs_Vector& data_to_add) { Vect_impl::add(data_to_add); }
};

#endif /* TRUST_Vector_included */
