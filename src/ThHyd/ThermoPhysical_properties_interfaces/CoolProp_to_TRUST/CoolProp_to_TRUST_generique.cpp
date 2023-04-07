/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <CoolProp_to_TRUST_generique.h>

using namespace CoolProp;

// iterator index !
#define i_it std::distance(TT.begin(), &val)
#define bi_it std::distance(bTT.begin(), &bval)

void CoolProp_to_TRUST_generique::set_CoolProp_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_COOLPROP
  fluide = std::shared_ptr<AbstractState>(AbstractState::factory(std::string(model_name), std::string(fluid_name)));
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_single_property_T_(Loi_en_T enum_prop, const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) return tppi_get_single_property_T__(enum_prop, P, T, R);
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];
      return tppi_get_single_property_T__(enum_prop, P, TT, R);
    }
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_single_property_T__(Loi_en_T enum_prop, const SpanD P, const SpanD T, SpanD R) const
{
#ifdef HAS_COOLPROP
  const int sz = (int )R.size();
  // derivees qui manquent ...
  if (enum_prop == Loi_en_T::MU_DP || enum_prop == Loi_en_T::LAMBDA_DP || enum_prop == Loi_en_T::SIGMA_DP)
    return FD_derivative_pT(enum_prop,P, T, R);

  if (enum_prop == Loi_en_T::MU_DT || enum_prop == Loi_en_T::LAMBDA_DT || enum_prop == Loi_en_T::SIGMA_DT)
    return FD_derivative_pT(enum_prop,P, T, R, false /* wrt T */);

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units

      if (enum_prop == Loi_en_T::RHO) R[i] = fluide->rhomass();
      if (enum_prop == Loi_en_T::RHO_DP) R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
      if (enum_prop == Loi_en_T::RHO_DT) R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP);

      if (enum_prop == Loi_en_T::H) R[i] = fluide->hmass();
      if (enum_prop == Loi_en_T::H_DP) R[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
      if (enum_prop == Loi_en_T::H_DT) R[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP);

      if (enum_prop == Loi_en_T::CP) R[i] = fluide->cpmass();
      if (enum_prop == Loi_en_T::CP_DP) R[i] = fluide->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iT);
      if (enum_prop == Loi_en_T::CP_DT) R[i] = fluide->first_partial_deriv(CoolProp::iCpmass,  CoolProp::iT, CoolProp::iP);

      if (enum_prop == Loi_en_T::MU) R[i] = fluide->viscosity();
      if (enum_prop == Loi_en_T::LAMBDA) R[i] = fluide->conductivity();
      if (enum_prop == Loi_en_T::BETA) R[i] = fluide->isobaric_expansion_coefficient();
      if (enum_prop == Loi_en_T::SIGMA) R[i] = fluide->surface_tension();
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::FD_derivative_pT(Loi_en_T enum_prop, const SpanD P, const SpanD T, SpanD R, bool wrt_p) const
{
#ifdef HAS_COOLPROP
  const int sz = (int )R.size();
  for (int i = 0; i < sz; i++)
    {
      double plus_ = EPS, minus_ = EPS;

      wrt_p ? fluide->update(CoolProp::PT_INPUTS, P[i] * (1. + EPS), T[i]) : fluide->update(CoolProp::PT_INPUTS, P[i], T[i] * (1. + EPS));

      if (enum_prop ==  Loi_en_T::MU_DP || enum_prop == Loi_en_T::MU_DT) plus_ = fluide->viscosity();
      if (enum_prop == Loi_en_T::LAMBDA_DP || enum_prop == Loi_en_T::LAMBDA_DT)  plus_ = fluide->conductivity();
      if (enum_prop == Loi_en_T::SIGMA_DP || enum_prop == Loi_en_T::SIGMA_DT) plus_ = fluide->surface_tension();

      wrt_p ? fluide->update(CoolProp::PT_INPUTS, P[i] * (1. - EPS), T[i]) : fluide->update(CoolProp::PT_INPUTS, P[i], T[i] * (1. - EPS));

      if (enum_prop ==  Loi_en_T::MU_DP || enum_prop == Loi_en_T::MU_DT) minus_ = fluide->viscosity();
      if (enum_prop == Loi_en_T::LAMBDA_DP || enum_prop == Loi_en_T::LAMBDA_DT)  minus_ = fluide->conductivity();
      if (enum_prop == Loi_en_T::SIGMA_DP || enum_prop == Loi_en_T::SIGMA_DT) minus_ = fluide->surface_tension();

      R[i] = wrt_p ? ((plus_ - minus_) / ( 2 * EPS * P[i])) : ((plus_ - minus_) / ( 2 * EPS * T[i]));
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_single_property_h_(Loi_en_h enum_prop, const SpanD P, const SpanD H, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )R.size());
  if (ncomp == 1) return tppi_get_single_property_h__(enum_prop, P, H, R);
  else /* attention stride */
    Process::exit("No stride allowed for the moment for enthalpie calls ... use temperature or call 911 !");
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_single_property_h__(Loi_en_h enum_prop, const SpanD P, const SpanD H, SpanD R) const
{
#ifdef HAS_COOLPROP
  const int sz = (int )R.size();
  // derivees qui manquent ...
  if (enum_prop == Loi_en_h::MU_DP || enum_prop == Loi_en_h::LAMBDA_DP || enum_prop == Loi_en_h::SIGMA_DP)
    return FD_derivative_ph(enum_prop,P, H, R);

  if (enum_prop == Loi_en_h::MU_DH || enum_prop == Loi_en_h::LAMBDA_DH || enum_prop == Loi_en_h::SIGMA_DH)
    return FD_derivative_ph(enum_prop,P, H, R, false /* wrt H */);

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i]);  // SI units

      if (enum_prop == Loi_en_h::RHO) R[i] = fluide->rhomass();
      if (enum_prop == Loi_en_h::RHO_DP) R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iHmass);
      if (enum_prop == Loi_en_h::RHO_DH) R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iHmass, CoolProp::iP);

      if (enum_prop == Loi_en_h::T) R[i] = fluide->T();
      if (enum_prop == Loi_en_h::T_DP) R[i] = fluide->first_partial_deriv(CoolProp::iT, CoolProp::iP, CoolProp::iHmass);
      if (enum_prop == Loi_en_h::T_DH) R[i] = fluide->first_partial_deriv(CoolProp::iT, CoolProp::iHmass, CoolProp::iP);

      if (enum_prop == Loi_en_h::CP) R[i] = fluide->cpmass();
      if (enum_prop == Loi_en_h::CP_DP) R[i] = fluide->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iHmass);
      if (enum_prop == Loi_en_h::CP_DH) R[i] = fluide->first_partial_deriv(CoolProp::iCpmass,  CoolProp::iHmass, CoolProp::iP);

      if (enum_prop == Loi_en_h::MU) R[i] = fluide->viscosity();
      if (enum_prop == Loi_en_h::LAMBDA) R[i] = fluide->conductivity();
      if (enum_prop == Loi_en_h::BETA) R[i] = fluide->isobaric_expansion_coefficient();
      if (enum_prop == Loi_en_h::SIGMA) R[i] = fluide->surface_tension();
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::FD_derivative_ph(Loi_en_h enum_prop, const SpanD P, const SpanD H, SpanD R, bool wrt_p) const
{
#ifdef HAS_COOLPROP
  const int sz = (int )R.size();
  for (int i = 0; i < sz; i++)
    {
      double plus_ = EPS, minus_ = EPS;

      wrt_p ? fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i] * (1. + EPS)) : fluide->update(CoolProp::HmassP_INPUTS, H[i] * (1. + EPS), P[i]);

      if (enum_prop ==  Loi_en_h::MU_DP || enum_prop == Loi_en_h::MU_DH) plus_ = fluide->viscosity();
      if (enum_prop == Loi_en_h::LAMBDA_DP || enum_prop == Loi_en_h::LAMBDA_DH)  plus_ = fluide->conductivity();
      if (enum_prop == Loi_en_h::SIGMA_DP || enum_prop == Loi_en_h::SIGMA_DH) plus_ = fluide->surface_tension();

      wrt_p ? fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i] * (1. - EPS)) : fluide->update(CoolProp::HmassP_INPUTS, H[i] * (1. - EPS), P[i]);

      if (enum_prop ==  Loi_en_h::MU_DP || enum_prop == Loi_en_h::MU_DH) minus_ = fluide->viscosity();
      if (enum_prop == Loi_en_h::LAMBDA_DP || enum_prop == Loi_en_h::LAMBDA_DH)  minus_ = fluide->conductivity();
      if (enum_prop == Loi_en_h::SIGMA_DP || enum_prop == Loi_en_h::SIGMA_DH) minus_ = fluide->surface_tension();

      R[i] = wrt_p ? ((plus_ - minus_) / ( 2 * EPS * P[i])) : ((plus_ - minus_) / ( 2 * EPS * H[i]));
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

