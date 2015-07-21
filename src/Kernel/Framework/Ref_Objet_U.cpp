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
// File:        Ref_Objet_U.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Ref_Objet_U.h>

// Cette classe est quasiment identique a REF(Objet_U),
// sauf qu'elle ne contient pas les operateurs de conversion
// de REF(Objet_U) en Objet_U.

#define implemente_ref_Objet_U(_TYPE_)                                        \
  Implemente_instanciable_sans_constructeur(REF(_TYPE_),"Ref",Ref_);        \
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
  const REF(_TYPE_)& REF(_TYPE_)::operator=(const _TYPE_& t){                \
    set_O_U_Ptr((_TYPE_ *) &t);                                                \
    return *this;                                                        \
  }                                                                        \
  const REF(_TYPE_)& REF(_TYPE_)::operator=(const REF(_TYPE_)& t){        \
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
  }
/* Le }\ fait planter le module perl de Doxygen, inutile on met } */
/* Ici on a retire les operateurs de conversion implicite */

implemente_ref_Objet_U(Objet_U)
