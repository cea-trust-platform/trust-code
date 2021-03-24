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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Flux_interfacial_Coef_Constant.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Flux_interfacial_Coef_Constant.h>
Implemente_instanciable(Flux_interfacial_Coef_Constant, "Flux_interfacial_Coef_Constant", Flux_interfacial_base);

Sortie& Flux_interfacial_Coef_Constant::printOn(Sortie& os) const
{
  return os;
}

Entree& Flux_interfacial_Coef_Constant::readOn(Entree& is)
{
  hl = 1e4, hv = 1e3;
  Param param(que_suis_je());
  param.ajouter("hl", &hl);
  param.ajouter("hv", &hv);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Flux_interfacial_Coef_Constant::flux(const double al, const double ag, const double Tl, const double Tg, const double Ti, const double dP_Ti,
                                          double& Fl, double& Fg, double& dal_fl, double& dag_fl, double& dal_fg, double& dag_fg, double& dTl_Fl, double& dTg_Fg, double& dP_Fl, double& dP_Fg) const
{

  Fl = hl * (Ti - Tl);
  Fg = hv * (Ti - Tg);
  dTl_Fl = -hl;
  dTg_Fg = -hv;

  dP_Fl = hl * dP_Ti;
  dP_Fg = hv * dP_Ti;

}
