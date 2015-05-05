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
// File:        MacList.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MacList_H
#define MacList_H

#include <liste.h>
#include <Motcle.h>

#define declare_liste(_TYPE_)                                                \
  class LIST(_TYPE_) : public liste {                                        \
    Declare_instanciable(LIST(_TYPE_));                                        \
  public :                                                                \
    inline LIST(_TYPE_)(const _TYPE_& t);                                \
    inline LIST(_TYPE_)(const LIST(_TYPE_)& t);                                \
    inline _TYPE_& operator[](int) ;                                        \
    inline const _TYPE_& operator[](int) const;                        \
    inline _TYPE_& operator[](const Nom&) ;                                \
    inline const _TYPE_& operator[](const Nom&) const;                        \
    inline _TYPE_& operator()(int i);                                \
    inline const _TYPE_& operator()(int i) const;                        \
    inline _TYPE_& operator()(const Nom& n);                                \
    inline const _TYPE_& operator()(const Nom& n) const;                \
    inline _TYPE_& valeur();                                                \
    inline const _TYPE_& valeur() const;                                \
    inline LIST(_TYPE_)& operator=(const _TYPE_& t);                        \
    inline _TYPE_* operator ->() ;                                        \
    inline LIST(_TYPE_)& operator=(const LIST(_TYPE_)& );                \
    inline LIST(_TYPE_)& add(const LIST(_TYPE_)& ) ;                        \
    inline _TYPE_& add(const _TYPE_& ) ;                                \
    inline LIST(_TYPE_)& inserer(const LIST(_TYPE_)& ) ;                \
    inline _TYPE_& inserer(const _TYPE_& ) ;                                \
    inline void supprimer() ;                                                \
    inline LIST(_TYPE_)& operator +=(const LIST(_TYPE_)&);                \
    inline LIST(_TYPE_)& operator +=(const _TYPE_&);                        \
    inline _TYPE_& add_if_not(const _TYPE_& ) ;                                \
    inline _TYPE_& add_if_not(const char* const ) ;                        \
    inline LIST(_TYPE_)& search(const _TYPE_&) const;                        \
    inline LIST(_TYPE_)& search(const char* const) const;                \
    inline LIST(_TYPE_)& suivant() ;                                        \
    inline const LIST(_TYPE_)& suivant() const ;                        \
    inline const LIST(_TYPE_)& dernier() const ;                        \
    inline LIST(_TYPE_)& dernier();                                        \
    inline LIST(_TYPE_)& operator -=(const _TYPE_&);                        \
    int est_egal_a(const Objet_U& ) const;                                \
    int contient(const _TYPE_&) const;                                \
    int rang(const _TYPE_&) const;                                        \
    void suppr(const _TYPE_&);                                                \
  protected :                                                                \
    inline int est_egal_a(const LIST(_TYPE_)& ) const;                \
  };                                                                        \
  class LIST_CURSEUR(_TYPE_) : public liste_curseur{                        \
  public :                                                                \
    inline LIST_CURSEUR(_TYPE_)(LIST(_TYPE_)& a_list);                        \
    inline _TYPE_& valeur();                                                \
    inline const _TYPE_& valeur() const;                                \
    inline _TYPE_* operator ->();                                        \
    inline void operator=( LIST(_TYPE_)& a_list);                                \
    inline const LIST(_TYPE_)& list() const;                                \
    inline LIST(_TYPE_)& list();                                        \
  };                                                                        \
  class CONST_LIST_CURSEUR(_TYPE_) : public liste_curseur{                \
  public :                                                                \
    inline CONST_LIST_CURSEUR(_TYPE_)(const LIST(_TYPE_)& a_list);        \
    inline const _TYPE_& valeur() const;                                \
    inline  const _TYPE_* operator ->();                                \
    inline void operator=(const LIST(_TYPE_)& a_list);                        \
    inline const LIST(_TYPE_)& list() const;                                \
  };                                                                        \
  inline _TYPE_& LIST(_TYPE_)::operator[](int i){                        \
    return (_TYPE_&) (liste::operator[](i));                                \
  }                                                                        \
  inline const _TYPE_& LIST(_TYPE_)::operator[](int i) const{        \
    return (_TYPE_&) (liste::operator[](i));                                \
  }                                                                        \
  inline _TYPE_& LIST(_TYPE_)::operator[](const Nom& nom){                \
    return (_TYPE_&) (liste::operator[](nom));                                \
  }                                                                        \
  inline const _TYPE_& LIST(_TYPE_)::operator[](const Nom& nom) const{        \
    return (_TYPE_&) (liste::operator[](nom));                                \
  }                                                                        \
  inline LIST(_TYPE_)::LIST(_TYPE_)(const _TYPE_& t) : liste(t){}        \
  inline LIST(_TYPE_)::LIST(_TYPE_)(const LIST(_TYPE_)& t) : liste(t){} \
  inline _TYPE_& LIST(_TYPE_)::operator()(int i){                        \
    return operator[](i);                                                \
  }                                                                        \
  inline const _TYPE_& LIST(_TYPE_)::operator()(int i) const{        \
    return operator[](i);                                                \
  }                                                                        \
  inline _TYPE_& LIST(_TYPE_)::operator()(const Nom& n){                \
    return operator[](n);                                                \
  }                                                                        \
  inline const _TYPE_& LIST(_TYPE_)::operator()(const Nom& n) const{        \
    return operator[](n);                                                \
  }                                                                        \
  inline _TYPE_& LIST(_TYPE_)::valeur(){                                \
    return (_TYPE_&) (liste::valeur());                                        \
  }                                                                        \
  inline const _TYPE_& LIST(_TYPE_)::valeur() const{                        \
    return (_TYPE_&) (liste::valeur());                                        \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::operator=(const _TYPE_& t){        \
    return (LIST(_TYPE_)&) (liste::operator=(t));                        \
  }                                                                        \
  inline _TYPE_* LIST(_TYPE_)::operator ->() {                                \
    return (_TYPE_*)(liste::operator->());                                \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::operator=(const LIST(_TYPE_)& a_list) { \
    return (LIST(_TYPE_)&) (liste::operator=(a_list));                        \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::add(const LIST(_TYPE_)& a_list) {        \
    return (LIST(_TYPE_)&) (liste::add(a_list));                                \
  }                                                                        \
  inline _TYPE_& LIST(_TYPE_)::add(const _TYPE_& t) {                        \
    return (_TYPE_&) (liste::add((const Objet_U &)t));                        \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::inserer(const LIST(_TYPE_)& a_list) { \
    return (LIST(_TYPE_)&) (liste::inserer(a_list));                        \
  }                                                                        \
  inline void LIST(_TYPE_)::supprimer() {                                \
    liste::supprimer();                                                        \
  }                                                                        \
  inline _TYPE_& LIST(_TYPE_)::inserer(const _TYPE_& t) {                \
    return (_TYPE_&) (liste::inserer(t));                                \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::operator +=(const LIST(_TYPE_)& a_list) { \
    return (LIST(_TYPE_)&) (liste::operator+=(a_list));                        \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::operator +=(const _TYPE_& t){        \
    return (LIST(_TYPE_)&) (liste::operator+=(t));                        \
  }                                                                        \
  inline _TYPE_& LIST(_TYPE_)::add_if_not(const char* const t) {        \
    Nom nom(t);                                                                \
    if( est_vide()) {                                                        \
      _TYPE_ to_add; to_add.nommer(nom);                                \
      return add(to_add);                                                \
    }                                                                        \
    {                                                                        \
      liste_curseur curseur=*this;                                        \
      while(curseur){                                                        \
        if(curseur.valeur().le_nom()==nom)                                \
          return (_TYPE_&)(curseur.valeur());                                \
        if(curseur.list().est_dernier())                                \
          {                                                                \
            _TYPE_ to_add; to_add.nommer(nom);                                \
            return add(to_add);                                                \
          }                                                                \
        ++curseur;                                                        \
      }                                                                        \
    }                                                                        \
    Cerr << "Error in a list for add_if_not " << finl;                        \
    return (_TYPE_&)(valeur());                                                \
  }                                                                        \
  inline _TYPE_& LIST(_TYPE_)::add_if_not(const _TYPE_& t) {                \
    return (_TYPE_&) (liste::add_if_not(t));                                \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::search(const _TYPE_& t) const{        \
    return (LIST(_TYPE_)&) (liste::search(t));                                \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::search(const char* const c) const{        \
    return (LIST(_TYPE_)&) (liste::search(c));                                \
  }                                                                        \
  inline const LIST(_TYPE_)& LIST(_TYPE_)::suivant() const {                \
    return (LIST(_TYPE_)&) (liste::suivant());                                \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::suivant() {                        \
    return (LIST(_TYPE_)&) (liste::suivant());                                \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::dernier() {                        \
    return (LIST(_TYPE_)&) (liste::dernier());                                \
  }                                                                        \
  inline LIST(_TYPE_)& LIST(_TYPE_)::operator -=(const _TYPE_& t) {        \
    return (LIST(_TYPE_)&) (liste::operator-=(t));                        \
  }                                                                        \
  inline int LIST(_TYPE_)::contient(const _TYPE_& t) const{                \
    return liste::contient(t);                                                \
  }                                                                        \
  inline int LIST(_TYPE_)::rang(const _TYPE_& t) const{                \
    return liste::rang(t);                                                \
  }                                                                        \
  inline void LIST(_TYPE_)::suppr(const _TYPE_& t){                        \
    liste::suppr(t);                                                        \
  }                                                                        \
  inline LIST_CURSEUR(_TYPE_)::LIST_CURSEUR(_TYPE_)(LIST(_TYPE_)& a_list):liste_curseur(a_list){ \
  }                                                                        \
  inline _TYPE_& LIST_CURSEUR(_TYPE_)::valeur()                                \
  { return (_TYPE_&) (liste_curseur::valeur());}                        \
  inline const _TYPE_& LIST_CURSEUR(_TYPE_)::valeur() const                \
  { return (_TYPE_&) (liste_curseur::valeur());}                        \
  inline _TYPE_* LIST_CURSEUR(_TYPE_)::operator ->()                        \
  { return (_TYPE_*) (liste_curseur::operator ->());}                        \
  inline void LIST_CURSEUR(_TYPE_)::operator=( LIST(_TYPE_)& a_list){        \
    liste_curseur::operator=(a_list);                                        \
  }                                                                        \
  inline const LIST(_TYPE_)& LIST_CURSEUR(_TYPE_)::list() const{        \
    return (LIST(_TYPE_)&) (liste_curseur::list());                        \
  }                                                                        \
  inline LIST(_TYPE_)& LIST_CURSEUR(_TYPE_)::list() {                        \
    return (LIST(_TYPE_)&) (liste_curseur::list());                        \
  }                                                                        \
  inline CONST_LIST_CURSEUR(_TYPE_)::CONST_LIST_CURSEUR(_TYPE_)(const LIST(_TYPE_)& a_list):liste_curseur(a_list){ \
  }                                                                        \
  inline const _TYPE_& CONST_LIST_CURSEUR(_TYPE_)::valeur() const        \
  { return (const _TYPE_&) (liste_curseur::valeur());}                        \
  inline const _TYPE_* CONST_LIST_CURSEUR(_TYPE_)::operator ->()        \
  { return (const _TYPE_*) (liste_curseur::operator ->());}                \
  inline void CONST_LIST_CURSEUR(_TYPE_)::operator=(const LIST(_TYPE_)& a_list){ \
    liste_curseur::operator=(a_list);                                        \
  }                                                                        \
  inline const LIST(_TYPE_)& CONST_LIST_CURSEUR(_TYPE_)::list() const{        \
    return (LIST(_TYPE_)&) (liste_curseur::list());                        \
  }

#define implemente_liste(_TYPE_)                                        \
  Implemente_instanciable(LIST(_TYPE_),string_macro_trio("List",LIST(_TYPE_)),liste); \
  Sortie& LIST(_TYPE_)::printOn(Sortie& os) const {                        \
    return liste::printOn(os);                                                \
  }                                                                        \
  Entree& LIST(_TYPE_)::readOn(Entree& is) {				\
    if (!est_vide()) {							\
      if(0) Cerr<<"list not empty, append to it"<<finl;			\
    }									\
    Nom accouverte="{";							\
    Nom accfermee="}";                                                        \
    Nom virgule=",";                                                        \
    Motcle nom_;                                                        \
    is >> nom_;                                                                \
    if(nom_==(const char*)"vide")                                        \
      return is;                                                        \
    if (nom_!=accouverte)                                                \
      {                                                                        \
        Cerr << "Error while reading a list." << finl;                        \
        Cerr << "One expected an opened bracket { to start." << finl;        \
        exit();                                                                \
      }                                                                        \
    _TYPE_ t;                                                                \
    while(1){                                                                \
      is >> add(t);                                                        \
      is >> nom_;                                                        \
      if(nom_==accfermee)                                                \
        return is;                                                        \
      if(nom_!=virgule) {                                                \
        Cerr << nom_ << " one expected a ',' or a '}'" << finl;                \
        exit();                                                                \
      }                                                                        \
    }                                                                        \
  }                                                                        \
  int LIST(_TYPE_)::est_egal_a(const Objet_U& ) const{                \
    return 0;                                                                \
  }
#define Declare_liste(_TYPE_)                        \
  declare_liste(_TYPE_) class __dummy__
#define Implemente_liste(_TYPE_)                \
  implemente_liste(_TYPE_) class __dummy__


#endif
