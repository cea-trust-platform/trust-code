/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Fluide_stiffened_gas.h>

Implemente_instanciable_sans_constructeur(Fluide_stiffened_gas, "StiffenedGas", Fluide_reel_base);
// XD fluide_stiffened_gas fluide_reel_base fluide_stiffened_gas -1 Class for Stiffened Gas
// XD attr gamma floattant gamma 1 Heat capacity ratio (Cp/Cv)
// XD attr pinf floattant pinf 1 Stiffened gas pressure constant (if set to zero, the state law becomes identical to that of perfect gases)
// XD attr mu floattant mu 1 Dynamic viscosity
// XD attr lambda floattant lambda 1 Thermal conductivity
// XD attr Cv floattant Cv 1 Thermal capacity at constant volume
// XD attr q floattant q 1 Reference energy
// XD attr q_prim floattant q_prim 1 Model constant

Fluide_stiffened_gas::Fluide_stiffened_gas() : pinf_(0.), Cv_(-1.), q_(0.), q_prim_(0.), gamma_(1.4), R_(8.31446261815324), mu__(0.), lambda__(0.) { }

Sortie& Fluide_stiffened_gas::printOn(Sortie& os) const { return os; }

Entree& Fluide_stiffened_gas::readOn(Entree& is)
{
  Fluide_reel_base::readOn(is);
  if (Cv_ == -1.) Cv_ = R_ / (gamma_ - 1.0);
  return is;
}

void Fluide_stiffened_gas::set_param(Param& param)
{
  Fluide_reel_base::set_param(param);
  param.ajouter("gamma",&gamma_);
  param.ajouter("pinf",&pinf_);
  param.ajouter("mu",&mu__);
  param.ajouter("lambda",&lambda__);
  param.ajouter("Cv",&Cv_);
  param.ajouter("q",&q_);
  param.ajouter("q_prim",&q_prim_);
}

#define ind std::distance(res.begin(), &val)

/* Lois en T */
void Fluide_stiffened_gas::rho_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = (P[ind] + pinf_) / (gamma_ - 1.0) / (T[ind * ncomp + id] + 273.15) / Cv_;
}

void Fluide_stiffened_gas::dP_rho_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = 1.0 / (gamma_ - 1.0) / Cv_ / (T[ind * ncomp + id] + 273.15);
}

void Fluide_stiffened_gas::dT_rho_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = -(P[ind] + pinf_) / (gamma_ - 1.0) / Cv_ / (T[ind * ncomp + id] + 273.15) / (T[ind * ncomp + id] + 273.15);
}

void Fluide_stiffened_gas::h_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val =  gamma_ * Cv_  * (T[ind * ncomp + id] + 273.15) + q_;
}

void Fluide_stiffened_gas::dP_h_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = 0.;
}

void Fluide_stiffened_gas::dT_h_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  return cp_(T,P,res,ncomp,id);
}

void Fluide_stiffened_gas::cp_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = gamma_ * Cv_;
}

void Fluide_stiffened_gas::beta_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = 1.0 / (T[ind * ncomp + id] + 273.15);
}

void Fluide_stiffened_gas::mu_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = mu__;
}

void Fluide_stiffened_gas::lambda_(const SpanD T, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  for (auto& val : res) val = lambda__;
}

/* Lois en h */
void Fluide_stiffened_gas::rho_h_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )res.size());
  for (auto& val : res) val = gamma_ * (P[ind] + pinf_) / (gamma_ - 1.0) / H[ind * ncomp + id];
}

void Fluide_stiffened_gas::dP_rho_h_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )res.size());
  for (auto& val : res) val = gamma_ / (gamma_ - 1.0) / H[ind * ncomp + id];
}

void Fluide_stiffened_gas::dh_rho_h_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )res.size());
  for (auto& val : res) val = - gamma_ * (P[ind] + pinf_) / (gamma_ - 1.0) / H[ind * ncomp + id] / H[ind * ncomp + id];
}

void Fluide_stiffened_gas::T_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )res.size());
  for (auto& val : res) val = H[ind * ncomp + id] / gamma_ / Cv_ - 273.15;
}

void Fluide_stiffened_gas::dP_T_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )res.size());
  for (auto& val : res) val = 0.;
}

void Fluide_stiffened_gas::dh_T_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )res.size());
  for (auto& val : res) val = 1. /  gamma_ / Cv_;
}

void Fluide_stiffened_gas::cp_h_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  cp_(H, P, res, ncomp, id);
}

void Fluide_stiffened_gas::beta_h_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )res.size());
  for (auto& val : res) val = 1.0 / (H[ind * ncomp + id] / (gamma_ * Cv_));
}

void Fluide_stiffened_gas::mu_h_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  mu_(H, P, res, ncomp, id);
}

void Fluide_stiffened_gas::lambda_h_(const SpanD H, const SpanD P, SpanD res, int ncomp, int id) const
{
  lambda_(H, P, res, ncomp, id);
}

#undef ind
