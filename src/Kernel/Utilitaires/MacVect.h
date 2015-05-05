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
// File:        MacVect.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MacVECT_H
#define MacVECT_H

#include <Nom.h>
#include <assert.h>
#include <DerOu_.h>

#define declare_vect(_TYPE_)                                                \
  class VECT(_TYPE_) : public Objet_U                                        \
  {                                                                        \
    Declare_instanciable_sans_constructeur_ni_destructeur(VECT(_TYPE_)); \
                                                                        \
  public :                                                                \
    VECT(_TYPE_)();                                                        \
    inline int size() const;                                                \
    VECT(_TYPE_)(int );                                                \
    inline const _TYPE_& operator[](int) const;                        \
    inline const _TYPE_& operator()(int i) const { return operator[](i); }; \
    inline _TYPE_& operator[](int);                                        \
    inline _TYPE_& operator()(int i) { return operator[](i); }; \
    VECT(_TYPE_)(const VECT(_TYPE_)&);                                        \
    virtual ~VECT(_TYPE_)();                                                \
    VECT(_TYPE_)& operator=(const VECT(_TYPE_)& );                        \
    int search(const _TYPE_&) const;                                        \
    int contient_(const char* const ch) const;                        \
    int rang(const char* const ch) const;                                \
    void dimensionner(int);                                                \
    void dimensionner_force(int);                                        \
    void reset();                                                        \
    VECT(_TYPE_)& add(const _TYPE_&);                                        \
    VECT(_TYPE_)& add(const VECT(_TYPE_)&);                                \
    Entree& lit(Entree& );                                                \
  protected :                                                                \
    int sz;                                                                \
    DerObjU* data;                                                        \
                                                                        \
  };                                                                        \
  /* Size of the VECT */                                         \
  inline int VECT(_TYPE_)::size() const {                                \
    return sz;                                                                \
  }                                                                        \
  /* Returns the ith VECT element */ \
  inline const _TYPE_& VECT(_TYPE_)::operator[](int i) const {        \
    assert( (i>=0) && (i<sz) );                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return (const _TYPE_&)data[i].valeur();                                \
  }                                                                        \
  /* Returns the ith VECT element */ \
  inline _TYPE_& VECT(_TYPE_)::operator[](int i) {                        \
    assert( (i>=0) && (i<sz) );                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return (_TYPE_&)data[i].valeur();                                        \
  }

