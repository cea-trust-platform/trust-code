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

#ifndef StdFunction_included
#define StdFunction_included

#include <UnaryFunction.h>
#include <cmath>


/*! @brief StdFunction
 *
 * @sa =
 */

// default functions for parser
double drand(double x) ;

class StdFunction : public UnaryFunction
{
  Declare_base(StdFunction);
public:
  const Nom& getName() const override
  {
    return que_suis_je();
  }
};

class Sin : public StdFunction
{
  Declare_instanciable(Sin);
public:
  double eval(double x) override
  {
    return sin(x);
  }
};

class Asin : public StdFunction
{
  Declare_instanciable(Asin);
public:
  double eval(double x) override
  {
    return asin(x);
  }
};

class Cos : public StdFunction
{
  Declare_instanciable(Cos);
public:
  double eval(double x) override
  {
    return cos(x);
  }
};

class Acos : public StdFunction
{
  Declare_instanciable(Acos);
public:
  double eval(double x) override
  {
    return acos(x);
  }
};

class Tan : public StdFunction
{
  Declare_instanciable(Tan);
public:
  double eval(double x) override
  {
    return tan(x);
  }
};

class Atan : public StdFunction
{
  Declare_instanciable(Atan);
public:
  double eval(double x) override
  {
    return atan(x);
  }
};

class Ln : public StdFunction
{
  Declare_instanciable(Ln);
public:
  double eval(double x) override
  {
    if (x<=0)
      {
        Cerr << "x=" << x << " for LN(x) function used." << finl << "Check your data file." << finl;
        exit();
      }
    return log(x);
  }
};

class Exp : public StdFunction
{
  Declare_instanciable(Exp);
public:
  double eval(double x) override
  {
    return exp(x);
  }
};

class Sqrt : public StdFunction
{
  Declare_instanciable(Sqrt);
public:
  double eval(double x) override
  {
    if (x<0)
      {
        Cerr << "x=" << x << " for SQRT(x) function used." << finl << "Check your data file." << finl;
        exit();
      }
    return sqrt(x);
  }
};

class Int : public StdFunction
{
  Declare_instanciable(Int);
public:
  double eval(double x) override
  {
    return (int) x;
  }
};

class Erf : public StdFunction
{
  Declare_instanciable(Erf);
public:
  double eval(double x) override
#ifndef MICROSOFT
  {
    return erf(x);
  }
#else
  {
    Cerr << "erf(x) fonction not implemented on Windows version." << finl;
    exit();
    return eval(x);
  }
#endif
};

class Rnd : public StdFunction
{
  Declare_instanciable(Rnd);
public:
  double eval(double x) override
  {
    return drand(x);
  }
};

class Cosh : public StdFunction
{
  Declare_instanciable(Cosh);
public:
  double eval(double x) override
  {
    return cosh(x);
  }
};

class Sinh : public StdFunction
{
  Declare_instanciable(Sinh);
public:
  double eval(double x) override
  {
    return sinh(x);
  }
};

class Tanh : public StdFunction
{
  Declare_instanciable(Tanh);
public:
  double eval(double x) override
  {
    return tanh(x);
  }
};

class Atanh : public StdFunction
{
  Declare_instanciable(Atanh);
public:
  double eval(double x) override
  {
    return  atanh(x);
  }
};

class Not : public StdFunction
{
  Declare_instanciable(Not);
public:
  double eval(double x) override
  {
    if (x==0) return 1 ;
    else return 0;
  }
};
class Abs : public StdFunction
{
  Declare_instanciable(Abs);
public:
  double eval(double x) override
  {
    return std::fabs(x);
  }
};

class Sgn : public StdFunction
{
  Declare_instanciable(Sgn);
public:
  double eval(double x) override
  {
    return (x > 0) - (x < 0);
  }
};

#endif
