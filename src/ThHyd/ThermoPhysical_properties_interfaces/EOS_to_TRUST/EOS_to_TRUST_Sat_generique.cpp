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

#include <EOS_to_TRUST_Sat_generique.h>

using namespace NEPTUNE ;

// iterator index !
#define i_it std::distance(TT.begin(), &val)
#define i_it2 std::distance(RR.begin(), &val)

void EOS_to_TRUST_Sat_generique::set_EOS_Sat_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_EOS
  fluide = new NEPTUNE::EOS(model_name, fluid_name);
//  assert(fluide->info_fluidequa() == fluid_name);
  fluide->set_error_handler(handler); // Set error handler
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_single_sat_p_(SAT enum_prop, const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert (ncomp * (int)P.size() == (int)res.size());
  if (ncomp == 1) return compute_eos_field(P, res, EOS_prop_sat[(int)enum_prop][0], EOS_prop_sat[(int)enum_prop][1]);
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      int err_;
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      err_ = compute_eos_field(P, RR, EOS_prop_sat[(int)enum_prop][0], EOS_prop_sat[(int)enum_prop][1]);
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
      return err_;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_T_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::T_SAT, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::T_SAT_DP, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HL_SAT, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HL_SAT_DP, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HV_SAT, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::HV_SAT_DP, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_rho_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOL_SAT, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_rho_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOL_SAT_DP, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_rho_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOV_SAT, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_rho_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::RHOV_SAT_DP, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_cp_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPL_SAT, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_cp_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPL_SAT_DP, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_cp_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPV_SAT, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_cp_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
  return tppi_get_single_sat_p_(SAT::CPV_SAT_DP, P, res, ncomp, ind);
}

