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

#ifndef Transport_turbulent_base_included
#define Transport_turbulent_base_included
#include <TRUSTTab.h>
#include <Correlation_base.h>
#include <Convection_Diffusion_std.h>
#include <Viscosite_turbulente_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Transport_turbulent_base
//    correlations decrivant l'effet de la turbulence dans une autre equation (thermique, quantites turbulentes...)
//    Methodes implementees :
//    - dimension_min_nu() -> nombre de composantes minimal du tenseur de diffusion par composante (1, D, D^2)
//    - modifier_nu() -> ajoute au tenseur de diffusion de l'equation la contribution de la turbulence
//                       prend en argument la correlation de viscosite turbulente de l'operateur de diffusion de la QDM
//    - gradu_required() -> 1 si la correlation a besoin du tenseur grad u
//////////////////////////////////////////////////////////////////////////////

class Transport_turbulent_base : public Correlation_base
{
  Declare_base(Transport_turbulent_base);
public:
  virtual int dimension_min_nu() const = 0;
  virtual int gradu_required() const
  {
    return 0;
  };
  virtual void modifier_nu(const Convection_Diffusion_std& eq, const Viscosite_turbulente_base& visc_turb, DoubleTab& nu) const = 0;
};

#endif
