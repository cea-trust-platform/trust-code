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

#ifndef RefObjet_U_H
#define RefObjet_U_H

#include <Ref.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class RefObjU
//     Pour creer une reference sur n'importe quel Objet_U
// .SECTION voir aussi
//   Objet_U
//////////////////////////////////////////////////////////////////////////////

// Cette classe est quasiment identique a REF(Objet_U),
// sauf qu'elle ne contient pas les operateurs de conversion
// de REF(Objet_U) en Objet_U.

#define declare_ref_Objet_U(_TYPE_)                                        \
  class REF(_TYPE_) : public Ref_                                        \
  {                                                                        \
    Declare_instanciable_sans_constructeur(REF(_TYPE_));                        \
      public:                                                                \
    REF(_TYPE_)() ;                                                        \
    REF(_TYPE_)(const _TYPE_& ) ;                                        \
    REF(_TYPE_)(const REF(_TYPE_)& ) ;                                        \
    inline const _TYPE_& valeur() const;                                \
    inline _TYPE_& valeur() ;                                                \
    inline const _TYPE_* operator ->() const;                                \
    inline _TYPE_* operator ->() ;                                        \
    const REF(_TYPE_)& operator=(const _TYPE_& );                        \
    const REF(_TYPE_)& operator=(const REF(_TYPE_)& );                        \
    /* Ici on a retire les operateurs de conversion implicite */        \
    /* methode reimplementee de O_U_Ptr : */                                \
    const Type_info & get_info_ptr() const override;                                \
  protected:                                                                \
    void set_O_U_Ptr(Objet_U *) override;                                        \
  private:                                                                \
    _TYPE_ * pointeur_;                                                        \
  };                                                                        \
  int operator ==(const REF(_TYPE_)&, const REF(_TYPE_)& );                \
  inline const _TYPE_& REF(_TYPE_)::valeur() const                        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return *pointeur_;                                                        \
  }                                                                        \
                                                                        \
  inline _TYPE_& REF(_TYPE_)::valeur()                                        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return *pointeur_;                                                        \
  }                                                                        \
                                                                        \
  inline const _TYPE_* REF(_TYPE_)::operator->() const                        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return pointeur_;                                                        \
  }                                                                        \
                                                                        \
  inline _TYPE_* REF(_TYPE_)::operator->()                                \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert(get_O_U_Ptr_check() || 1);                                        \
    return pointeur_;                                                        \
  }

declare_ref_Objet_U(Objet_U)

typedef REF(Objet_U) RefObjU;

#endif
