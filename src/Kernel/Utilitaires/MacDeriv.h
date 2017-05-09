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
// File:        MacDeriv.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////
// Modif B. Mathieu 07/2004
// Voir commentaires dans MacRef.h

#ifndef MacDeriv_H
#define MacDeriv_H

#include <Deriv_.h>

// B. Mathieu, 08_2004 : si on met les methodes valeur() et operator->
//  en inline, en debug, la taille de l'executable augmente de 10Mo,
//  et la taille de la librairie aussi !
//  Donc je mets ces methodes en inline uniquement en mode optimise.
#ifndef VERSION_DEBUG_ALLEGEE
#define _INLINE_DERIV_ inline
#else
#define _INLINE_DERIV_
#endif

#define _deriv_methodes_inline_(_TYPE_)                                        \
  _INLINE_DERIV_ const _TYPE_& DERIV(_TYPE_)::valeur() const                \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return *pointeur_;                                                        \
  }                                                                        \
                                                                        \
  _INLINE_DERIV_ _TYPE_& DERIV(_TYPE_)::valeur()                        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return *pointeur_;                                                        \
  }                                                                        \
                                                                        \
  _INLINE_DERIV_ const _TYPE_* DERIV(_TYPE_)::operator->() const        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return pointeur_;                                                        \
  }                                                                        \
  _INLINE_DERIV_ _TYPE_* DERIV(_TYPE_)::operator->()                        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return pointeur_;                                                        \
  }

#ifndef VERSION_DEBUG_ALLEGEE
#define _declare_deriv_methodes_inline_(_TYPE_) _deriv_methodes_inline_(_TYPE_)
#define _implemente_deriv_methodes_inline_(_TYPE_)
#else
#define _declare_deriv_methodes_inline_(_TYPE_)
#define _implemente_deriv_methodes_inline_(_TYPE_) _deriv_methodes_inline_(_TYPE_)
#endif


#define declare_deriv(_TYPE_)                                        \
  class DERIV(_TYPE_) : public Deriv_                                \
  {                                                                \
    Declare_instanciable_sans_constructeur(DERIV(_TYPE_));        \
      public:                                                        \
    DERIV(_TYPE_)() ;                                                \
   /*  DERIV(_TYPE_)(const _TYPE_& ) ;       */                         \
    DERIV(_TYPE_)(const DERIV(_TYPE_)& ) ;                       \
    _INLINE_DERIV_ const _TYPE_& valeur() const;                \
    _INLINE_DERIV_ _TYPE_& valeur() ;                                \
    _INLINE_DERIV_ const  _TYPE_* operator ->() const;                \
    _INLINE_DERIV_ _TYPE_* operator ->() ;                        \
    const DERIV(_TYPE_)& operator=(const _TYPE_& );                \
    const DERIV(_TYPE_)& operator=(const DERIV(_TYPE_)& );        \
    operator const _TYPE_&() const;                                \
    operator _TYPE_&();                                                \
    /* methode reimplementee de O_U_Ptr : */                        \
    const Type_info & get_info_ptr() const;                        \
  protected:                                                        \
    void set_O_U_Ptr(Objet_U *);                                \
  private:                                                        \
    _TYPE_ * pointeur_;                                                \
  };                                                                \
  _declare_deriv_methodes_inline_(_TYPE_)

#define implemente_deriv(_TYPE_)                                        \
  Implemente_instanciable_sans_constructeur_ni_destructeur(DERIV(_TYPE_),string_macro_trio("Deriv",DERIV(_TYPE_)),Deriv_); \
    Sortie& DERIV(_TYPE_)::printOn(Sortie& s) const {                        \
    return Deriv_::printOn(s);                                                \
  }                                                                        \
  Entree& DERIV(_TYPE_)::readOn(Entree& s) {                                \
    return Deriv_::readOn(s);                                                \
  }                                                                        \
  DERIV(_TYPE_)::DERIV(_TYPE_)() :  Deriv_(),                                \
                                    pointeur_(0)                        \
  {                                                                        \
  }                                                                        \
  DERIV(_TYPE_)::~DERIV(_TYPE_)()                                        \
  {                                                                        \
    detach();                                                                \
  }                                                                        \
 /*  DERIV(_TYPE_)::DERIV(_TYPE_)(const _TYPE_& t):Deriv_() {   */             \
 /*    pointeur_=0;                 */                                       \
/*     recopie(t);                 */                                              \
 /*  } */                                                                        \
  DERIV(_TYPE_)::DERIV(_TYPE_)(const DERIV(_TYPE_)& t) : Deriv_() {        \
    pointeur_=0;                                                        \
    if (t.non_nul())                                                        \
      recopie(t.valeur());                                                \
  }                                                                        \
  const DERIV(_TYPE_)& DERIV(_TYPE_)::operator=(const _TYPE_& t){        \
    if (pointeur_!=(&t)) {                                                \
      detach();                                                                \
      recopie(t);                                                        \
    }                                                                        \
    return *this;                                                        \
  }                                                                        \
  const DERIV(_TYPE_)& DERIV(_TYPE_)::operator=(const DERIV(_TYPE_)& t){ \
    if (pointeur_!=t.pointeur_) {                                        \
      detach();                                                                \
      if (t.non_nul())                                                        \
        recopie(t.valeur());                                                \
    }                                                                        \
    return *this;                                                        \
  }                                                                        \
  DERIV(_TYPE_)::operator _TYPE_&()                                        \
  {                                                                        \
    return valeur();                                                        \
  }                                                                        \
  DERIV(_TYPE_)::operator const _TYPE_&() const                                \
  {                                                                        \
    return valeur();                                                        \
  }                                                                        \
  /* Description: renvoie le type de base accepte par la ref */                \
  const Type_info & DERIV(_TYPE_)::get_info_ptr() const                        \
  {                                                                        \
    const Type_info * type_info = _TYPE_::info();                        \
    return *type_info;                                                        \
  }                                                                        \
  void DERIV(_TYPE_)::set_O_U_Ptr(Objet_U * objet)                        \
  {                                                                        \
    O_U_Ptr::set_O_U_Ptr(objet);                                        \
    if (objet)                                                                \
      /* Attention: cette conversion de type est non triviale */        \
      /* si le _TYPE_ est issu d'un heritage multiple. */                \
      pointeur_ = (_TYPE_*) objet;                                        \
    else                                                                \
      pointeur_ = (_TYPE_*) 0;                                                \
  }
_implemente_deriv_methodes_inline_(_TYPE_)


#define Declare_deriv(_TYPE_)                        \
  declare_deriv(_TYPE_) class __dummy__
#define Implemente_deriv(_TYPE_)                \
  implemente_deriv(_TYPE_) class __dummy__

#endif