/*
 * ******************* *
 * Lois en temperature
 * ******************* *
 */
int CoolProp_to_TRUST_generique::tppi_get_rho_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::RHO, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_rho_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::RHO_DP, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_rho_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::RHO_DT, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_h_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::H, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_h_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::H_DP, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_h_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::H_DT, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_cp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::CP, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_cp_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::CP_DP, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_cp_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::CP_DT, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_mu_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::MU, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_mu_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::MU_DP, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_mu_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::MU_DT, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_lambda_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::LAMBDA, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_lambda_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::LAMBDA_DP, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_lambda_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::LAMBDA_DT, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::SIGMA, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_sigma_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::SIGMA_DP, P, T, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_sigma_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::SIGMA_DT, P, T, R, ncomp, ind);
}

// appel simple si besoin : cas incompressible
int CoolProp_to_TRUST_generique::tppi_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_T_(Loi_en_T::BETA, P, T, R, ncomp, ind);
}

/*
 * ***************** *
 * Lois en enthalpie
 * ***************** *
 */
int CoolProp_to_TRUST_generique::tppi_get_rho_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::RHO, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_rho_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::RHO_DP, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_rho_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::RHO_DH, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_T_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::T, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_T_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::T_DP, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_T_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::T_DH, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_cp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::CP, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_cp_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::CP_DP, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_cp_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::CP_DH, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_mu_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::MU, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_mu_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::MU_DP, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_mu_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::MU_DH, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_lambda_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::LAMBDA, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_lambda_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::LAMBDA_DP, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_lambda_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::LAMBDA_DH, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_sigma_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::SIGMA, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_sigma_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::SIGMA_DP, P, H, R, ncomp, ind);
}

