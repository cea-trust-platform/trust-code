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

#include <EOS_to_TRUST_generique.h>

using namespace NEPTUNE ;

// iterator index !
#define i_it std::distance(TT.begin(), &val)
#define bi_it std::distance(bTT.begin(), &bval)
#define i_itR std::distance(R.begin(), &val)

void EOS_to_TRUST_generique::set_EOS_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_EOS
  fluide = new NEPTUNE::EOS(model_name, fluid_name);
//  assert (fluide->info_fluidequa() == fluid_name);
  fluide->set_error_handler(handler); // Set error handler
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_single_property_T_(Loi_en_T enum_prop, const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1)
    compute_eos_field(P, T, R, EOS_prop_en_T[(int)enum_prop][0], EOS_prop_en_T[(int)enum_prop][1]);
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, EOS_prop_en_T[(int)enum_prop][0], EOS_prop_en_T[(int)enum_prop][1]);
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_single_property_h_(Loi_en_h enum_prop, const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )H.size() == (int )P.size() && (int )H.size() == (int )R.size());
  assert (ncomp == 1);
  if (ncomp == 1)
    compute_eos_field_h(P, H, R, EOS_prop_en_h[(int)enum_prop][0], EOS_prop_en_h[(int)enum_prop][1]);
  else
    Process::exit("No stride allowed for the moment for enthalpie calls ... use temperature or call 911 !");
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_rho_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::RHO, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_rho_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::RHO, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_rho_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::RHO_DP, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_rho_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::RHO_DP, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_rho_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::RHO_DT, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_rho_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::RHO_DH, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_h_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::H, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_T_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::T, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_h_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::H_DP, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_T_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::T_DP, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_h_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::H_DT, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_T_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::T_DH, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_cp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::CP, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_cp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::CP, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_cp_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::CP_DP, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_cp_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::CP_DP, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_cp_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::CP_DT, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_cp_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::CP_DH, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_mu_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::MU, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_mu_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::MU, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_mu_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::MU_DP, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_mu_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::MU_DP, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_mu_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::MU_DT, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_mu_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::MU_DH, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_lambda_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::LAMBDA, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_lambda_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::LAMBDA, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_lambda_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::LAMBDA_DP, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_lambda_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::LAMBDA_DP, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_lambda_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::LAMBDA_DT, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_lambda_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::LAMBDA_DH, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::SIGMA, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_sigma_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::SIGMA, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_sigma_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::SIGMA_DP, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_sigma_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::SIGMA_DP, P, H, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_sigma_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_T_(Loi_en_T::SIGMA_DT, P, T, R, ncomp, id);
}

void EOS_to_TRUST_generique::eos_get_sigma_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  eos_get_single_property_h_(Loi_en_h::SIGMA_DH, P, H, R, ncomp, id);
}

// si incompressible et en T ...
void EOS_to_TRUST_generique::eos_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )R.size() && (int )T.size() == ncomp * (int )P.size());
  VectorD drho_dt_((int)P.size()), rho_((int)P.size()), temp_((int)P.size());
  SpanD drho_dt(drho_dt_), rho(rho_), TT(temp_);
  for (auto& val : TT) val = T[i_it * ncomp + id];
  compute_eos_field(P, TT, rho, EOS_prop_en_T[(int) Loi_en_T::RHO][0], EOS_prop_en_T[(int) Loi_en_T::RHO][1]);
  compute_eos_field(P, TT, drho_dt, EOS_prop_en_T[(int) Loi_en_T::RHO_DT][0], EOS_prop_en_T[(int) Loi_en_T::RHO_DT][1]);
  // fill beta ...
  for (auto& val : R) val = drho_dt[i_itR] / rho[i_itR];
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

