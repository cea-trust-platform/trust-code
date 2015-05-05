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
// File:        StringTokenizer.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
#ifndef StringTokenizer_included
#define StringTokenizer_included

#include <String2.h>


class StringTokenizer
{

public :

  // Les differents operateurs et separateurs
  static const int ADD;
  static const int SUBTRACT;
  static const int DIVIDE;
  static const int MULTIPLY;
  static const int POWER;
  static const int GRP;
  static const int ENDGRP;
  static const int LT;
  static const int GT;
  static const int LE;
  static const int GE;
  static const int MOD;
  static const int MAX;
  static const int MIN;
  static const int AND;
  static const int OR;
  static const int EQ;
  static const int NEQ;

  //nb de separateur
private:
  static const int nb_op;
  static const int nb_op_bis;
  static const char keyword_op[][10];
  static const char keyword_op_bis[][10];

public:

  // Le type d'un element detecte  : nb, chaine, fin de chaine
  static const int NUMBER;
  static const int STRING;
  static const int EOS;

  int type;

  StringTokenizer();
  StringTokenizer(String2& );
  StringTokenizer(String2,String2);
  ~StringTokenizer();

  int nextToken(void);

  int check_GRP();


  inline double getNValue()
  {
    return nval;
  }
  inline const String2& getSValue()
  {
    return *sval;
  }



private :

  double nval;
  String2* sval;
  String2* str;
  char *reste;


  char* find_sep(char* ch, int& type_sep, int& type_keyword);
  void init_keyword_op();
  char ** op_sep;

};



#endif