#define implemente_vect(_TYPE_)                                                \
  Implemente_instanciable_sans_constructeur_ni_destructeur(VECT(_TYPE_),string_macro_trio("VECT",VECT(_TYPE_)),Objet_U); \
  Sortie& VECT(_TYPE_)::printOn(Sortie& s) const {                        \
    int i=sz;                                                        \
    s<<sz<<space;                                                        \
    for(i=0;i<sz;i++)                                                        \
      s<<data[i].valeur()<<space;                                        \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return s<<finl;                                                        \
  }                                                                        \
  Entree& VECT(_TYPE_)::readOn(Entree& s) {                                \
    int i;                                                                \
    s >> i;                                                                \
    dimensionner(i);                                                        \
    for(i=0;i<sz;i++)                                                        \
      s>>data[i].valeur();                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return s;                                                                \
  }                                                                        \
  Entree& VECT(_TYPE_)::lit(Entree& s) {                                \
    int i;                                                                \
    s >> i;                                                                \
    dimensionner_force(i);                                                \
    for(i=0;i<sz;i++)                                                        \
      s>>data[i].valeur();                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return s;                                                                \
  }                                                                        \
  /* VECT constructors */ \
  VECT(_TYPE_)::VECT(_TYPE_)() : sz(0), data(0){}                        \
  VECT(_TYPE_)::VECT(_TYPE_)(int i): sz(i){                                \
    if(i==0) data=0;                                                        \
    else  { data=0;dimensionner_force(i);                                \
      if(!data) {                                                        \
        Cerr << "Allocation is not possible : " << finl;                \
        exit();                                                                \
      }                                                                        \
    }                                                                        \
    assert(sz>=0); if(sz) assert(data!=0);                                \
  }                                                                        \
  /* Constructor by copy for VECT */ \
  VECT(_TYPE_)::VECT(_TYPE_)(const VECT(_TYPE_)& vect)                        \
    : Objet_U(vect),sz(vect.sz), data(new DerObjU[vect.sz]) {                \
    if(!data) {                                                                \
      Cerr << "Allocation is not possible : " << finl;                        \
      exit();                                                                \
    }                                                                        \
    int i=sz;                                                        \
    if(i==0) {                                                                \
      delete[] data;                                                        \
      data=0;                                                                \
    }                                                                        \
    else while(i--) {                                                        \
      (data[i])=(vect[i]);  }                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
  }                                                                        \
  /* VECT destructor */ \
  VECT(_TYPE_)::~VECT(_TYPE_)(){                                        \
    reset();                                                                \
    assert (data==0) ;                                                        \
  }                                                                        \
  /* VECT operator= */ \
  VECT(_TYPE_)& VECT(_TYPE_)::operator=(const VECT(_TYPE_)& vect)        \
  {                                                                        \
    if (this==&(VECT(_TYPE_)&)vect) return *this;                        \
    if(data) {                                                                \
      reset();                                                                \
    }                                                                        \
    sz=vect.sz;                                                                \
    if(sz) {                                                                \
      data = new DerObjU[sz];                                                \
      if(!data) {                                                        \
        Cerr << "Allocation is not possible : " << finl;                \
        exit();                                                                \
      }                                                                        \
      int i=sz;                                                        \
      while(i--) {                                                        \
        data[i]=vect[i]; }                                                \
    }                                                                        \
    else data=0;                                                        \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return *this;                                                        \
  }                                                                        \
  /* Returns 1 if the VECT contains the string, else 0 */ \
  int VECT(_TYPE_)::contient_(const char* const ch) const                \
  {                                                                        \
    return (rang(ch)!=-1);                                                \
  }                                                                    \
  /* Returns the VECT position number of a string (-1 if not found) */                  \
  int VECT(_TYPE_)::rang(const char* const ch) const                \
  {                                                                        \
    Nom nom(ch);                                                        \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    int i=sz;                                                        \
    while(i--)                                                                \
      if ((data[i].valeur()).le_nom()==nom) {                                \
        return i;                                                        \
      }                                                                        \
    return -1;                                                                \
  }                                                                        \
  /* Returns the VECT position number of an element (-1 if not found) */ \
  int VECT(_TYPE_)::search(const _TYPE_& t ) const                        \
  {                                                                        \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    int i=sz;                                                        \
    while(i--)                                                                \
      if (((_TYPE_&) data[i].valeur())==t) {                                \
        return i;                                                        \
      }                                                                        \
    return -1;                                                                \
  }                                                                        \
  /* Size a VECT to i elements */ \
  void VECT(_TYPE_)::dimensionner(int i) {                                \
    if(sz==i) return;                                                        \
    if(sz!=0)                                                                \
      {                                                                        \
        Cerr << "WARNING : dimensionner method of a VECT(_TYPE_)" << finl; \
        Cerr << "old size : " << sz << finl;                                \
        Cerr << "new size : " << i << finl;                                \
      }                                                                        \
    assert(data==0);                                                        \
    sz=i;                                                                \
    data = new DerObjU[i];                                                \
    if(!data) {                                                                \
      Cerr << "Allocation is not possible. " << finl;                        \
      exit();                                                                \
    }                                                                        \
    for (int ii=0;ii<sz;ii++)                                                \
      data[ii]=(const _TYPE_&) _TYPE_();                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
  }                                                                        \
  /* Empty then size a VECT to i elements */ \
  void VECT(_TYPE_)::dimensionner_force(int i) {                        \
    if(data) {                                                                \
      reset();                                                                \
    }                                                                        \
    sz=i;                                                                \
    data = new DerObjU[i];                                                \
    if(!data) {                                                        \
      Cerr << "Allocation is not possible. " << finl;                        \
      exit();                                                                \
    }                                                                        \
    for (int ii=0;ii<sz;ii++)                                                \
      data[ii]=(const _TYPE_&)_TYPE_();                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
  }                                                                        \
  /* Empty a VECT (delete data and reset size to 0) */ \
  void VECT(_TYPE_)::reset() {                                                \
    if (data) {                                                                \
      delete [] data; data=0;                                                \
      sz=0;                                                                \
    }                                                                        \
    assert(sz==0);                                                        \
  }                                                                        \
  /* Add a new element to the VECT */ \
  VECT(_TYPE_)& VECT(_TYPE_)::add(const _TYPE_& data_to_add) {                \
    DerObjU* tempo;                                                        \
    int new_size=sz+1;                                                        \
    tempo = new DerObjU[new_size];                                        \
    for(int i=0;i<sz;i++)                                                \
      (tempo[i]).deplace(data[i]);                                        \
    if (data) delete [] data;                                                \
    data=tempo; sz=new_size;                                                \
    data[sz-1]= data_to_add;                                                \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return *this;                                                        \
  }                                                                        \
  /* Append a VECT to a VECT */ \
  VECT(_TYPE_)& VECT(_TYPE_)::add(const VECT(_TYPE_)& data_to_add) {        \
    int sz_data_to_add = data_to_add.size();                                \
    DerObjU* tempo;                                                        \
    int new_size=sz+sz_data_to_add;                                        \
    tempo = new DerObjU[new_size];                                        \
    for(int i=0;i<sz;i++)                                                \
      (tempo[i]).deplace( data[i]);                                        \
    if (data) delete [] data;                                                \
    data=tempo; sz=new_size;                                                \
    for(int j=0;j<sz_data_to_add;j++) {                                \
      data[sz-sz_data_to_add+j]= data_to_add[j]; }                        \
    assert(sz>=0); if(sz) assert(data!=0);                                \
    return *this;                                                        \
  }

#define Declare_vect(_TYPE_)                        \
  declare_vect(_TYPE_) class __dummy__
#define Implemente_vect(_TYPE_)                        \
  implemente_vect(_TYPE_) class __dummy__

#endif