int CoolProp_to_TRUST_generique::tppi_get_sigma_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind ) const
{
  return tppi_get_single_property_h_(Loi_en_h::SIGMA_DH, P, H, R, ncomp, ind);
}

// appel simple si besoin : cas incompressible
int CoolProp_to_TRUST_generique::tppi_get_beta_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind) const
{
  return tppi_get_single_property_h_(Loi_en_h::BETA, P, H, R, ncomp, ind);
}

// methods particuliers par application pour gagner en performance : utilise dans Pb_Multiphase (pour le moment !)
int CoolProp_to_TRUST_generique::tppi_get_CPMLB_pb_multiphase_pT(const MSpanD input, MLoiSpanD prop, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )prop.size() == 4 && (int )input.size() == 2);
  const SpanD T = input.at("temperature"), P = input.at("pressure");
  assert((int )T.size() == ncomp * (int )P.size());

  Tk_(T); // XXX : ATTENTION : need Kelvin
  SpanD cp = prop.at(Loi_en_T::CP), mu = prop.at(Loi_en_T::MU), lamb = prop.at(Loi_en_T::LAMBDA), bet = prop.at(Loi_en_T::BETA);

#ifndef NDEBUG
  for (auto& itr : prop) assert((int )T.size() == ncomp * (int )itr.second.size());
