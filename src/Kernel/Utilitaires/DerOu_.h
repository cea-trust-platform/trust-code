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
// File:        DerOu_.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef DerOu_H
#define DerOu_H

#include <Deriv.h>
#include <Nom.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class DerObjU
//     Pour representer n'importe quel Objet_U
//     La methode fondamentale est typer.
// .SECTION voir aussi
//   Objet_U
//////////////////////////////////////////////////////////////////////////////

// B. Mathieu : cette classe est quasiment identique a DERIV(Objet_U),
// sauf qu'elle ne contient pas les operateurs de conversion
// de DERIV(Objet_U) en Objet_U.
// La macro est un copie-colle de MacDeriv.h
// Il existe 3 methodes supplementaires :
//   deplace(DERIV(_TYPE_)&)
//   reprendre
//   sauvegarder

#define declare_deriv_Objet_U(_TYPE_)                                        \
  class DERIV(_TYPE_) : public Deriv_                                        \
  {                                                                        \
    Declare_instanciable_sans_constructeur(DERIV(_TYPE_));                \
      public:                                                                \
    DERIV(_TYPE_)() ;                                                        \
    DERIV(_TYPE_)(const _TYPE_& ) ;                                        \
    DERIV(_TYPE_)(const DERIV(_TYPE_)& ) ;                                \
    inline const _TYPE_& valeur() const;                                \
    inline _TYPE_& valeur() ;                                                \
    inline _TYPE_* operator ->() const;                                        \
    const DERIV(_TYPE_)& operator=(const _TYPE_& );                        \
    const DERIV(_TYPE_)& operator=(const DERIV(_TYPE_)& );                \
    /* Ici on a retire les operateurs de conversion implicite */        \
    void   deplace(DERIV(_TYPE_)&);                                        \
    int reprendre(Entree &);                                                \
    int sauvegarder(Sortie &) const;                                        \
    /* methode reimplementee de O_U_Ptr : */                                \
    const Type_info & get_info_ptr() const;                                \
  protected:                                                                \
    void set_O_U_Ptr(Objet_U *);                                        \
  private:                                                                \
    _TYPE_ * pointeur_;                                                        \
  };                                                                        \
                                                                        \
  inline const _TYPE_& DERIV(_TYPE_)::valeur() const                        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert((_TYPE_*) get_O_U_Ptr_check() == pointeur_);                        \
    return *pointeur_;                                                        \
  }                                                                        \
                                                                        \
  inline _TYPE_& DERIV(_TYPE_)::valeur()                                \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert((_TYPE_*) get_O_U_Ptr_check() == pointeur_);                        \
    return *pointeur_;                                                        \
  }                                                                        \
                                                                        \
  inline _TYPE_* DERIV(_TYPE_)::operator->() const                        \
  {                                                                        \
    assert(pointeur_ != 0);                                                \
    assert((_TYPE_*) get_O_U_Ptr_check() == pointeur_);                        \
    return pointeur_;                                                        \
  }

declare_deriv_Objet_U(Objet_U)

typedef DERIV(Objet_U) DerObjU;

#endif
