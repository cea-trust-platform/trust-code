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
// File:        MacRef.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////
// Modification B.Mathieu 07/2004 :
//  But :
//   Avoir un pointeur direct vers l'objet pour que .valeur() soit
//   le plus rapide possible (donc inline)
//  Probleme :
//   Si on met un pointeur comme ceci dans la classe O_Uptr
//     Objet_U * ptr;
//   alors il faut que la classe _TYPE_ soit definie pour pouvoir calculer
//   la conversion de type suivante dans la methode "valeur()" :
//     _TYPE_* typed_ptr = (_TYPE_*) ptr;
//   (probleme avec l'heritage multiple).
//   Par exemple, ceci ne fonctionne pas :
//    class Type_base; // forward declaration
//    class REF(_TYPE_) : public Ref_ {
//      // Probleme : cette conversion de type de fonctionne pas :
//      // (warning sur HP, bug avec gcc)
//      inline valeur() { return * ((_TYPE_*) ptr); };
//    }
//   Donc, soit il faut laisser tomber la declaration forward,
//   soit il ne faut pas mettre "valeur()" en inline.
//   Pour avoir les deux il faut que le pointeur soit du type _TYPE_*.

#ifndef MacRef_H
#define MacRef_H

#include <Ref_.h>

// B. Mathieu, 08_2004 : si on met les methodes valeur() et operator->
//  en inline, en debug, la taille de l'executable augmente de 10Mo,
//  et la taille de la librairie aussi !
//  Donc je mets ces methodes en inline uniquement en mode optimise.
#ifndef VERSION_DEBUG_ALLEGEE
#define _INLINE_REF_ inline
#else
#define _INLINE_REF_
#endif

#define _ref_methodes_inline_(_TYPE_)                                \
  _INLINE_REF_ const _TYPE_& REF(_TYPE_)::valeur() const        \
  {                                                                \
    assert(pointeur_ != 0);                                        \
    assert(get_O_U_Ptr_check() || 1);                                \
    return *pointeur_;                                                \
  }                                                                \
                                                                \
  _INLINE_REF_ _TYPE_& REF(_TYPE_)::valeur()                        \
  {                                                                \
    assert(pointeur_ != 0);                                        \
    assert(get_O_U_Ptr_check() || 1);                                \
    return *pointeur_;                                                \
  }                                                                \
                                                                \
  _INLINE_REF_ const _TYPE_* REF(_TYPE_)::operator->() const        \
  {                                                                \
    assert(pointeur_ != 0);                                        \
    assert(get_O_U_Ptr_check() || 1);                                \
    return pointeur_;                                                \
  }                                                                \
  _INLINE_REF_ _TYPE_* REF(_TYPE_)::operator->()                \
  {                                                                \
    assert(pointeur_ != 0);                                        \
    assert(get_O_U_Ptr_check() || 1);                                \
    return pointeur_;                                                \
  }

#ifndef VERSION_DEBUG_ALLEGEE
#define _declare_ref_methodes_inline_(_TYPE_) _ref_methodes_inline_(_TYPE_)
#define _implemente_ref_methodes_inline_(_TYPE_)
#else
#define _declare_ref_methodes_inline_(_TYPE_)
#define _implemente_ref_methodes_inline_(_TYPE_) _ref_methodes_inline_(_TYPE_)
#endif

#define declare_ref(_TYPE_)                                        \
  class REF(_TYPE_) : public Ref_                                \
  {                                                                \
    Declare_instanciable_sans_constructeur(REF(_TYPE_));                \
      public:                                                        \
    REF(_TYPE_)();                                                \
    REF(_TYPE_)(const _TYPE_& );                                \
    REF(_TYPE_)(const REF(_TYPE_)& );                                \
    _INLINE_REF_ const _TYPE_& valeur() const;                        \
    _INLINE_REF_ _TYPE_&       valeur() ;                        \
    _INLINE_REF_ const _TYPE_*       operator ->() const;        \
    _INLINE_REF_ _TYPE_*       operator ->() ;                        \
    const REF(_TYPE_)& operator=(const _TYPE_& );                \
    const REF(_TYPE_)& operator=(const REF(_TYPE_)& );                \
    operator const _TYPE_&() const;                                \
    operator _TYPE_&();                                                \
    /* methode reimplementee de O_U_Ptr : */                        \
    const Type_info & get_info_ptr() const;                        \
  protected:                                                        \
    void set_O_U_Ptr(Objet_U *);                                \
  private:                                                        \
    /* Le pointeur doit absolument etre de type _TYPE_* */        \
    /* (voir commentaires ci-dessus) */                                \
    _TYPE_ * pointeur_;                                                \
  };                                                                \
  int operator ==(const REF(_TYPE_)&, const REF(_TYPE_)& );        \
  _declare_ref_methodes_inline_(_TYPE_)

