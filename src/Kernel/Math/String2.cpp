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
// File:        String2.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#include <String2.h>
#include <Nom.h>
#include <string.h>

String2::String2()
{
  s = new char[2];
  s[0]='0';
  s[1]='\0';
}

String2::String2(const char *ss)
{
  s = new char[strlen(ss)+1];
  strcpy(s,ss);
  convertit_en_majuscule(s);
}

String2::~String2()
{
  delete[] s;
}

String2::String2(const String2& ss)
{
  s = new char[strlen(ss.s)+1];
  strcpy(s,ss.s);
}

String2::String2(String2* ss)
{
  s = new char[strlen(ss->s)+1];
  strcpy(s,ss->s);
}


String2* String2::substring(int i0, int i1)
{
  char* ss = new char[i1-i0+1];
  for (int i=i0; i<i1; i++)
    ss[i]=s[i];

  String2* str=new String2(ss);
  delete[] ss;
  return str;
}

/*
  int String2::compare(const char * ss) const
  {
  }
*/
Sortie& operator<<(Sortie&   os,const String2& c)
{
  return os << c.s;
}
