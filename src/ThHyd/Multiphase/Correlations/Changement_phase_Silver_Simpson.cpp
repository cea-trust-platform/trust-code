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
// File:        Changement_phase_Silver_Simpson.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Changement_phase_Silver_Simpson.h>
#include <Pb_Multiphase.h>
#include <TRUSTTab.h>
#include <cmath>

Implemente_instanciable(Changement_phase_Silver_Simpson, "Changement_phase_Silver_Simpson", Changement_phase_base);

Sortie& Changement_phase_Silver_Simpson::printOn(Sortie& os) const
{
  return os;
}

Entree& Changement_phase_Silver_Simpson::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("lambda_e", &lambda_ec[0]);
  param.ajouter("lambda_c", &lambda_ec[1]);
  param.ajouter("alpha_min", &alpha_min);
  param.ajouter("M", &M, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  return is;
}

double Changement_phase_Silver_Simpson::calculer(int k, int l, const double& dh, const double *alpha, const double *T, const double p, const double *nv,
                                                 const double *lambda, const double *mu, const double *rho, const double *Cp, const Saturation_base& sat,
                                                 DoubleTab& dT_G, DoubleTab& da_G, double& dp_G) const
{
  double T0 = 273.15,
         var_ak = std::max(alpha[k], alpha_min), var_al = std::pow(std::max(alpha[l], alpha_min), 1.5), var_a = var_ak * var_al, //partie variable en alpha
         var_T = sat.Psat(T[k]) / sqrt(T[k] + T0) - p / sqrt(T[l] + T0), //partie variable en (T, p)
         fac = lambda_ec[var_T < 0] * 4 / dh * sqrt(M / (2 * M_PI * 8.314)); //partie constante -> G = fac * var
  dT_G = 0, da_G = 0;
  dT_G(k) = fac * var_a * (sat.dT_Psat(T[k]) - 0.5 * sat.Psat(T[k]) / (T[k] + T0)) / sqrt(T[k] + T0);
  dT_G(l) = fac * var_a * 0.5 * p * std::pow(T[l] + T0, -1.5);
  da_G(k) = alpha[k] > alpha_min ? fac * var_T * var_al : 0;
  da_G(l) = alpha[l] > alpha_min ? fac * var_T * var_ak * 1.5 * sqrt(alpha[l]) : 0;
  dp_G = - fac * var_a / sqrt(T[l] + T0);
  return fac * var_a * var_T;
}
