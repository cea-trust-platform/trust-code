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

#include <CoolProp_to_TRUST_Sat_generique.h>

using namespace CoolProp;

// iterator index !
#define i_it2 std::distance(RR.begin(), &val)

void CoolProp_to_TRUST_Sat_generique::set_saturation_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_COOLPROP
  fluide = std::shared_ptr<AbstractState>(AbstractState::factory(std::string(model_name), std::string(fluid_name)));
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_single_sat_p_(SAT enum_prop, const SpanD P_ou_T, SpanD res, int ncomp, int ind, bool is_liq) const
{
#ifdef HAS_COOLPROP
  if (enum_prop == SAT::P_SAT || enum_prop == SAT::P_SAT_DT)
    {
      assert ((int)P_ou_T.size() == (int)res.size());
      const int sz = (int )res.size();
      if (enum_prop == SAT::P_SAT)
        for (int i = 0; i < sz; i++)
          {
            fluide->update(CoolProp::QT_INPUTS, 0, P_ou_T[i]);  // SI units
            res[i] = fluide->p();
          }
      else // SAT::P_SAT_DT
        for (int i = 0; i < sz; i++)
          {
            double plus_ = EPS, minus_ = EPS;
            fluide->update(CoolProp::QT_INPUTS, 0, P_ou_T[i] * (1. + EPS));  // SI units
            plus_ = fluide->p();
            fluide->update(CoolProp::QT_INPUTS, 0, P_ou_T[i] * (1. - EPS));  // SI units
            minus_ = fluide->p();
            res[i] = (plus_ - minus_) / ( 2 * EPS * P_ou_T[i]);
          }

      return 0; // FIXME : on suppose que tout OK
    }

  if (enum_prop == SAT::SIGMA)
    {
      const int sz = (int )res.size();
      assert((int )P_ou_T.size() == sz);
      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P_ou_T[i], 0);  // SI units
          res[i] = fluide->surface_tension();
        }
      return 0; // FIXME : on suppose que tout OK
    }

  assert (ncomp * (int)P_ou_T.size() == (int)res.size());
  if (ncomp == 1) return tppi_get_single_sat_p__(enum_prop, P_ou_T, res, is_liq);
  else /* attention stride */
    {
      VectorD temp_((int)P_ou_T.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      tppi_get_single_sat_p__(enum_prop, P_ou_T, RR, is_liq);
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
      return 0; // FIXME : on suppose que tout OK
    }
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_single_sat_p__(SAT enum_prop, const SpanD P, SpanD res, bool is_liq) const
{
#ifdef HAS_COOLPROP
  // derivees qui manquent ...
  if (enum_prop == SAT::T_SAT_DP || enum_prop == SAT::LV_SAT_DP ||
      enum_prop == SAT::RHOL_SAT_DP || enum_prop == SAT::RHOV_SAT_DP ||
      enum_prop == SAT::CPL_SAT_DP || enum_prop == SAT::CPV_SAT_DP ||
      enum_prop == SAT::HL_SAT_DP || enum_prop == SAT::HV_SAT_DP)
    return FD_derivative_p(enum_prop, P, res, is_liq);

  const int sz = (int )res.size();
  if (enum_prop == SAT::LV_SAT)
    {
      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          const double hv_ = fluide->hmass();
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          const double hl_ = fluide->hmass();
          res[i] = hv_ - hl_;
        }
      return 0; // FIXME : on suppose que tout OK
    }

  for (int i = 0; i < sz; i++)
    {
      is_liq ? fluide->update(CoolProp::PQ_INPUTS,  P[i], 0) : fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
      if (enum_prop == SAT::T_SAT) res[i] = fluide->T();
      if (enum_prop == SAT::HL_SAT || enum_prop == SAT::HV_SAT) res[i] = fluide->hmass();
      if (enum_prop == SAT::RHOL_SAT || enum_prop == SAT::RHOV_SAT) res[i] = fluide->rhomass();
      if (enum_prop == SAT::CPL_SAT || enum_prop == SAT::CPV_SAT) res[i] = fluide->cpmass();
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::FD_derivative_p(SAT enum_prop, const SpanD P, SpanD res, bool is_liq) const
{
#ifdef HAS_COOLPROP
  const int sz = (int )res.size();

  if (enum_prop == SAT::LV_SAT_DP)
    {
      for (int i = 0; i < sz; i++)
        {
          double plus_ = EPS, minus_ = EPS;
          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 1);  // SI units
          plus_ = fluide->hmass();
          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 1);  // SI units
          minus_ = fluide->hmass();
          const double hv_dp_ = (plus_ - minus_) / ( 2 * EPS * P[i]);


          plus_ = EPS, minus_ = EPS;
          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 0);  // SI units
          plus_ = fluide->hmass();
          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 0);  // SI units
          minus_ = fluide->hmass();
          const double hl_dp_ = (plus_ - minus_) / ( 2 * EPS * P[i]);

          res[i] = hv_dp_ - hl_dp_;
        }
      return 0; // FIXME : on suppose que tout OK
    }

  for (int i = 0; i < sz; i++)
    {
      double plus_ = EPS, minus_ = EPS;

      is_liq ? fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 0) : fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 1);  // SI units
      if (enum_prop == SAT::T_SAT_DP) plus_ = fluide->T();
      if (enum_prop == SAT::HL_SAT_DP || enum_prop == SAT::HV_SAT_DP) plus_ = fluide->hmass();
      if (enum_prop == SAT::RHOL_SAT_DP || enum_prop == SAT::RHOV_SAT_DP) plus_ = fluide->rhomass();
      if (enum_prop == SAT::CPL_SAT_DP || enum_prop == SAT::CPV_SAT_DP) plus_ = fluide->cpmass();

      is_liq ? fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 0) : fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 1);  // SI units
      if (enum_prop == SAT::T_SAT_DP) minus_ = fluide->T();
      if (enum_prop == SAT::HL_SAT_DP || enum_prop == SAT::HV_SAT_DP) minus_ = fluide->hmass();
      if (enum_prop == SAT::RHOL_SAT_DP || enum_prop == SAT::RHOV_SAT_DP) minus_ = fluide->rhomass();
      if (enum_prop == SAT::CPL_SAT_DP || enum_prop == SAT::CPV_SAT_DP) minus_ = fluide->cpmass();

      res[i] = (plus_ - minus_) / ( 2 * EPS * P[i]);
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_T_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::T_SAT, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::T_SAT_DP, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_p_sat_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::P_SAT, T, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::P_SAT_DT, T, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HL_SAT, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HL_SAT_DP, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HV_SAT, P, res, ncomp, ind, false /* vapor */);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HV_SAT_DP, P, res, ncomp, ind, false /* vapor */);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_lvap_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::LV_SAT, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_lvap_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::LV_SAT_DP, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOL_SAT, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOL_SAT_DP, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOV_SAT, P, res, ncomp, ind, false /* vapor */);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOV_SAT_DP, P, res, ncomp, ind, false /* vapor */);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPL_SAT, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPL_SAT_DP, P, res, ncomp, ind);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPV_SAT, P, res, ncomp, ind, false /* vapor */);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPV_SAT_DP, P, res, ncomp, ind, false /* vapor */);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_sigma_pT(const SpanD P, const SpanD T, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::SIGMA, P, res, 1, 0);
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_all_flux_interfacial_pb_multiphase(const SpanD P, MSatSpanD sats, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )sats.size() == 8);
  const int sz = (int) P.size();

  SpanD Ts__ = sats.at(SAT::T_SAT), dPTs__ = sats.at(SAT::T_SAT_DP), Hvs__ = sats.at(SAT::HV_SAT), Hls__ = sats.at(SAT::HL_SAT),
        dPHvs__ = sats.at(SAT::HV_SAT_DP), dPHls__ = sats.at(SAT::HL_SAT_DP), Lvap__ = sats.at(SAT::LV_SAT), dPLvap__ = sats.at(SAT::LV_SAT_DP);

