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
// File:        MacVect.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MacVECT_H
#define MacVECT_H

#include <Nom.h>
#include <assert.h>
#include <vect_impl.h>


#define declare_vect(_TYPE_)						\
  class VECT(_TYPE_) : public vect_impl					\
  {									\
    Declare_instanciable_sans_constructeur(VECT(_TYPE_));		\
    									\
  public :								\
    VECT(_TYPE_)();							\
    VECT(_TYPE_)(int );							\
    inline const _TYPE_& operator[](int) const;				\
    inline const _TYPE_& operator()(int i) const { return operator[](i); }; \
    inline _TYPE_& operator[](int);					\
    inline _TYPE_& operator()(int i) { return operator[](i); };		\
    VECT(_TYPE_)(const VECT(_TYPE_)&);					\
    VECT(_TYPE_)& operator=(const VECT(_TYPE_)& );			\
    _TYPE_& add(const _TYPE_& titi);					\
    _TYPE_& add();					\
    void add(const VECT(_TYPE_)&);				\
    Entree& lit(Entree& );						\
  protected:								\
    Objet_U* cree_une_instance() const;					\
  };									\
  /* Returns the ith VECT element */					\
  inline const _TYPE_& VECT(_TYPE_)::operator[](int i) const {		\
    return (const _TYPE_&)vect_impl::operator[](i);			\
  }									\
  /* Returns the ith VECT element */					\
  inline _TYPE_& VECT(_TYPE_)::operator[](int i) {			\
    return ( _TYPE_&)vect_impl::operator[](i);				\
  }

#define implemente_vect(_TYPE_)						\
  Implemente_instanciable_sans_constructeur(VECT(_TYPE_),string_macro_trio("VECT",VECT(_TYPE_)),vect_impl); \
  Sortie& VECT(_TYPE_)::printOn(Sortie& s) const {			\
    return vect_impl::printOn(s);					\
    }									\
  Entree& VECT(_TYPE_)::readOn(Entree& s) {				\
    return vect_impl::readOn(s);					\
      }									\
  Entree& VECT(_TYPE_)::lit(Entree& s) {                                \
    return vect_impl::lit(s) ;						\
    }									\
  Objet_U* VECT(_TYPE_)::cree_une_instance() const 			\
  { _TYPE_* instan = new _TYPE_();					\
    return instan ; }							\
  /* VECT constructors */						\
  VECT(_TYPE_)::VECT(_TYPE_)():vect_impl() { }				\
  VECT(_TYPE_)::VECT(_TYPE_)(int i) 					\
  {									\
    build_vect_impl(i) ;							\
  }									\
  /* Constructor by copy for VECT */					\
  VECT(_TYPE_)::VECT(_TYPE_)(const VECT(_TYPE_)& avect)			\
    : vect_impl(avect)  {  }						\
  /* VECT destructor */							\
  /* VECT operator= */							\
  VECT(_TYPE_)& VECT(_TYPE_)::operator=(const VECT(_TYPE_)& avect)	\
  {									\
    vect_impl::operator=(avect);					\
    return *this; 							\
    }									\
  _TYPE_& VECT(_TYPE_)::add() {\
  return add(_TYPE_()) ;\
  } \
  /* Add a new element to the VECT */					\
  _TYPE_& VECT(_TYPE_)::add(const _TYPE_& data_to_add) {		\
   vect_impl::add(data_to_add);					\
   return (*this)[size()-1];  \
  }									\
  /* Append a VECT to a VECT */						\
  void VECT(_TYPE_)::add(const VECT(_TYPE_)& data_to_add) {	\
    vect_impl::add(data_to_add);					\
  }

#define Declare_vect(_TYPE_)			\
  declare_vect(_TYPE_) class __dummy__
#define Implemente_vect(_TYPE_)			\
  implemente_vect(_TYPE_) class __dummy__

#endif