#endif

  const int sz = (int ) cp.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        cp[i] = fluide->cpmass();
        mu[i] = fluide->viscosity();
        lamb[i] = fluide->conductivity();
        bet[i] = fluide->isobaric_expansion_coefficient();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          cp[i] = fluide->cpmass();
          mu[i] = fluide->viscosity();
          lamb[i] = fluide->conductivity();
          bet[i] = fluide->isobaric_expansion_coefficient();
        }
    }

  // XXX : ATTENTION : need to put back T in C
  Tc_(T);

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_all_pb_multiphase_pT(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert( (int )input.size() == 4 && (int )inter.size() == 6 && (int )bord.size() == 2);
  const SpanD T = input.at("temperature"), P = input.at("pressure"), bT = input.at("bord_temperature"), bP = input.at("bord_pressure");
  assert ((int )bT.size() == ncomp * (int )bP.size() && (int )T.size() == ncomp * (int )P.size());

  // XXX : ATTENTION : need Kelvin
  Tk_(T), Tk_(bT);

  SpanD rho = inter.at(Loi_en_T::RHO), drhodp = inter.at(Loi_en_T::RHO_DP), drhodT = inter.at(Loi_en_T::RHO_DT),
        h = inter.at(Loi_en_T::H), dhdp = inter.at(Loi_en_T::H_DP), dhdT = inter.at(Loi_en_T::H_DT),
        brho = bord.at(Loi_en_T::RHO), bh = bord.at(Loi_en_T::H);

#ifndef NDEBUG
  for (auto& itr : inter) assert((int )T.size() == ncomp * (int )itr.second.size());
  for (auto& itr : bord) assert((int )bT.size() == ncomp * (int )itr.second.size());
#endif

  const int sz = (int ) rho.size(), bsz = (int ) brho.size();
  if (ncomp == 1)
    {
      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
          rho[i] = fluide->rhomass();
          drhodp[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
          drhodT[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP);
          h[i] = fluide->hmass();
          dhdp[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
          dhdT[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP);
        }
      for (int i = 0; i < bsz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, bP[i], bT[i]);  // SI units
          brho[i] = fluide->rhomass();
          bh[i] = fluide->hmass();
        }
    }
  else /* attention stride */
    {
      VectorD temp_((int)P.size()), btemp_((int)bP.size());
      SpanD TT(temp_), bTT(btemp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];
      for (auto& bval : bTT) bval = bT[bi_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          rho[i] = fluide->rhomass();
          drhodp[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
          drhodT[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP);
          h[i] = fluide->hmass();
          dhdp[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
          dhdT[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP);
        }
      for (int i = 0; i < bsz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, bP[i], bTT[i]);  // SI units
          brho[i] = fluide->rhomass();
          bh[i] = fluide->hmass();
        }
    }

  // XXX : ATTENTION : need to put back T in C
  Tc_(T), Tc_(bT);
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

// methods particuliers par application pour gagner en performance : utilise dans F5 (pour le moment !)
int CoolProp_to_TRUST_generique::tppi_get_all_prop_loi_F5(const MSpanD input, MLoiSpanD_h  inter, int ncomp, int ind, bool is_liq) const
{
#ifdef HAS_COOLPROP
  assert(ncomp == 1);
  const SpanD P = input.at("pression"), H = is_liq ? input.at("H_L") : input.at("H_V");
  // TODO FIXME

#ifndef NDEBUG
  for (auto &itr : inter) assert((int )H.size() == ncomp * (int )itr.second.size());
#endif
  const int sz = (int) H.size();

  if (is_liq)
    {
      SpanD rho = inter.at(Loi_en_h::RHO), drhodp = inter.at(Loi_en_h::RHO_DP), drhodh = inter.at(Loi_en_h::RHO_DH),
            cp = inter.at(Loi_en_h::CP), cpdp = inter.at(Loi_en_h::CP_DP), cpdh = inter.at( Loi_en_h::CP_DH),
            T = inter.at(Loi_en_h::T), dTdp = inter.at(Loi_en_h::T_DP), dTdh = inter.at(Loi_en_h::T_DH),
            MU = inter.at(Loi_en_h::MU), dMUdp = inter.at(Loi_en_h::MU_DP), dMUdh = inter.at(Loi_en_h::MU_DH),
            L = inter.at(Loi_en_h::LAMBDA), dLdp = inter.at(Loi_en_h::LAMBDA_DP), dLdh = inter.at(Loi_en_h::LAMBDA_DH);

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i]);  // SI units

          rho[i] = fluide->rhomass();
          drhodp[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iHmass);
          drhodh[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iHmass, CoolProp::iP);

          cp[i] = fluide->cpmass();
          cpdp[i] = fluide->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iHmass);
          cpdh[i] = fluide->first_partial_deriv(CoolProp::iCpmass, CoolProp::iHmass, CoolProp::iP);

          T[i] = fluide->T();
          dTdp[i] = fluide->first_partial_deriv(CoolProp::iT, CoolProp::iP, CoolProp::iHmass);
          dTdh[i] = fluide->first_partial_deriv(CoolProp::iT, CoolProp::iHmass, CoolProp::iP);

          MU[i] = fluide->viscosity();
          dMUdp[i] = fluide->first_partial_deriv(CoolProp::iviscosity, CoolProp::iP, CoolProp::iHmass);
          dMUdh[i] = fluide->first_partial_deriv(CoolProp::iviscosity, CoolProp::iHmass, CoolProp::iP);

          L[i] = fluide->conductivity();
          dLdp[i] = fluide->first_partial_deriv(CoolProp::iconductivity, CoolProp::iP, CoolProp::iHmass);
          dLdh[i] = fluide->first_partial_deriv(CoolProp::iconductivity, CoolProp::iHmass, CoolProp::iP);
        }
    }
  else
    {
      SpanD rho = inter.at(Loi_en_h::RHO), drhodp = inter.at(Loi_en_h::RHO_DP), drhodh = inter.at(Loi_en_h::RHO_DH),
            cp = inter.at(Loi_en_h::CP), cpdp = inter.at(Loi_en_h::CP_DP), cpdh = inter.at( Loi_en_h::CP_DH),
            T = inter.at(Loi_en_h::T), dTdp = inter.at(Loi_en_h::T_DP), dTdh = inter.at(Loi_en_h::T_DH),
            MU = inter.at(Loi_en_h::MU), dMUdp = inter.at(Loi_en_h::MU_DP), dMUdh = inter.at(Loi_en_h::MU_DH),
            L = inter.at(Loi_en_h::LAMBDA), dLdp = inter.at(Loi_en_h::LAMBDA_DP), dLdh = inter.at(Loi_en_h::LAMBDA_DH),
            sigma = inter.at(Loi_en_h::SIGMA), dsigma_dp =  inter.at(Loi_en_h::SIGMA_DP);

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i]);  // SI units

          rho[i] = fluide->rhomass();
          drhodp[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iHmass);
          drhodh[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iHmass, CoolProp::iP);

          cp[i] = fluide->cpmass();
          cpdp[i] = fluide->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iHmass);
          cpdh[i] = fluide->first_partial_deriv(CoolProp::iCpmass, CoolProp::iHmass, CoolProp::iP);

          T[i] = fluide->T();
          dTdp[i] = fluide->first_partial_deriv(CoolProp::iT, CoolProp::iP, CoolProp::iHmass);
          dTdh[i] = fluide->first_partial_deriv(CoolProp::iT, CoolProp::iHmass, CoolProp::iP);

          MU[i] = fluide->viscosity();
          dMUdp[i] = fluide->first_partial_deriv(CoolProp::iviscosity, CoolProp::iP, CoolProp::iHmass);
          dMUdh[i] = fluide->first_partial_deriv(CoolProp::iviscosity, CoolProp::iHmass, CoolProp::iP);

          L[i] = fluide->conductivity();
          dLdp[i] = fluide->first_partial_deriv(CoolProp::iconductivity, CoolProp::iP, CoolProp::iHmass);
          dLdh[i] = fluide->first_partial_deriv(CoolProp::iconductivity, CoolProp::iHmass, CoolProp::iP);

          sigma[i] = fluide->surface_tension();
          dsigma_dp[i] = fluide->first_partial_deriv(CoolProp::isurface_tension, CoolProp::iP, CoolProp::iHmass);
        }
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_all_prop_loi_F5_2(const MSpanD input, MLoiSpanD_h  inter, int ncomp, int ind, bool is_liq) const
{
#ifdef HAS_COOLPROP
  assert(ncomp == 1);
  const SpanD P = input.at("pression"), H = is_liq ? input.at("H_L") : input.at("H_V");
  // TODO FIXME

#ifndef NDEBUG
  for (auto &itr : inter) assert((int )H.size() == ncomp * (int )itr.second.size());
#endif
  const int sz = (int) H.size();

  SpanD MU = inter.at(Loi_en_h::MU), dMUdp = inter.at(Loi_en_h::MU_DP),
        L = inter.at(Loi_en_h::LAMBDA), dLdp = inter.at(Loi_en_h::LAMBDA_DP);

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i]);  // SI units

      MU[i] = fluide->viscosity();
      dMUdp[i] = fluide->first_partial_deriv(CoolProp::iviscosity, CoolProp::iP, CoolProp::iHmass);

      L[i] = fluide->conductivity();
      dLdp[i] = fluide->first_partial_deriv(CoolProp::iconductivity, CoolProp::iP, CoolProp::iHmass);
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_all_prop_loi_F5_3(const MSpanD input, MLoiSpanD_h  inter, int ncomp, int ind, bool is_liq) const
{
#ifdef HAS_COOLPROP
  assert(ncomp == 1);
  const SpanD P = input.at("pression"), H = is_liq ? input.at("H_L") : input.at("H_V");
  // TODO FIXME

#ifndef NDEBUG
  for (auto &itr : inter) assert((int )H.size() == ncomp * (int )itr.second.size());
#endif
  const int sz = (int) H.size();


  SpanD cp = inter.at(Loi_en_h::CP),  MU = inter.at(Loi_en_h::MU), dMUdp = inter.at(Loi_en_h::MU_DP), dMUdh = inter.at(Loi_en_h::MU_DH) ;

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i]);  // SI units

      cp[i] = fluide->cpmass();

      MU[i] = fluide->viscosity();
      dMUdp[i] = fluide->first_partial_deriv(CoolProp::iviscosity, CoolProp::iP, CoolProp::iHmass);
      dMUdh[i] = fluide->first_partial_deriv(CoolProp::iviscosity, CoolProp::iHmass, CoolProp::iP);
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_all_prop_loi_F5_4(const MSpanD input, MLoiSpanD_h  inter, int ncomp, int ind, bool is_liq) const
{
#ifdef HAS_COOLPROP
  assert(ncomp == 1);
  const SpanD P = input.at("pression"), H = is_liq ? input.at("H_L") : input.at("H_V");
  // TODO FIXME

#ifndef NDEBUG
  for (auto &itr : inter) assert((int )H.size() == ncomp * (int )itr.second.size());
#endif
  const int sz = (int) H.size();

  SpanD  L = inter.at(Loi_en_h::LAMBDA), dLdp = inter.at(Loi_en_h::LAMBDA_DP), dLdh = inter.at(Loi_en_h::LAMBDA_DH);

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::HmassP_INPUTS, H[i], P[i]);  // SI units

      L[i] = fluide->conductivity();
      dLdp[i] = fluide->first_partial_deriv(CoolProp::iconductivity, CoolProp::iP, CoolProp::iHmass);
      dLdh[i] = fluide->first_partial_deriv(CoolProp::iconductivity, CoolProp::iHmass, CoolProp::iP);
    }



  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}
