/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <StdFunction.h>
#include <stdlib.h>

double drand(double x)
{
  return x*drand48();
}

Implemente_base(StdFunction,"StdFunction",UnaryFunction);
Sortie& StdFunction::printOn(Sortie& os) const
{
  return os;
}
Entree& StdFunction::readOn(Entree& is)
{
  return is;
}



Implemente_instanciable(Sin,"Sin",StdFunction);
Sortie& Sin::printOn(Sortie& os) const
{
  return os;
}
Entree& Sin::readOn(Entree& is)
{
  return is;
}
Implemente_instanciable(Asin,"Asin",StdFunction);
Sortie& Asin::printOn(Sortie& os) const
{
  return os;
}
Entree& Asin::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Cos,"Cos",StdFunction);
Sortie& Cos::printOn(Sortie& os) const
{
  return os;
}
Entree& Cos::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Acos,"Acos",StdFunction);
Sortie& Acos::printOn(Sortie& os) const
{
  return os;
}
Entree& Acos::readOn(Entree& is)
{
  return is;
}
Implemente_instanciable(Tan,"Tan",StdFunction);
Sortie& Tan::printOn(Sortie& os) const
{
  return os;
}
Entree& Tan::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Atan,"Atan",StdFunction);
Sortie& Atan::printOn(Sortie& os) const
{
  return os;
}
Entree& Atan::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Ln,"Ln",StdFunction);
Sortie& Ln::printOn(Sortie& os) const
{
  return os;
}
Entree& Ln::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Exp,"Exp",StdFunction);
Sortie& Exp::printOn(Sortie& os) const
{
  return os;
}
Entree& Exp::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Sqrt,"Sqrt",StdFunction);
Sortie& Sqrt::printOn(Sortie& os) const
{
  return os;
}
Entree& Sqrt::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Int,"Int",StdFunction);
Sortie& Int::printOn(Sortie& os) const
{
  return os;
}
Entree& Int::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Erf,"Erf",StdFunction);
Sortie& Erf::printOn(Sortie& os) const
{
  return os;
}
Entree& Erf::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Rnd,"Rnd",StdFunction);
Sortie& Rnd::printOn(Sortie& os) const
{
  return os;
}
Entree& Rnd::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Cosh,"Cosh",StdFunction);
Sortie& Cosh::printOn(Sortie& os) const
{
  return os;
}
Entree& Cosh::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Sinh,"Sinh",StdFunction);
Sortie& Sinh::printOn(Sortie& os) const
{
  return os;
}
Entree& Sinh::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Tanh,"Tanh",StdFunction);
Sortie& Tanh::printOn(Sortie& os) const
{
  return os;
}
Entree& Tanh::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Atanh,"Atanh",StdFunction);
Sortie& Atanh::printOn(Sortie& os) const
{
  return os;
}
Entree& Atanh::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Not,"Not",StdFunction);
Sortie& Not::printOn(Sortie& os) const
{
  return os;
}
Entree& Not::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Abs,"Abs",StdFunction);
Sortie& Abs::printOn(Sortie& os) const
{
  return os;
}
Entree& Abs::readOn(Entree& is)
{
  return is;
}

Implemente_instanciable(Sgn,"Sgn",StdFunction);
Sortie& Sgn::printOn(Sortie& os) const
{
  return os;
}
Entree& Sgn::readOn(Entree& is)
{
  return is;
}



