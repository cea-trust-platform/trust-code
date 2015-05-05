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
// File:        DerOu_.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <DerOu_.h>

// B. Mathieu : cette classe est quasiment identique a DERIV(Objet_U),
// sauf qu'elle ne contient pas les operateurs de conversion
// de DERIV(Objet_U) en Objet_U.
// La macro est un copie-colle de MacDeriv.h
// La seule difference est l'absence des deux operateurs de conversion.
// et il existe une methode supplementaire : deplace(DERIV(_TYPE_)&)

#define implemente_deriv_Objet_U(_TYPE_)                                \
  Implemente_instanciable_sans_constructeur_ni_destructeur(DERIV(_TYPE_),"Deriv",Deriv_); \
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
  DERIV(_TYPE_)::DERIV(_TYPE_)(const _TYPE_& t) : Deriv_() {                \
    pointeur_=0;                                                        \
    recopie(t);                                                                \
  }                                                                        \
  DERIV(_TYPE_)::DERIV(_TYPE_)(const DERIV(_TYPE_)& t) : Deriv_() {        \
    pointeur_=0;                                                        \
    if (t.non_nul())                                                        \
      recopie(t.valeur());                                                \
  }                                                                        \
  const DERIV(_TYPE_)& DERIV(_TYPE_)::operator=(const _TYPE_& t){        \
    detach();                                                                \
    recopie(t);                                                                \
    return *this;                                                        \
  }                                                                        \
  const DERIV(_TYPE_)& DERIV(_TYPE_)::operator=(const DERIV(_TYPE_)& t){ \
    detach();                                                                \
    if (t.non_nul())                                                        \
      recopie(t.valeur());                                                \
    else                                                                \
      set_O_U_Ptr((Objet_U *) 0);                                        \
    return *this;                                                        \
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
  }                                                                        \
  /* Cette fonction detruit l'objet en reference s'il existe et */        \
  /* le remplace par l'objet deriv_obj.valeur() */                        \
  /* deriv_obj devient une reference nulle */                                \
  void DERIV(_TYPE_)::deplace(DERIV(_TYPE_)& deriv_obj)                        \
  {                                                                        \
    detach();                                                                \
    Objet_U & objet = deriv_obj.valeur();                                \
    set_O_U_Ptr(& objet);                                                \
    deriv_obj.set_O_U_Ptr((Objet_U*) 0);                                \
  }                                                                        \
  int DERIV(_TYPE_)::reprendre(Entree & is)                                \
  {                                                                        \
    return valeur().reprendre(is);                                        \
  }                                                                        \
  int DERIV(_TYPE_)::sauvegarder(Sortie & os) const                        \
  {                                                                        \
    return valeur().sauvegarder(os);                                        \
  }
/* Le }\ fait planter le module perl de Doxygen, inutile on met } */
/* Ici on a retire les operateurs de conversion implicite */

implemente_deriv_Objet_U(Objet_U)

