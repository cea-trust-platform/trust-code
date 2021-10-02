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
// File:        StiffenedGas.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <StiffenedGas.h>

Implemente_instanciable(StiffenedGas, "StiffenedGas", Fluide_reel_base);

Sortie& StiffenedGas::printOn(Sortie& os) const
{
  return os;
}

Entree& StiffenedGas::readOn(Entree& is)
{
  Fluide_reel_base::readOn(is);
  return is;
}

void StiffenedGas::set_param(Param& param)
{
  param.ajouter("gamma",&gamma_);
  param.ajouter("pinf",&pinf_);
  param.ajouter("mu",&mu__);
  param.ajouter("lambda",&lambda__);
}


double StiffenedGas::rho_(const double T, const double P) const
{
  return (P + pinf_) / R_ / T;
}

double StiffenedGas::dT_rho_(const double T, const double P) const
{
  return -(P + pinf_) / R_ / T / T;
}

double StiffenedGas::dP_rho_(const double T, const double P) const
{
  return 1.0 / R_ / T;
}

double StiffenedGas::h_(const double T, const double P) const
{
  return cp_(T, P) * T;
}

double StiffenedGas::dT_h_(const double T, const double P) const
{
  return cp_(T, P);
}

double StiffenedGas::dP_h_(const double T, const double P) const
{
  return 0;
}

double StiffenedGas::cp_(const double T, const double P) const
{
  return gamma_ * R_ / (gamma_ - 1.0);
}

double StiffenedGas::mu_(const double T, const double P) const
{
  return mu__;
}

double StiffenedGas::lambda_(const double T, const double P) const
{
  return lambda__;
}

double StiffenedGas::beta_(const double T, const double P) const
{
  return 1.0 / T;
}
