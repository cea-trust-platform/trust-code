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
#ifndef Variable_included
#define Variable_included

#include <string>
#include <algorithm>
#include <Nom.h>

class Variable
{
private:
  double value;
  std::string *s;



public:
  Variable()
  {
    value =0.;
    s = new std::string("X");
  }

  ~Variable()
  {
    delete s;
  }
  Variable(Variable& v)
  {
    value =v.value;
    s = new std::string(*v.s);
  }

  Variable(std::string& ss)
  {
    value = 0.;
    s = new std::string(ss);
  }

  Variable(const char* ss)
  {
    value = 0.;
    s = new std::string(ss);
    // int length = s->size();
    std::transform(s->begin(), s->end(), s->begin(), ::toupper);
  }


  double getValue()
  {
    return value;
  }
  void setValue(double x)
  {
    value = x;
  }
  std::string& getString()
  {
    return *s;
  }

  /*void setValue(std::string s)
    {
    System.out.println(s);
    Parser p = new Parser(s);
    p.parseString();
    value = p.eval();
    }
  */
};


#endif
