/****************************************************************************
* Copyright (c) 2025, CEA
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


// .SECTION Description
// les macros Declare_instanciable_sans_destructeur et
// Implemente_instanciable_sans_destructeur
// permettent de creer simplement des classes
// de bases conforme a TRUST.

// .SECTION Description du header
// class A_base : public B {
// Declare_instanciable_sans_destructeur(A_base);
// };
// .SECTION Description du source
// Implemente_instanciable_sans_destructeur(A_base, "A_base", B);
#include <Cast.h>
#include <arch.h>

#ifdef LATATOOLS
#define Declare_instanciable_sans_constructeur_ni_destructeur(_TYPE_)        \
                                                                        \
  public :                                                                \
  unsigned taille_memoire() const override { return 0; };                                      \
  int duplique() const override { return 0; };                                                \
  protected :                                                                \
  Sortie& printOn(Sortie& x) const override;                                \
  Entree& readOn(Entree&) override

#else
#define Declare_instanciable_sans_constructeur_ni_destructeur(_TYPE_)        \
                                                                        \
  public :                                                                \
  static Objet_U* cree_instance() ;                                        \
  unsigned taille_memoire() const override;                                        \
  int duplique() const override;                                                \
  static const Type_info info_obj;                                        \
  static const Type_info* info();                                        \
  const Type_info* get_info() const override;                                        \
  /* methode rajoutee pour caster en python */                                \
  static _TYPE_& self_cast( Objet_U&) ;                                        \
  static const _TYPE_& self_cast(const Objet_U&) ;                        \
  protected :                                                                \
  Sortie& printOn(Sortie& x) const override;                                \
  Entree& readOn(Entree&) override
#endif

#define Declare_instanciable_sans_constructeur(_TYPE_)                \
  public:                                                        \
  ~_TYPE_();                                                        \
  Declare_instanciable_sans_constructeur_ni_destructeur(_TYPE_) \
 
#define Declare_instanciable_sans_destructeur(_TYPE_)                \
  public:                                                        \
  _TYPE_();                                                        \
  Declare_instanciable_sans_constructeur_ni_destructeur(_TYPE_) \
 
#define Declare_instanciable(_TYPE_)                                \
  public:                                                        \
  _TYPE_();                                                        \
  ~_TYPE_();                                                        \
  Declare_instanciable_sans_constructeur_ni_destructeur(_TYPE_) \
 
#ifdef LATATOOLS
#define Implemente_instanciable_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_)

#else
#define Implemente_instanciable_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_) \
                                                                        \
  const Type_info* name2(_TYPE_,bases)[1]={                                \
    &(_BASE_::info_obj)};                                                \
  const Type_info _TYPE_::info_obj(_NOM_, _TYPE_::cree_instance, 1, name2(_TYPE_,bases)); \
                                                                        \
                                                                        \
  int _TYPE_::duplique()  const                                        \
  {                                                                        \
    _TYPE_* xxx = new _TYPE_(*this);                                        \
    if(!xxx){Cerr << "Not enough memory " << finl; exit();}                \
    return xxx->numero();                                                \
  }                                                                        \
  _TYPE_& _TYPE_::self_cast( Objet_U& r)   {                                \
    return ref_cast_non_const(_TYPE_,r); /* _non_const important sinon  recursion dans ref_cast */ \
  }                                                                        \
  const _TYPE_& _TYPE_::self_cast(const Objet_U& r)   {                        \
    return ref_cast_non_const(_TYPE_,r); /* _non_const important sinon  recursion dans ref_cast */ \
  }                                                                        \
  Objet_U* _TYPE_::cree_instance()                                        \
  {  _TYPE_* xxx = new _TYPE_();                                        \
    if(!xxx){Cerr << "Not enough memory " << finl; exit();}                \
    return xxx ;                                                        \
  }                                                                        \
                                                                        \
  const Type_info*  _TYPE_::get_info() const {                                \
    return &info_obj;                                                        \
  }                                                                        \
                                                                        \
  const Type_info*  _TYPE_::info() {                                        \
    return &info_obj;                                                        \
  }                                                                        \
                                                                        \
  unsigned _TYPE_::taille_memoire() const {                                \
    return sizeof(_TYPE_);                                                \
  }                                                                      \
  class __dummy__
#endif

#define Implemente_instanciable_sans_constructeur(_TYPE_,_NOM_,_BASE_)        \
  _TYPE_::~_TYPE_() { }                                                        \
  Implemente_instanciable_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_)

#define Implemente_instanciable_sans_destructeur(_TYPE_,_NOM_,_BASE_)        \
  _TYPE_::_TYPE_() { }                                                        \
  Implemente_instanciable_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_)

#define Implemente_instanciable(_TYPE_,_NOM_,_BASE_)                        \
  _TYPE_::_TYPE_() { }                                                        \
  _TYPE_::~_TYPE_() { }                                                        \
  Implemente_instanciable_sans_constructeur_ni_destructeur(_TYPE_,_NOM_,_BASE_)


