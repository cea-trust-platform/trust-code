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
// File:        ptrParam.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////
#include <ptrParam.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(ptrParam,"objet_interne_ptrPram",Objet_U);
Implemente_vect(ptrParam);

Entree& ptrParam::readOn(Entree& is)
{
  return Objet_U::readOn(is);
}
Sortie& ptrParam::printOn(Sortie& os) const
{
  return Objet_U::printOn(os);
}

ptrParam::ptrParam() :param_(0)
{
}
ptrParam::~ptrParam()
{
  delete param_;
  param_=0;
}

void ptrParam::create(const char*  name)
{
  param_=new Param(name);
}
Param& ptrParam::valeur()
{
  if (param_==0)
    {
      Cerr<<"param not defined" <<finl;
      exit();
    }
  return *param_;
}
const Param& ptrParam::valeur() const
{
  if (param_==0)
    {
      Cerr<<"param not defined" <<finl;
      exit();
    }
  return *param_;
}

ptrParam::ptrParam(const ptrParam& p):Objet_U(p)
{
  if (p.non_nul())
    abort();
  param_=0;
}
const ptrParam& ptrParam::operator=(const ptrParam& p)
{
  abort();
  return (*this);
}
