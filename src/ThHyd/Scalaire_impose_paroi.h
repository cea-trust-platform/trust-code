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
// File:        Scalaire_impose_paroi.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Scalaire_impose_paroi_included
#define Scalaire_impose_paroi_included

#include <Dirichlet.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Scalaire_impose_paroi
//    Impose un scalaire a la paroi dans une equation de type
//    Convection-Difussion d'un scalaire
// .SECTION voir aussi
//    Dirichlet
//////////////////////////////////////////////////////////////////////////////
class Scalaire_impose_paroi  : public Dirichlet
{

  Declare_instanciable(Scalaire_impose_paroi);

public :

  virtual int compatible_avec_eqn(const Equation_base& ) const;
  int compatible_avec_discr(const Discretisation_base& ) const;
};


#endif