/////////////////////////////////////////
/////// TEMPLATE VERSION for 32/64 bits !!! Oh yes baby!
/////////////////////////////////////////

// The declare remain identical but we create a new macro so that bin/KSH/mk_Instanciable spots it correctly:
#define Declare_instanciable_sans_constructeur_ni_destructeur_32_64(_TYPE_) Declare_instanciable_sans_constructeur_ni_destructeur(_TYPE_)
#define Declare_instanciable_sans_constructeur_32_64(_TYPE_) Declare_instanciable_sans_constructeur(_TYPE_)
#define Declare_instanciable_sans_destructeur_32_64(_TYPE_) Declare_instanciable_sans_destructeur(_TYPE_)
#define Declare_instanciable_32_64(_TYPE_) Declare_instanciable(_TYPE_)

// Helper macro for info_obj static variable definition:
#if INT_is_64_ == 2
# define info_obj_def_macro_inst(_NOM_, _TYPE_) \
  template <> const Type_info _TYPE_<int>::info_obj(_NOM_, _TYPE_<int>::cree_instance, 1, name2(_TYPE_,bases)<int> );  \
  template <> const Type_info _TYPE_<trustIdType>::info_obj(_NOM_ "_64", _TYPE_<trustIdType>::cree_instance, 1, name2(_TYPE_, bases)<trustIdType> );
#else
# define info_obj_def_macro_inst(_NOM_, _TYPE_) \
  template <> const Type_info _TYPE_<int>::info_obj(_NOM_, _TYPE_<int>::cree_instance, 1, name2(_TYPE_,bases)<int> );
#endif

#ifdef LATATOOLS
#define Implemente_instanciable_sans_constructeur_ni_destructeur_32_64(_TYPE_,_NOM_,_BASE_)
#else
// The implemente are of course different:
#define Implemente_instanciable_sans_constructeur_ni_destructeur_32_64(_TYPE_,_NOM_,_BASE_) \
                                                                                              \
  template <typename _T_> const Type_info* name2(_TYPE_,bases)[1] = { &(_BASE_::info_obj) };     \
                                                                                                  \
  info_obj_def_macro_inst(_NOM_, _TYPE_)                               \
                                                                        \
  template <typename _T_> int _TYPE_<_T_>::duplique()  const                                        \
  {                                                                        \
    _TYPE_<_T_>* xxx = new _TYPE_<_T_>(*this);                                        \
    if(!xxx){Cerr << "Not enough memory " << finl; Process::exit();}                \
    return xxx->numero();                                                \
  }                                                                        \
  template <typename _T_> _TYPE_<_T_>& _TYPE_<_T_>::self_cast( Objet_U& r)   {                                \
    return ref_cast_non_const(_TYPE_<_T_>,r); /* _non_const important sinon  recursion dans ref_cast */ \
  }                                                                        \
  template <typename _T_> const _TYPE_<_T_>& _TYPE_<_T_>::self_cast(const Objet_U& r)   {                        \
    return ref_cast_non_const(_TYPE_<_T_>,r); /* _non_const important sinon  recursion dans ref_cast */ \
  }                                                                        \
  template <typename _T_> Objet_U* _TYPE_<_T_>::cree_instance()                                        \
  {  _TYPE_<_T_>* xxx = new _TYPE_<_T_>();                                        \
    if(!xxx){Cerr << "Not enough memory " << finl; Process::exit();}                \
    return xxx ;                                                        \
  }                                                                        \
                                                                        \
  template <typename _T_> const Type_info*  _TYPE_<_T_>::get_info() const {                                \
    return &info_obj;                                                        \
  }                                                                        \
                                                                        \
   template <typename _T_> const Type_info*  _TYPE_<_T_>::info() {                                        \
    return &info_obj;                                                        \
  }                                                                        \
                                                                        \
  template <typename _T_> unsigned _TYPE_<_T_>::taille_memoire() const {                                \
    return sizeof(_TYPE_<_T_>);                                                \
  }                                                                      \
  class __dummy__

#endif  // LATATOOLS

#define Implemente_instanciable_sans_constructeur_32_64(_TYPE_,_NOM_,_BASE_)        \
  template <typename _T_> _TYPE_<_T_>::~_TYPE_() { }                                                        \
  Implemente_instanciable_sans_constructeur_ni_destructeur_32_64(_TYPE_,_NOM_,_BASE_)

#define Implemente_instanciable_sans_destructeur_32_64(_TYPE_,_NOM_,_BASE_)        \
  template <typename _T_> _TYPE_<_T_>::_TYPE_() { }                                                        \
  Implemente_instanciable_sans_constructeur_ni_destructeur_32_64(_TYPE_,_NOM_,_BASE_)

#define Implemente_instanciable_32_64(_TYPE_,_NOM_,_BASE_)                        \
  template <typename _T_> _TYPE_<_T_>::_TYPE_() { }                                                        \
  template <typename _T_> _TYPE_<_T_>::~_TYPE_() { }                                                        \
  Implemente_instanciable_sans_constructeur_ni_destructeur_32_64(_TYPE_,_NOM_,_BASE_)
