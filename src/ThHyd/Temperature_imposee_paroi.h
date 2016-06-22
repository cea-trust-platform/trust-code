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
// File:        Temperature_imposee_paroi.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Temperature_imposee_paroi_included
#define Temperature_imposee_paroi_included

#include <Scalaire_impose_paroi.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Temperature_imposee_paroi
//    Impose la temperature de la paroi dans une equation de type
//    Convection_Diffusion_Temperature
// .SECTION voir aussi
//    Dirichlet Convection_Diffusion_Temperature
//////////////////////////////////////////////////////////////////////////////
class Temperature_imposee_paroi  : public Scalaire_impose_paroi
{

  Declare_instanciable(Temperature_imposee_paroi);

public :

  virtual int compatible_avec_eqn(const Equation_base& ) const;
};


#endif
