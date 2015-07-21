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
// File:        String2.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////
#ifndef String2_included
#define String2_included
#include <string.h>
class Sortie;

// .DESCRIPTION        : class String2
//  Cette classe encapsule une chaine de caracteres simple.
//  Contrairement a "Nom", elle ne derive pas de Objet_U.
//  (Utilisee par la classe Parser)

class String2
{
public:
  String2();
  String2(const char*);
  String2(const String2& ss);
  String2(String2* ss);
  ~String2();
  inline int         length()
  {
    return strlen(s);
  };
  inline char*       toChar()
  {
    return s;
  };
  inline const char* toChar() const
  {
    return s;
  };
  inline int         compare(const String2& ss) const
  {
    return strcmp(s,ss.s);
  };
  inline int         compare(const String2* ss) const
  {
    return strcmp(s,ss->s);
  };
  //  inline int         compare(const char * ss) const        { return strcasecmp(s,ss);};
  String2*     substring(int i0, int i1) ;

  friend Sortie& operator<<(Sortie&   os,const String2& c);
  friend class StringTokenizer;

private:
  char* s;
};

Sortie& operator<<(Sortie&   os,const String2& c);
#endif
