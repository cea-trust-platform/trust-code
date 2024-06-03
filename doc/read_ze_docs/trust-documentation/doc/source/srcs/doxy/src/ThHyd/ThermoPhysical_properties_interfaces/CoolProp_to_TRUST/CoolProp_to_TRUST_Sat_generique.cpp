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
  fld_name_sat_ = std::string(fluid_name);
  fluide = std::shared_ptr<AbstractState>(AbstractState::factory(std::string(model_name), fld_name_sat_));
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
  if (enum_prop == SAT::T_SAT_DP || enum_prop == SAT::LV_SAT_DP || enum_prop == SAT::RHOL_SAT_DP || enum_prop == SAT::RHOV_SAT_DP ||
      enum_prop == SAT::CPL_SAT_DP || enum_prop == SAT::CPV_SAT_DP || enum_prop == SAT::HL_SAT_DP || enum_prop == SAT::HV_SAT_DP)
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
      fluide->update(CoolProp::PQ_INPUTS,  P[i], is_liq ? 0 : 1);  // SI units
      if (enum_prop == SAT::T_SAT) res[i] = fluide->T();
      if (enum_prop == SAT::HL_SAT || enum_prop == SAT::HV_SAT) res[i] = fluide->hmass();
      if (enum_prop == SAT::RHOL_SAT || enum_prop == SAT::RHOV_SAT) res[i] = fluide->rhomass();
      if (enum_prop == SAT::CPL_SAT || enum_prop == SAT::CPV_SAT) res[i] = fluide->cpmass();
      if (enum_prop == SAT::SIGMA  && sigma_mano_<=0.)
        {
          // soucis la avec plusieurs instance de factory abstract_state
//          res[i] = fluide->surface_tension();
          const int ph_ = is_liq ? 0 : 1;
          res[i] = CoolProp::PropsSI("surface_tension", "P", P[i], "Q", ph_, fld_name_sat_);
        }
      else if (enum_prop == SAT::SIGMA && sigma_mano_>0.) res[i] = sigma_mano_;
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

int CoolProp_to_TRUST_Sat_generique::tppi_get_all_flux_interfacial_pb_multiphase(const SpanD P, MSatSpanD sats, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )sats.size() == 7);
  const int sz = (int) P.size();

  SpanD dPTs__ = sats.at(SAT::T_SAT_DP), Hvs__ = sats.at(SAT::HV_SAT), Hls__ = sats.at(SAT::HL_SAT),
        dPHvs__ = sats.at(SAT::HV_SAT_DP), dPHls__ = sats.at(SAT::HL_SAT_DP), Lvap__ = sats.at(SAT::LV_SAT), dPLvap__ = sats.at(SAT::LV_SAT_DP);

#ifndef NDEBUG
  for (auto &itr : sats) assert(ncomp * (int )P.size() == (int )itr.second.size());
