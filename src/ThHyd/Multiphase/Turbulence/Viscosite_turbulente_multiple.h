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
// File:        Viscosite_turbulente_multiple.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Viscosite_turbulente_multiple_included
#define Viscosite_turbulente_multiple_included
#include <Viscosite_turbulente_base.h>
#include <Correlation.h>
#include <map>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Viscosite_turbulente_multiple
//    Classe qui peut contenir plusieurs viscosites turbulentes pour faire de la turbulence monophasique + diphasique
//    En pratique, la viscosite turbulente n'est que monophasique et l'operateur de diffusion reste identique
//    La classe Source_BIF_PolyMAC_V2 appelle reynolds_stress_BIF et entre les termes de divergence du tenseur de Reynolds
//      en explicite dans la QDM
//
//////////////////////////////////////////////////////////////////////////////

class Viscosite_turbulente_multiple : public Viscosite_turbulente_base
{
  Declare_instanciable(Viscosite_turbulente_multiple);
public:
  void eddy_viscosity(DoubleTab& nu_t) const override;
  void reynolds_stress(DoubleTab& R_ij) const override;
  void k_over_eps(DoubleTab& k_sur_eps) const override;
  int gradu_required() const override {  return 1; };
  void eps(DoubleTab& eps) const override;

  void reynolds_stress_BIF(DoubleTab& R_ij) const;

private:
  std::map<std::string, Correlation> viscs_turbs;

};

#endif
