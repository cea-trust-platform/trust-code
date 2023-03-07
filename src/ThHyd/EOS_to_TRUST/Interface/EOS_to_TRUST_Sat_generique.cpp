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
#include <Process.h>

using namespace NEPTUNE ;

// iterator index !
#define i_it std::distance(TT.begin(), &val)
#define i_it2 std::distance(RR.begin(), &val)

void EOS_to_TRUST_Sat_generique::set_EOS_Sat_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_EOS
  fluide = new NEPTUNE::EOS(model_name, fluid_name);
  assert(fluide->fluid_name() == fluid_name);
  fluide->set_error_handler(handler); // Set error handler
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_T_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert (ncomp * (int)P.size() == (int)res.size());
  if (ncomp == 1) compute_eos_field(P, res, "tsat", "T_sat");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      compute_eos_field(P, RR, "tsat", "T_sat");
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_d_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert (ncomp * (int)P.size() == (int)res.size());
  if (ncomp == 1) compute_eos_field(P, res, "dtsatdp", "d_T_sat_d_p");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      compute_eos_field(P, RR, "dtsatdp", "d_T_sat_d_p");
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_p_sat_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert ((int)T.size() == (int)res.size());
  compute_eos_field(T, res, "psat", "P_sat",true /* is_T */);
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_d_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  Cerr << "EOS_to_TRUST_Sat_generique::eos_get_d_p_sat_d_T_T not coded ! Call the 911 !" << finl;
  Process::exit();
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_lvap_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  VectorD arr1_((int)P.size()), arr2_((int)P.size());
  eos_get_h_v_sat_p(P,SpanD(arr1_),1,0);
  eos_get_h_l_sat_p(P,SpanD(arr2_),1,0);
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = arr1_[i] - arr2_[i];
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_d_lvap_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  VectorD arr1_((int)P.size()), arr2_((int)P.size());
  eos_get_d_h_v_sat_d_p_p(P,SpanD(arr1_),1,0);
  eos_get_d_h_l_sat_d_p_p(P,SpanD(arr2_),1,0);
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = arr1_[i] - arr2_[i];
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert (ncomp * (int)P.size() == (int)res.size());
  if (ncomp == 1) compute_eos_field(P, res, "hlsat", "h_l_sat");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      compute_eos_field(P, RR, "hlsat", "h_l_sat");
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_d_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert (ncomp * (int)P.size() == (int)res.size());
  if (ncomp == 1) compute_eos_field(P, res, "dhlsatdp", "d_h_l_sat_d_p");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      compute_eos_field(P, RR, "dhlsatdp", "d_h_l_sat_d_p");
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert (ncomp * (int)P.size() == (int)res.size());
  if (ncomp == 1) compute_eos_field(P, res, "hvsat", "h_v_sat");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      compute_eos_field(P, RR, "hvsat", "h_v_sat");
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_d_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_EOS
  assert (ncomp * (int)P.size() == (int)res.size());
  if (ncomp == 1) compute_eos_field(P, res, "dhvsatdp", "d_h_v_sat_d_p");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];
      compute_eos_field(P, RR, "dhvsatdp", "d_h_v_sat_d_p");
      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "sigma", "sigma");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "sigma", "sigma");
      for (auto& val : TT) T[i_it * ncomp + id] = val;
    }
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_Sat_generique::eos_get_all_flux_interfacial(MSpanD sats, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )sats.size() == 8);

  const SpanD P = sats.at("pressure");
  SpanD Ts__ = sats.at("Tsat"), dPTs__ = sats.at("dP_Tsat"), Hvs__ = sats.at("Hvs"), Hls__ = sats.at("Hls"), dPHvs__ = sats.at("dP_Hvs"), dPHls__ = sats.at("dP_Hls"), Lvap__ = sats.at("Lvap");

  assert(ncomp * (int )P.size() == (int )Ts__.size());
  assert(ncomp * (int )P.size() == (int )dPTs__.size());
  assert(ncomp * (int )P.size() == (int )Hvs__.size());
  assert(ncomp * (int )P.size() == (int )Hls__.size());
  assert(ncomp * (int )P.size() == (int )dPHvs__.size());
  assert(ncomp * (int )P.size() == (int )dPHls__.size());
  assert(ncomp * (int )P.size() == (int )Lvap__.size());

  int i_out = 0;
  const int nb_out = 7; /* 7 variables to fill */
  ArrOfInt tmp((int)P.size());
  EOS_Error_Field ferr(tmp);
  EOS_Fields flds_out(nb_out);
  EOS_Field P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());

  if (ncomp == 1)
    {
      flds_out[i_out++] = EOS_Field("tsat", "T_sat", (int) Ts__.size(), (double*) Ts__.begin());
      flds_out[i_out++] = EOS_Field("dtsatdp", "d_T_sat_d_p", (int) dPTs__.size(), (double*) dPTs__.begin());
      flds_out[i_out++] = EOS_Field("hvsat", "h_v_sat", (int) Hvs__.size(), (double*) Hvs__.begin());
      flds_out[i_out++] = EOS_Field("hlsat", "h_l_sat", (int) Hls__.size(), (double*) Hls__.begin());
      flds_out[i_out++] = EOS_Field("dhvsatdp", "d_h_v_sat_d_p", (int) dPHvs__.size(), (double*) dPHvs__.begin());
      flds_out[i_out++] = EOS_Field("dhlsatdp", "d_h_l_sat_d_p", (int) dPHls__.size(), (double*) dPHls__.begin());
      flds_out[i_out++] = EOS_Field("hlsat", "h_l_sat", (int) Lvap__.size(), (double*) Lvap__.begin());
      fluide->compute(P_fld, flds_out, ferr);
    }
  else
    {
      const int sz = (int) P.size();
      VectorD Ts(sz), dPTs(sz), Hvs(sz), Hls(sz), dPHvs(sz), dPHls(sz), Lvap(sz);
      SpanD Ts_(Ts), dPTs_(dPTs), Hvs_(Hvs), Hls_(Hls), dPHvs_(dPHvs), dPHls_(dPHls), Lvap_(Lvap);

      flds_out[i_out++] = EOS_Field("tsat", "T_sat", (int) Ts_.size(), (double*) Ts_.begin());
      flds_out[i_out++] = EOS_Field("dtsatdp", "d_T_sat_d_p", (int) dPTs_.size(), (double*) dPTs_.begin());
      flds_out[i_out++] = EOS_Field("hvsat", "h_v_sat", (int) Hvs_.size(), (double*) Hvs_.begin());
      flds_out[i_out++] = EOS_Field("hlsat", "h_l_sat", (int) Hls_.size(), (double*) Hls_.begin());
      flds_out[i_out++] = EOS_Field("dhvsatdp", "d_h_v_sat_d_p", (int) dPHvs_.size(), (double*) dPHvs_.begin());
      flds_out[i_out++] = EOS_Field("dhlsatdp", "d_h_l_sat_d_p", (int) dPHls_.size(), (double*) dPHls_.begin());
      flds_out[i_out++] = EOS_Field("hlsat", "h_l_sat", (int) Lvap_.size(), (double*) Lvap_.begin());
      fluide->compute(P_fld, flds_out, ferr);


      for (int i = 0; i < sz; i++)
        {
          Ts__[i * ncomp + id] = Ts_[i];
          dPTs__[i * ncomp + id] = dPTs_[i];
          Hvs__[i * ncomp + id] = Hvs_[i];
          Hls__[i * ncomp + id] = Hls_[i];
          dPHvs__[i * ncomp + id] = dPHvs_[i];
          dPHls__[i * ncomp + id] = dPHls_[i];
          Lvap__[i * ncomp + id] = Lvap_[i];
        }
    }

  // XXX : ATTENTION : need to put back T in C
  Tc_(Ts__);
#else
  Cerr << "EOS_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}