#endif

  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
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
      VectorD dPTs(sz), Hvs(sz), Hls(sz), dPHvs(sz), dPHls(sz), Lvap(sz), dPLvap;
      SpanD dPTs_(dPTs), Hvs_(Hvs), Hls_(Hls), dPHvs_(dPHvs), dPHls_(dPHls), Lvap_(Lvap), dPLvap_(dPLvap);

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
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
          dPTs__[i * ncomp + ind] = dPTs_[i];
          Hvs__[i * ncomp + ind] = Hvs_[i];
          Hls__[i * ncomp + ind] = Hls_[i];
          dPHvs__[i * ncomp + ind] = dPHvs_[i];
          dPHls__[i * ncomp + ind] = dPHls_[i];
          Lvap__[i * ncomp + ind] = Hvs_[i] - Hls_[i];
          dPLvap__[i * ncomp + ind] = dPHvs_[i] - dPHls_[i];
        }
    }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_all_sat_loi_F5(const MSpanD input, MSatSpanD sats, int ncomp, int id) const
{
#ifdef HAS_COOLPROP
  const SpanD P = input.at("pression");
  const int sz = (int) P.size();

#ifndef NDEBUG
  assert(ncomp == 1);
  for (auto &itr : sats) assert(ncomp * sz == (int )itr.second.size());
#endif

  bool has_liq_prop = false, has_vap_prop = false;
  for (auto &itr : sats)
    {
      if (itr.first == SAT::T_SAT || itr.first == SAT::SIGMA || itr.first == SAT::HL_SAT || itr.first == SAT::RHOL_SAT || itr.first == SAT::CPL_SAT) has_liq_prop = true;
      break;
    }

  for (auto &itr : sats)
    {
      if (itr.first == SAT::HV_SAT || itr.first == SAT::RHOV_SAT || itr.first == SAT::CPV_SAT) has_vap_prop = true;
      break;
    }

  if (has_liq_prop)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;
            SpanD span_ = itr.second;

            if (prop_ == SAT::T_SAT) span_[i] = fluide->T();
            if (prop_ == SAT::SIGMA) span_[i] = fluide->surface_tension();

            if (prop_ == SAT::HL_SAT) span_[i] = fluide->hmass();
            if (prop_ == SAT::RHOL_SAT) span_[i] = fluide->rhomass();
            if (prop_ == SAT::CPL_SAT) span_[i] = fluide->cpmass();
          }
      }

  if (has_vap_prop)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;
            SpanD span_ = itr.second;

            if (prop_ == SAT::HV_SAT) span_[i] = fluide->hmass();
            if (prop_ == SAT::RHOV_SAT) span_[i] = fluide->rhomass();
            if (prop_ == SAT::CPV_SAT) span_[i] = fluide->cpmass();
          }
      }

  // derivees qui manquent ...
  bool has_liq_DP = false, has_vap_DP = false;
  for (auto &itr : sats)
    {
      if (itr.first == SAT::T_SAT_DP || itr.first == SAT::HL_SAT_DP || itr.first == SAT::RHOL_SAT_DP || itr.first == SAT::CPL_SAT_DP || itr.first == SAT::SIGMA_DP) has_liq_DP = true;
      break;
    }

  for (auto &itr : sats)
    {
      if (itr.first == SAT::HV_SAT_DP || itr.first == SAT::RHOV_SAT_DP || itr.first == SAT::CPV_SAT_DP ) has_vap_DP = true;
      break;
    }

  if (has_liq_DP)
    for (int i = 0; i < sz; i++)
      {
        double plus_T_ = EPS, plus_h_ = EPS, plus_rho_ = EPS, plus_cp_ = EPS, plus_sigma_ = EPS;
        fluide->update(CoolProp::PQ_INPUTS, P[i] * (1. + EPS), 0);
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;

            if (prop_ == SAT::T_SAT_DP) plus_T_ = fluide->T();
            if (prop_ == SAT::HL_SAT_DP) plus_h_ = fluide->hmass();
            if (prop_ == SAT::RHOL_SAT_DP) plus_rho_ = fluide->rhomass();
            if (prop_ == SAT::CPL_SAT_DP) plus_cp_ = fluide->cpmass();
            if (prop_ == SAT::SIGMA_DP) plus_sigma_ = fluide->surface_tension();
          }

        double minus_T_ = EPS, minus_h_ = EPS, minus_rho_ = EPS, minus_cp_ = EPS, minus_sigma_ = EPS;
        fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 0);
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;

            if (prop_ == SAT::T_SAT_DP) minus_T_ = fluide->T();
            if (prop_ == SAT::HL_SAT_DP) minus_h_ = fluide->hmass();
            if (prop_ == SAT::RHOL_SAT_DP) minus_rho_ = fluide->rhomass();
            if (prop_ == SAT::CPL_SAT_DP) minus_cp_ = fluide->cpmass();
            if (prop_ == SAT::SIGMA_DP) minus_sigma_ = fluide->surface_tension();
          }
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;
            SpanD span_ = itr.second;

            if (prop_ == SAT::T_SAT_DP) span_[i] = (plus_T_ - minus_T_) / ( 2 * EPS * P[i]);
            if (prop_ == SAT::HL_SAT_DP) span_[i] = (plus_h_ - minus_h_) / ( 2 * EPS * P[i]);
            if (prop_ == SAT::RHOL_SAT_DP) span_[i] = (plus_rho_ - minus_rho_) / ( 2 * EPS * P[i]);
            if (prop_ == SAT::CPL_SAT_DP) span_[i] = (plus_cp_ - minus_cp_) / ( 2 * EPS * P[i]);
            if (prop_ == SAT::SIGMA_DP) span_[i] = (plus_sigma_ - minus_sigma_) / ( 2 * EPS * P[i]);
          }
      }

  if (has_vap_DP)
    for (int i = 0; i < sz; i++)
      {
        double plus_h_ = EPS, plus_rho_ = EPS, plus_cp_ = EPS;
        fluide->update(CoolProp::PQ_INPUTS, P[i] * (1. + EPS), 1);
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;

            if (prop_ == SAT::HV_SAT_DP) plus_h_ = fluide->hmass();
            if (prop_ == SAT::RHOV_SAT_DP) plus_rho_ = fluide->rhomass();
            if (prop_ == SAT::CPV_SAT_DP) plus_cp_ = fluide->cpmass();
          }

        double minus_h_ = EPS, minus_rho_ = EPS, minus_cp_ = EPS;
        fluide->update(CoolProp::PQ_INPUTS,  P[i] * (1. - EPS), 1);
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;

            if (prop_ == SAT::HV_SAT_DP) minus_h_ = fluide->hmass();
            if (prop_ == SAT::RHOV_SAT_DP) minus_rho_ = fluide->rhomass();
            if (prop_ == SAT::CPV_SAT_DP) minus_cp_ = fluide->cpmass();
          }
        for (auto &itr : sats)
          {
            SAT prop_ = itr.first;
            SpanD span_ = itr.second;

            if (prop_ == SAT::HV_SAT_DP) span_[i] = (plus_h_ - minus_h_) / ( 2 * EPS * P[i]);
            if (prop_ == SAT::RHOV_SAT_DP) span_[i] = (plus_rho_ - minus_rho_) / ( 2 * EPS * P[i]);
            if (prop_ == SAT::CPV_SAT_DP) span_[i] = (plus_cp_ - minus_cp_) / ( 2 * EPS * P[i]);

          }
      }

  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}