int EOS_to_TRUST_Sat_generique::tppi_get_p_sat_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert ((int)T.size() == (int)res.size());
  return compute_eos_field(T, res, EOS_prop_sat[(int)SAT::P_SAT][0], EOS_prop_sat[(int)SAT::P_SAT][1], true /* is_T */);
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  Process::exit("EOS_to_TRUST_Sat_generique::tppi_get_p_sat_d_T_T not coded ! Call the 911 !");
  return 1;
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_lvap_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  VectorD arr1_((int)P.size()), arr2_((int)P.size());
  int err1_, err2_;
  err1_ = tppi_get_h_v_sat_p(P,SpanD(arr1_),1,0);
  err2_ = tppi_get_h_l_sat_p(P,SpanD(arr2_),1,0);
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = arr1_[i] - arr2_[i];
  return std::max(err1_,err2_);
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_lvap_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  VectorD arr1_((int)P.size()), arr2_((int)P.size());
  int err1_, err2_;
  err1_ = tppi_get_h_v_sat_d_p_p(P,SpanD(arr1_),1,0);
  err2_ = tppi_get_h_l_sat_d_p_p(P,SpanD(arr2_),1,0);
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = arr1_[i] - arr2_[i];
  return std::max(err1_,err2_);
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) return compute_eos_field(P, T, R, EOS_prop_sat[(int)SAT::SIGMA][0], EOS_prop_sat[(int)SAT::SIGMA][1]);
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      int err_;
      for (auto& val : TT) val = T[i_it * ncomp + id];
      err_ = compute_eos_field(P, TT, R, EOS_prop_sat[(int)SAT::SIGMA][0], EOS_prop_sat[(int)SAT::SIGMA][1]);
      for (auto& val : TT) T[i_it * ncomp + id] = val;
      return err_;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_sigma_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )R.size());
  if (ncomp == 1) return compute_eos_field_h(P, H, R, EOS_prop_sat[(int)SAT::SIGMA][0], EOS_prop_sat[(int)SAT::SIGMA][1]);
  else /* attention stride */
    Process::exit("No stride allowed for the moment for enthalpie calls ... use temperature or call 911 !");
  return 1;
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_all_flux_interfacial_pb_multiphase(const SpanD P, MSatSpanD sats, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )sats.size() == 8);

  SpanD Ts__ = sats.at(SAT::T_SAT), dPTs__ = sats.at(SAT::T_SAT_DP), Hvs__ = sats.at(SAT::HV_SAT), Hls__ = sats.at(SAT::HL_SAT),
        dPHvs__ = sats.at(SAT::HV_SAT_DP), dPHls__ = sats.at(SAT::HL_SAT_DP), Lvap__ = sats.at(SAT::LV_SAT), dPLvap__ = sats.at(SAT::LV_SAT_DP);

  const int sz = (int) P.size(), nb_out = 6; /* NOTA BENE : 6 car LV_SAT et LV_SAT_DP on recalcule apres  */
  int i_out = 0, err_;
  ArrOfInt tmp(sz);
  EOS_Error_Field ferr(tmp);
  EOS_Fields flds_out(nb_out);
  EOS_Field P_fld("Pressure", "P", sz, (double*) P.begin());

  if (ncomp == 1)
    {
      for (auto &itr : sats)
        {
          SAT prop_ = itr.first;
          SpanD span_ = itr.second;
          assert(ncomp * (int )P.size() == (int )span_.size());
          if (prop_ != SAT::LV_SAT && prop_ != SAT::LV_SAT_DP)
            flds_out[i_out++] = EOS_Field(EOS_prop_sat[(int) prop_][0], EOS_prop_sat[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
        }
      err_ = (int)fluide->compute(P_fld, flds_out, ferr);

      // on rempli LV_SAT et LV_SAT_DP
      for (int i = 0; i < sz; i++)
        {
          Lvap__[i] = Hvs__[i] - Hls__[i];
          dPLvap__[i] = dPHvs__[i] - dPHls__[i];
        }
    }
  else /* attention stride */
    {
#ifndef NDEBUG
      for (auto &itr : sats) assert(ncomp * (int )P.size() == (int )itr.second.size());
#endif
      VectorD Ts(sz), dPTs(sz), Hvs(sz), Hls(sz), dPHvs(sz), dPHls(sz), Lvap(sz), dPLvap;
      SpanD Ts_(Ts), dPTs_(dPTs), Hvs_(Hvs), Hls_(Hls), dPHvs_(dPHvs), dPHls_(dPHls), Lvap_(Lvap), dPLvap_(dPLvap);

      MSatSpanD sats_loc = { { SAT::T_SAT, Ts_ }, { SAT::T_SAT_DP, dPTs_ }, { SAT::HV_SAT, Hvs_ }, { SAT::HL_SAT, Hls_ },
        { SAT::HV_SAT_DP, dPHvs_ }, { SAT::HL_SAT_DP, dPHls_ }
      };

      for (auto &itr : sats_loc)
        {
          SAT prop_ = itr.first;
          SpanD span_ = itr.second;
          flds_out[i_out++] = EOS_Field(EOS_prop_sat[(int) prop_][0], EOS_prop_sat[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
        }

      err_ = (int)fluide->compute(P_fld, flds_out, ferr);

      for (int i = 0; i < sz; i++)
        {
          Ts__[i * ncomp + id] = Ts_[i];
          dPTs__[i * ncomp + id] = dPTs_[i];
          Hvs__[i * ncomp + id] = Hvs_[i];
          Hls__[i * ncomp + id] = Hls_[i];
          dPHvs__[i * ncomp + id] = dPHvs_[i];
          dPHls__[i * ncomp + id] = dPHls_[i];
          Lvap__[i * ncomp + id] = Hvs_[i] - Hls_[i];
          dPLvap__[i * ncomp + id] = dPHvs_[i] - dPHls_[i];
        }
    }

  // XXX : ATTENTION : need to put back T in C
  Tc_(Ts__);
  return err_;
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_Sat_generique::tppi_get_all_sat_loi_F5(const MSpanD input, MSatSpanD sats, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert(ncomp == 1);
  const SpanD P = input.at("pression");

  int i_out = 0, nb_out = (int) sats.size(), sz = (int) P.size();
  ArrOfInt tmp(sz);
  EOS_Error_Field ferr(tmp);
  EOS_Fields flds_out(nb_out);
  EOS_Field P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());

  for (auto &itr : sats)
    {
      SAT prop_ = itr.first;
      SpanD span_ = itr.second;
      assert(sz == (int ) span_.size());
      flds_out[i_out++] = EOS_Field(EOS_prop_sat[(int) prop_][0], EOS_prop_sat[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
    }

  return (int)fluide->compute(P_fld, flds_out, ferr);
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}
