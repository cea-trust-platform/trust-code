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
// File:        Variable.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Variable_included
#define Variable_included

#include <String2.h>

class Variable
{
private:
  double value;
  String2 *s;



public:
  Variable()
  {
    value =0.;
    s = new String2("x");
  }

  ~Variable()
  {
    delete s;
  }
  Variable(Variable& v)
  {
    value =v.value;
    s = new String2(v.s);
  }

  Variable(String2& ss)
  {
    value = 0.;
    s = new String2(ss);
  }

  Variable(const char* ss)
  {
    value = 0.;
    s = new String2(ss);
  }


  double getValue()
  {
    return value;
  }
  void setValue(double x)
  {
    value = x;
  }
  String2& getString()
  {
    return *s;
  }

  /*void setValue(String2 s)
    {
    System.out.println(s);
    Parser p = new Parser(s);
    p.parseString();
    value = p.eval();
    }
  */
};


#endif
