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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Source_dep_inco_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Source_dep_inco_base_included
#define Source_dep_inco_base_included

#include <Source_base.h>

//
//  DESCRIPTION class Source_dep_inco_base
//     Les sources heritant de cette clase doivent coder ajouter_
//  de facon a permettre d'impliciter le terme en diffusion implicite.
//  On cherche a obtenir la source sous la forme f(un)*uk
//  ajouter(resu)=ajouter_(inco,resu), et dans ajouter_ on utilise
// inco pour uk , inconnue.valeurs() pour Un, voir Perte_Charge_Singuliere_VDF_Face.cpp

class Source_dep_inco_base : public Source_base
{

  Declare_base(Source_dep_inco_base);

public:

  DoubleTab& ajouter(DoubleTab& resu) const override ;
  virtual DoubleTab& ajouter_(const DoubleTab& inco, DoubleTab& resu ) const =0;
  DoubleTab& calculer(DoubleTab& resu) const override ;

};

#endif
