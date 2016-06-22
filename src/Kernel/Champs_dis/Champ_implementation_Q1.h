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
// File:        Champ_implementation_Q1.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_implementation_Q1_included
#define Champ_implementation_Q1_included

#include <Champ_implementation_sommet_base.h>

/////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION        : class Champ_implementation_Q1
//
// Decrire ici la classe Champ_implementation_Q1
//
//////////////////////////////////////////////////////////////////////////////

class Champ_implementation_Q1 : public Champ_implementation_sommet_base
{

public :
  virtual       Champ_base& le_champ(void)       =0;
  virtual const Champ_base& le_champ(void) const =0;

public :
  inline virtual ~Champ_implementation_Q1() { };

protected :
  virtual  double form_function(const ArrOfDouble& position, int cell, int ddl) const;
  virtual void value_interpolation(const ArrOfDouble& position, int cell, const DoubleTab& values, ArrOfDouble& resu,int ncomp=-1) const;


};

#endif /* Champ_implementation_Q1_inclus */
