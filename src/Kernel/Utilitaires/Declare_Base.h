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
// File:        Declare_Base.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


// .SECTION Description
// les macros Declare_base_sans_destructeur et
// Implemente_base_sans_destructeur
// permettent de creer simplement des classes
// de bases conforme a TRUST.

// .SECTION Description du header
// class A_base : public B {
// Declare_base_sans_destructeur(A_base);
// };
// .SECTION Description du source
// Implemente_base_sans_destructeur(A_base, "A_base", B);

#define Declare_base_sans_constructeur_ni_destructeur(_TYPE_)        \
                                                                \
  public :                                                        \
  static const Type_info info_obj;                                \
  static const Type_info* info();                                \
  const Type_info* get_info() const override;                                \
  /* methode rajoutee pour caster en python */                        \
  static _TYPE_& self_cast( Objet_U&) ;                                \
  static const _TYPE_& self_cast(const Objet_U&) ;                \
  protected :                                                        \
  Sortie& printOn(Sortie& x) const override;                        \
  Entree& readOn(Entree&) override

#define Declare_base_sans_constructeur(_TYPE_)                \
  public:                                                \
  ~_TYPE_();                                                \
  Declare_base_sans_constructeur_ni_destructeur(_TYPE_)

#define Declare_base_sans_destructeur(_TYPE_)                \
  public:                                                \
  _TYPE_();                                                \
  Declare_base_sans_constructeur_ni_destructeur(_TYPE_)

#define Declare_base(_TYPE_)                                \
  public:                                                \
  _TYPE_();                                                \
  ~_TYPE_();                                                \
  Declare_base_sans_constructeur_ni_destructeur(_TYPE_)

#define Implemente_base_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_) \
                                                                        \
  const Type_info* name2(_TYPE_,bases)[1]={                                \
    &(_BASE_::info_obj)};                                                \
  const Type_info _TYPE_::info_obj(_NOM_, 1, name2(_TYPE_,bases));        \
                                                                        \
  _TYPE_& _TYPE_::self_cast( Objet_U& r)   {                                \
    return ref_cast_non_const(_TYPE_,r); /* _non_const important sinon recursion dans ref_cast */ \
  }                                                                        \
  const _TYPE_& _TYPE_::self_cast(const Objet_U& r)   {                        \
    return ref_cast_non_const(_TYPE_,r); /* _non_const important sinon recursion dans ref_cast */ \
  }                                                                        \
  const Type_info*  _TYPE_::get_info() const {                                \
    return &info_obj;                                                        \
  }                                                                        \
  const Type_info*  _TYPE_::info() {                                        \
    return &info_obj;                                                        \
  }                                                                          \
  class __dummy__

#define Implemente_base_sans_constructeur(_TYPE_,_NOM_,_BASE_)                \
  _TYPE_::~_TYPE_() { }                                                        \
  Implemente_base_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_)

#define Implemente_base_sans_destructeur(_TYPE_,_NOM_,_BASE_)                \
  _TYPE_::_TYPE_() { }                                                        \
  Implemente_base_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_)

#define Implemente_base(_TYPE_,_NOM_,_BASE_)                                \
  _TYPE_::_TYPE_() { }                                                        \
  _TYPE_::~_TYPE_() { }                                                        \
  Implemente_base_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_)
