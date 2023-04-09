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

void CoolProp_to_TRUST_generique::set_fluide_generique(const char *const model_name, const char *const fluid_name)
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
    return FD_derivative_pT__(enum_prop,P, T, R);

  if (enum_prop == Loi_en_T::MU_DT || enum_prop == Loi_en_T::LAMBDA_DT || enum_prop == Loi_en_T::SIGMA_DT)
    return FD_derivative_pT__(enum_prop,P, T, R, false /* wrt T */);

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

int CoolProp_to_TRUST_generique::FD_derivative_pT__(Loi_en_T enum_prop, const SpanD P, const SpanD T, SpanD R, bool wrt_p) const
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
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = H[i_it * ncomp + ind];
      return tppi_get_single_property_h__(enum_prop, P, TT, R);
    }
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
    return FD_derivative_ph__(enum_prop,P, H, R);

  if (enum_prop == Loi_en_h::MU_DH || enum_prop == Loi_en_h::LAMBDA_DH || enum_prop == Loi_en_h::SIGMA_DH)
    return FD_derivative_ph__(enum_prop,P, H, R, false /* wrt H */);

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

int CoolProp_to_TRUST_generique::FD_derivative_ph__(Loi_en_h enum_prop, const SpanD P, const SpanD H, SpanD R, bool wrt_p) const
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

int CoolProp_to_TRUST_generique::tppi_get_all_properties_T__(const MSpanD input, MLoiSpanD prop) const
{
#ifdef HAS_COOLPROP
  const SpanD T = input.at("temperature"), P = input.at("pressure");
  const int sz = (int ) P.size();
  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
      for (auto &itr : prop)
        {
          Loi_en_T prop_ = itr.first;
          SpanD span_ = itr.second;

          if (prop_ == Loi_en_T::RHO) span_[i] = fluide->rhomass();
          else if (prop_ == Loi_en_T::RHO_DP) span_[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
          else if (prop_ == Loi_en_T::RHO_DT) span_[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP);
          else if (prop_ == Loi_en_T::H) span_[i] = fluide->hmass();
          else if (prop_ == Loi_en_T::H_DP) span_[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
          else if (prop_ == Loi_en_T::H_DT) span_[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP);
          else if (prop_ == Loi_en_T::CP) span_[i] = fluide->cpmass();
          else if (prop_ == Loi_en_T::CP_DP) span_[i] = fluide->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iT);
          else if (prop_ == Loi_en_T::CP_DT) span_[i] = fluide->first_partial_deriv(CoolProp::iCpmass,  CoolProp::iT, CoolProp::iP);
          else if (prop_ == Loi_en_T::MU) span_[i] = fluide->viscosity();
          else if (prop_ == Loi_en_T::LAMBDA) span_[i] = fluide->conductivity();
          else if (prop_ == Loi_en_T::BETA) span_[i] = fluide->isobaric_expansion_coefficient();
          else if (prop_ == Loi_en_T::SIGMA) span_[i] = fluide->surface_tension();
          else Process::exit("Derivative requested is not coded !");
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

// methods particuliers par application pour gagner en performance : utilise dans Pb_Multiphase (pour le moment !)
int CoolProp_to_TRUST_generique::tppi_get_CPMLB_pb_multiphase_pT(const MSpanD input, MLoiSpanD prop, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  const SpanD T = input.at("temperature"), P = input.at("pressure");

#ifndef NDEBUG
  assert((int )prop.size() == 4 && (int )input.size() == 2);
  assert((int )T.size() == ncomp * (int )P.size());
  for (auto& itr : prop) assert((int )T.size() == ncomp * (int )itr.second.size());
#endif

  Tk_(T); // XXX : ATTENTION : need Kelvin

  if (ncomp == 1) tppi_get_all_properties_T__(input,prop);
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];
      MSpanD input_ = { { "temperature", TT }, { "pressure", P }};
      tppi_get_all_properties_T__(input_,prop);
    }

  Tc_(T); // XXX : ATTENTION : need to put back T in C

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_all_pb_multiphase_pT(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  const SpanD T = input.at("temperature"), P = input.at("pressure"), bT = input.at("bord_temperature"), bP = input.at("bord_pressure");

#ifndef NDEBUG
  assert( (int )input.size() == 4 && (int )inter.size() == 6 && (int )bord.size() == 2);
  assert ((int )bT.size() == ncomp * (int )bP.size() && (int )T.size() == ncomp * (int )P.size());
  for (auto& itr : inter) assert((int )T.size() == ncomp * (int )itr.second.size());
  for (auto& itr : bord) assert((int )bT.size() == ncomp * (int )itr.second.size());
#endif

  Tk_(T), Tk_(bT); // XXX : ATTENTION : need Kelvin

  if (ncomp == 1)
    {
      MSpanD input_int_ = { { "temperature", T }, { "pressure", P }};
      MSpanD input_bord_ = { { "temperature", bT }, { "pressure", bP }};
      tppi_get_all_properties_T__(input_int_,inter);
      tppi_get_all_properties_T__(input_bord_,bord);
    }
  else /* attention stride */
    {
      VectorD temp_((int)P.size()), btemp_((int)bP.size());
      SpanD TT(temp_), bTT(btemp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];
      for (auto& bval : bTT) bval = bT[bi_it * ncomp + ind];

      MSpanD input_int_ = { { "temperature", TT }, { "pressure", P }};
      MSpanD input_bord_ = { { "temperature", bTT }, { "pressure", bP }};
      tppi_get_all_properties_T__(input_int_,inter);
      tppi_get_all_properties_T__(input_bord_,bord);
    }

  Tc_(T), Tc_(bT); // XXX : ATTENTION : need to put back T in C

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