// methods particuliers par application pour gagner en performance : utilise dans Pb_Multiphase (pour le moment !)
#ifdef HAS_EOS
void EOS_to_TRUST_generique::eos_get_all_properties_T_(const MSpanD input , EOS_Fields& flds_out, EOS_Error_Field& ferr, int ncomp, int id) const
{
  const SpanD T = input.at("temperature"), P = input.at("pressure");
  if (ncomp == 1)
    {
      EOS_Field T_fld("Temperature", "T", (int) T.size(), (double*) T.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
      fluide->compute(P_fld, T_fld, flds_out, ferr);
    }
  else /* attention stride */
    {
      VectorD temp_((int) P.size());
      SpanD TT(temp_);
      for (auto &val : TT) val = T[i_it * ncomp + id];

      EOS_Field T_fld("Temperature", "T", (int) TT.size(), (double*) TT.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
      fluide->compute(P_fld, T_fld, flds_out, ferr);
    }
}
#endif

void EOS_to_TRUST_generique::eos_get_CPMLB_pb_multiphase_pT(const MSpanD input, MLoiSpanD prop, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )prop.size() == 4 && (int )input.size() == 2);

  const SpanD T = input.at("temperature"), P = input.at("pressure");
  assert((int )T.size() == ncomp * (int )P.size());

  Tk_(T); // XXX : ATTENTION : need Kelvin

  SpanD B = prop.at(Loi_en_T::BETA);

  const int nb_out = 5; /* 5 variables to fill */
  VectorD drho_dt_((int) P.size()), rho_((int) P.size());
  SpanD drho_dt(drho_dt_), rho(rho_);
  ArrOfInt tmp((int)P.size());
  EOS_Error_Field ferr(tmp);
  EOS_Fields flds_out(nb_out);
  int i_out = 0;

  for (auto& itr : prop)
    {
      assert((int )T.size() == ncomp * (int )itr.second.size());
      if (itr.first != Loi_en_T::BETA)
        flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) itr.first][0], EOS_prop_en_T[(int) itr.first][1], (int) itr.second.size(), (double*) itr.second.begin());
      else /* pour beta on recalcule sans appel a beta de eos ... */
        {
          flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) Loi_en_T::RHO][0], EOS_prop_en_T[(int) Loi_en_T::RHO][1], (int) rho.size(), (double*) rho.begin());
          flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) Loi_en_T::RHO_DT][0], EOS_prop_en_T[(int) Loi_en_T::RHO_DT][1], (int) drho_dt.size(), (double*) drho_dt.begin());
        }
    }

  eos_get_all_properties_T_(input, flds_out, ferr, ncomp, id);

  /* beta */
  for (int i = 0; i < (int) B.size(); i++) B[i] = drho_dt[i] / rho[i];

  // XXX : ATTENTION : need to put back T in C
  Tc_(T);
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_all_pb_multiphase_pT(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert( (int )input.size() == 4 && (int )inter.size() == 6 && (int )bord.size() == 2);

  const SpanD T = input.at("temperature"), P = input.at("pressure"), bT = input.at("bord_temperature"), bP = input.at("bord_pressure");
  assert ((int )bT.size() == ncomp * (int )bP.size() && (int )T.size() == ncomp * (int )P.size());

  // XXX : ATTENTION : need Kelvin
  Tk_(T), Tk_(bT);

  const int nb_out = (int )inter.size(), bnb_out = (int )bord.size();
  ArrOfInt tmp((int)P.size()), btmp((int)bP.size());
  EOS_Error_Field ferr(tmp), bferr(btmp);
  EOS_Fields flds_out(nb_out), bflds_out(bnb_out);

  int i_out = 0, bi_out = 0;

  for (auto& itr : bord)
    {
      assert((int )bT.size() == ncomp * (int )itr.second.size());
      bflds_out[bi_out++] = EOS_Field(EOS_prop_en_T[(int) itr.first][0], EOS_prop_en_T[(int) itr.first][1], (int) itr.second.size(), (double*) itr.second.begin());
    }

  for (auto& itr : inter)
    {
      assert((int )T.size() == ncomp * (int )itr.second.size());
      flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) itr.first][0], EOS_prop_en_T[(int) itr.first][1], (int) itr.second.size(), (double*) itr.second.begin());
    }

  eos_get_all_properties_T_( { { "temperature", bT }, { "pressure", bP } }, bflds_out, bferr, ncomp, id); // bords
  eos_get_all_properties_T_( { { "temperature", T }, { "pressure", P } }, flds_out, ferr, ncomp, id); // interne

  // XXX : ATTENTION : need to put back T in C
  Tc_(T), Tc_(bT);
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_all_prop_loi_F5(const MSpanD input, MLoiSpanD_h spans, int ncomp, int id, bool is_liq) const
{
#ifdef HAS_EOS
  assert(ncomp == 1);
  const SpanD P = input.at("pression"), H = is_liq ? input.at("H_L") : input.at("H_V");
  int i_out = 0, nb_out = (int) spans.size(), sz = (int) P.size();
  ArrOfInt tmp(sz);
  EOS_Error_Field ferr(tmp);
  EOS_Fields flds_out(nb_out);

  for (auto &itr : spans)
    {
      assert(sz == (int ) itr.second.size());
      flds_out[i_out++] = EOS_Field(EOS_prop_en_h[(int) itr.first][0], EOS_prop_en_h[(int) itr.first][1], (int) itr.second.size(), (double*) itr.second.begin());
    }

  EOS_Field T_fld("Enthalpy", "h", (int) H.size(), (double*) H.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
  fluide->compute(P_fld, T_fld, flds_out, ferr);

#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}
