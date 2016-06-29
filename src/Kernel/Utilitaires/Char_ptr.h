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
// File:        Char_ptr.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Char_ptr_included
#define Char_ptr_included

#include <string.h>

//////////////////////////////////////////////////////////////////////////////
// .NOM  Char_ptr
// .ENTETE  TRUST Utilitaires
// .LIBRAIRIE  libutil
// .FILE  Char_ptr.h
// .FILE  Char_ptr.cpp
//
// .DESCRIPTION
//     class Char_ptr
//     Une chaine de caractere pour nommer les objets de TRUST
// .SECTION voir aussi
//      Motcle
// .CONTRAINTES
// .INVARIANTS
// .HTML
// .EPS
//////////////////////////////////////////////////////////////////////////////

class Char_ptr
{
public:
  Char_ptr();                                         // construit "??"
  Char_ptr(const char* nom);
  Char_ptr(const Char_ptr&);
  virtual ~Char_ptr();
  void allocate(int n);
  operator char*() const;
  int longueur() const ;
  Char_ptr& operator=(const char*);
  Char_ptr& operator=(const Char_ptr&);
  inline char* getChar()
  {
    return nom_;
  }
  inline const char* getChar() const
  {
    return nom_;
  }

protected :
  char* nom_;
};

#endif // NOM_H