#define implemente_ref(_TYPE_)                                                \
  Implemente_instanciable_sans_constructeur(REF(_TYPE_),string_macro_trio("Ref",REF(_TYPE_)),Ref_); \
    Sortie& REF(_TYPE_)::printOn(Sortie& s) const {                        \
    return Ref_::printOn(s);                                                \
  }                                                                        \
  Entree& REF(_TYPE_)::readOn(Entree& s) {                                \
    return Ref_::readOn(s);                                                \
  }                                                                        \
  REF(_TYPE_)::REF(_TYPE_)() : Ref_() ,                                        \
                               pointeur_(0)                                \
  {                                                                        \
  }                                                                        \
  REF(_TYPE_)::REF(_TYPE_)(const _TYPE_& t) :  Ref_()                        \
  {                                                                        \
    set_O_U_Ptr((_TYPE_ *) &t);                                                \
  }                                                                        \
  REF(_TYPE_)::REF(_TYPE_)(const REF(_TYPE_)& t) :  Ref_()                \
  {                                                                        \
    set_O_U_Ptr(t.pointeur_);                                                \
  }                                                                        \
  const REF(_TYPE_)& REF(_TYPE_)::operator=(const _TYPE_& t)                \
  {                                                                        \
    set_O_U_Ptr((_TYPE_ *) &t);                                                \
    return *this;                                                        \
  }                                                                        \
  const REF(_TYPE_)& REF(_TYPE_)::operator=(const REF(_TYPE_)& t)        \
  {                                                                        \
    set_O_U_Ptr(t.pointeur_);                                                \
    return *this;                                                        \
  }                                                                        \
  /* Le resultat de == est positif si r1 et r2 pointent */                \
  /* sur le meme objet (meme cle)                       */                \
  /* ou si les deux references sont nulles              */                \
  int operator ==(const REF(_TYPE_)& r1, const REF(_TYPE_)& r2)        \
  {                                                                        \
    if ( (!r1.non_nul()) && (!r2.non_nul()) )                                \
      return 1;                                                                \
    if (r1.valeur().numero() == r2.valeur().numero())                        \
      return 1;                                                                \
    return 0;                                                                \
  }                                                                        \
                                                                        \
  REF(_TYPE_)::operator const _TYPE_&() const                                \
  {                                                                        \
    return valeur();                                                        \
  }                                                                        \
  REF(_TYPE_)::operator _TYPE_&()                                        \
  {                                                                        \
    return valeur();                                                        \
  }                                                                        \
  /* Description: renvoie le type de base accepte par la ref */                \
  const Type_info & REF(_TYPE_)::get_info_ptr() const                        \
  {                                                                        \
    const Type_info * type_info = _TYPE_::info();                        \
    return *type_info;                                                        \
  }                                                                        \
  void REF(_TYPE_)::set_O_U_Ptr(Objet_U * objet)                        \
  {                                                                        \
    O_U_Ptr::set_O_U_Ptr(objet);                                        \
    if (objet)                                                                \
      /* Attention: cette conversion de type est non triviale */        \
      /* si le _TYPE_ est issu d'un heritage multiple. */                \
      pointeur_ = (_TYPE_*) objet;                                        \
    else                                                                \
      pointeur_ = (_TYPE_*) 0;                                                \
  }                                                                        \
                                                                        \
  _implemente_ref_methodes_inline_(_TYPE_)

#define Declare_ref(_TYPE_)                        \
  declare_ref(_TYPE_) class __dummy__
#define Implemente_ref(_TYPE_)                        \
  implemente_ref(_TYPE_) class __dummy__
#endif