#ifndef NDEBUG
  for (auto &itr : sats) assert(ncomp * (int )P.size() == (int )itr.second.size());
#endif

  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        Ts__[i] = fluide->T();
        Hls__[i] = fluide->hmass();

        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        Hvs__[i] = fluide->hmass();

        // derivees
        double plus_1_ = EPS, minus_1_ = EPS, plus_2_ = EPS, minus_2_ = EPS;

        fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 0);  // SI units
        plus_1_ = fluide->T();
        plus_2_ = fluide->hmass();

        fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 0);  // SI units
        minus_1_ = fluide->T();
        minus_2_ = fluide->hmass();

        dPTs__[i] = (plus_1_ - minus_1_) / ( 2 * EPS * P[i]);
        dPHls__[i] = (plus_2_ - minus_2_) / ( 2 * EPS * P[i]);

        plus_1_ = EPS, minus_1_ = EPS;

        fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 1);  // SI units
        plus_1_ = fluide->hmass();

        fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 1);  // SI units
        minus_1_ = fluide->hmass();

        dPHvs__[i] = (plus_1_ - minus_1_) / ( 2 * EPS * P[i]);

        // fill values
        Lvap__[i] = Hvs__[i] - Hls__[i];
        dPLvap__[i] = dPHvs__[i] - dPHls__[i];
      }
  else /* attention stride */
    {
      VectorD Ts(sz), dPTs(sz), Hvs(sz), Hls(sz), dPHvs(sz), dPHls(sz), Lvap(sz), dPLvap;
      SpanD Ts_(Ts), dPTs_(dPTs), Hvs_(Hvs), Hls_(Hls), dPHvs_(dPHvs), dPHls_(dPHls), Lvap_(Lvap), dPLvap_(dPLvap);

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          Ts_[i] = fluide->T();
          Hls_[i] = fluide->hmass();

          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          Hvs_[i] = fluide->hmass();

          // derivees
          double plus_1_ = EPS, minus_1_ = EPS, plus_2_ = EPS, minus_2_ = EPS;

          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 0);  // SI units
          plus_1_ = fluide->T();
          plus_2_ = fluide->hmass();

          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 0);  // SI units
          minus_1_ = fluide->T();
          minus_2_ = fluide->hmass();

          dPTs_[i] = (plus_1_ - minus_1_) / ( 2 * EPS * P[i]);
          dPHls_[i] = (plus_2_ - minus_2_) / ( 2 * EPS * P[i]);

          plus_1_ = EPS, minus_1_ = EPS;

          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. + EPS), 1);  // SI units
          plus_1_ = fluide->hmass();

          fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 1);  // SI units
          minus_1_ = fluide->hmass();

          dPHvs_[i] = (plus_1_ - minus_1_) / ( 2 * EPS * P[i]);

          // fill values
          Ts__[i * ncomp + ind] = Ts_[i];
          dPTs__[i * ncomp + ind] = dPTs_[i];
          Hvs__[i * ncomp + ind] = Hvs_[i];
          Hls__[i * ncomp + ind] = Hls_[i];
          dPHvs__[i * ncomp + ind] = dPHvs_[i];
          dPHls__[i * ncomp + ind] = dPHls_[i];
          Lvap__[i * ncomp + ind] = Hvs_[i] - Hls_[i];
          dPLvap__[i * ncomp + ind] = dPHvs_[i] - dPHls_[i];
        }
    }

  // XXX : ATTENTION : need to put back T in C
  Tc_(Ts__);

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_all_sat_loi_F5(const MSpanD input, MSatSpanD sats, int ncomp, int id) const
{
#ifdef HAS_COOLPROP
  assert(ncomp == 1);
  const SpanD P = input.at("pression");

#ifndef NDEBUG
  for (auto &itr : sats) assert(ncomp * (int )P.size() == (int )itr.second.size());
#endif

  SpanD Ts_ = sats.at(SAT::T_SAT), dPTs_ = sats.at(SAT::T_SAT_DP),
        Hvs_ = sats.at(SAT::HV_SAT), Hls_ = sats.at(SAT::HL_SAT),
        dPHvs_ = sats.at(SAT::HV_SAT_DP), dPHls_ = sats.at(SAT::HL_SAT_DP),
        Rvs_ = sats.at(SAT::RHOV_SAT), Rls_ = sats.at(SAT::RHOL_SAT),
        dPRvs_ = sats.at(SAT::RHOV_SAT_DP), dPRls_ = sats.at(SAT::RHOL_SAT_DP),
        Cpvs_ = sats.at(SAT::CPV_SAT), Cpls_ = sats.at(SAT::CPL_SAT),
        dPCpvs_ = sats.at(SAT::CPV_SAT_DP), dPCpls_ = sats.at(SAT::CPL_SAT_DP);

  const int sz = (int) P.size();

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
      Hls_[i] = fluide->saturated_liquid_keyed_output(CoolProp::iHmass);
      dPHls_[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
      Rls_[i] = fluide->saturated_liquid_keyed_output(CoolProp::iDmass);
      dPRls_[i] = fluide->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP);
      Cpls_[i] = fluide->saturated_liquid_keyed_output(CoolProp::iCpmass);
      dPCpls_[i] = fluide->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP);

      fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
      Ts_[i] = fluide->saturated_vapor_keyed_output(CoolProp::iT); /* FIXME : j'utilise vapor (normalement pareil) */
      dPTs_[i] = fluide->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
      Hvs_[i] = fluide->saturated_vapor_keyed_output(CoolProp::iHmass);
      dPHvs_[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
      Rvs_[i] = fluide->saturated_vapor_keyed_output(CoolProp::iDmass);
      dPRvs_[i] = fluide->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP);
      Cpvs_[i] = fluide->saturated_vapor_keyed_output(CoolProp::iCpmass);
      dPCpvs_[i] = fluide->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP);
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_all_sat_loi_F5_2(const MSpanD input, MSatSpanD sats, int ncomp, int id) const
{
#ifdef HAS_COOLPROP
  assert(ncomp == 1);
  const SpanD P = input.at("pression");

#ifndef NDEBUG
  for (auto &itr : sats) assert(ncomp * (int )P.size() == (int )itr.second.size());
#endif

  SpanD Hvs_ = sats.at(SAT::HV_SAT),  Rvs_ = sats.at(SAT::RHOV_SAT);

  const int sz = (int) P.size();

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
      Hvs_[i] = fluide->saturated_vapor_keyed_output(CoolProp::iHmass);
      Rvs_[i] = fluide->saturated_vapor_keyed_output(CoolProp::iDmass);
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_all_sat_loi_F5_3(const MSpanD input, MSatSpanD sats, int ncomp, int id) const
{
#ifdef HAS_COOLPROP
  assert(ncomp == 1);
  const SpanD P = input.at("pression");

#ifndef NDEBUG
  for (auto &itr : sats) assert(ncomp * (int )P.size() == (int )itr.second.size());
#endif

  SpanD Hvs_ = sats.at(SAT::HV_SAT),  Hls_ = sats.at(SAT::HL_SAT);

  const int sz = (int) P.size();

  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
      Hls_[i] = fluide->saturated_liquid_keyed_output(CoolProp::iHmass);

      fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
      Hvs_[i] = fluide->saturated_vapor_keyed_output(CoolProp::iHmass);
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}
