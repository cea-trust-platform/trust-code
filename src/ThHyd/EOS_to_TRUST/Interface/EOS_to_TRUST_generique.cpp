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
#include <Process.h>

using namespace NEPTUNE ;

// iterator index !
#define i_it  std::distance(TT.begin(), &val)
#define bi_it  std::distance(bTT.begin(), &bval)
#define i_itR std::distance(R.begin(), &val)

void EOS_to_TRUST_generique::set_EOS_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_EOS
  fluide = new NEPTUNE::EOS(model_name, fluid_name);
  assert (fluide->fluid_name() == fluid_name);
  fluide->set_error_handler(handler); // Set error handler
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_rho_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "rho", "rho");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "rho", "rho");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_rho_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "drhodp", "d_rho_d_p_T");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "drhodp", "d_rho_d_p_T");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_rho_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "drhodT", "d_rho_d_T_p");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "drhodT", "d_rho_d_T_p");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_h_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "enthalpie", "h");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "enthalpie", "h");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_h_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "dhdp", "d_h_d_p_T");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "dhdp", "d_h_d_p_T");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_h_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )R.size() && (int )T.size() == ncomp * (int )P.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "dhdT", "d_h_d_T_p");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "dhdT", "d_h_d_T_p");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_cp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )R.size() && (int )T.size() == ncomp * (int )P.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "cp", "cp");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "cp", "cp");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )R.size() && (int )T.size() == ncomp * (int )P.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "beta", "beta");
  else
    {
      VectorD drho_dt_((int)P.size());
      SpanD drho_dt(drho_dt_);
      VectorD rho_((int)P.size());
      SpanD rho(rho_);
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, rho, "rho", "rho");
      compute_eos_field(P, TT, rho, "drhodT", "d_rho_d_T_p");
      for (auto& val : R)
        val = drho_dt[i_itR] / rho[i_itR];
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_mu_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )R.size() && (int )T.size() == ncomp * (int )P.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "mu", "mu");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "mu", "mu");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_lambda_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )R.size() && (int )T.size() == ncomp * (int )P.size());
  if (ncomp == 1) compute_eos_field(P, T, R, "lambda", "lambda");
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      compute_eos_field(P, TT, R, "lambda", "lambda");
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_cp_mu_lambda_beta_pT(const SpanD P, const SpanD T, MSpanD prop, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )prop.size() == 4);

  // BEEM
  SpanD CP = prop.at("cp"), M = prop.at("mu"), L = prop.at("lambda"), B = prop.at("beta");
  assert((int )T.size() == ncomp * (int )CP.size() && (int )T.size() == ncomp * (int )P.size());
  assert((int )T.size() == ncomp * (int )B.size() && (int )T.size() == ncomp * (int )P.size());
  assert((int )T.size() == ncomp * (int )M.size() && (int )T.size() == ncomp * (int )P.size());
  assert((int )T.size() == ncomp * (int )L.size() && (int )T.size() == ncomp * (int )P.size());

  const int nb_out = 5; /* 5 variables to fill */
  VectorD drho_dt_((int) P.size()), rho_((int) P.size());
  SpanD drho_dt(drho_dt_), rho(rho_);
  ArrOfInt tmp((int)P.size());
  EOS_Error_Field ferr(tmp);
  EOS_Fields flds_out(nb_out);
  int i_out = 0;

  flds_out[i_out++] = EOS_Field("cp", "cp", (int) CP.size(), (double*) CP.begin());
  flds_out[i_out++] = EOS_Field("mu", "mu", (int) M.size(), (double*) M.begin());
  flds_out[i_out++] = EOS_Field("lambda", "lambda", (int) L.size(), (double*) L.begin());
  flds_out[i_out++] = EOS_Field("rho", "rho", (int) rho.size(), (double*) rho.begin());
  flds_out[i_out++] = EOS_Field("drhodT", "d_rho_d_T_p", (int) drho_dt.size(), (double*) drho_dt.begin());

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

  /* beta */
  for (int i = 0; i < (int) B.size(); i++) B[i] = drho_dt[i] / rho[i];
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST_generique::eos_get_all_pT(MSpanD inter, MSpanD bord, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )inter.size() == 8 && (int )bord.size() == 4);

  const SpanD T = inter.at("temperature"), P = inter.at("pressure"), bT = bord.at("temperature"), bP = bord.at("pressure");
  SpanD R = inter.at("rho"), dP = inter.at("dp_rho"), dT = inter.at("dT_rho"), H = inter.at("h"), dPH = inter.at("dp_h"), dTH = inter.at("dT_h"), bR = bord.at("rho"), bH = bord.at("h");

  // XXX : ATTENTION : need Kelvin
  Tk_(T), Tk_(bT);

  assert((int )bT.size() == ncomp * (int )bP.size() && (int )bT.size() == ncomp * (int )bR.size());
  assert((int )bT.size() == ncomp * (int )bP.size() && (int )bT.size() == ncomp * (int )bH.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dP.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dT.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )H.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dPH.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dTH.size());

  const int nb_out = 6, bnb_out = 2; /* 8 variables to fill */
  ArrOfInt tmp((int)P.size()), btmp((int)bP.size());
  EOS_Error_Field ferr(tmp), bferr(btmp);
  EOS_Fields flds_out(nb_out), bflds_out(bnb_out);

  int i_out = 0, bi_out = 0;
  bflds_out[bi_out++] = EOS_Field("rho", "rho", (int) bR.size(), (double*) bR.begin());
  bflds_out[bi_out++] = EOS_Field("enthalpie", "h", (int) bH.size(), (double*) bH.begin());
  flds_out[i_out++] = EOS_Field("rho", "rho", (int) R.size(), (double*) R.begin());
  flds_out[i_out++] = EOS_Field("drhodp", "d_rho_d_p_T", (int) dP.size(), (double*) dP.begin());
  flds_out[i_out++] = EOS_Field("drhodT", "d_rho_d_T_p", (int) dT.size(), (double*) dT.begin());
  flds_out[i_out++] = EOS_Field("enthalpie", "h", (int) H.size(), (double*) H.begin());
  flds_out[i_out++] = EOS_Field("dhdp", "d_h_d_p_T", (int) dPH.size(), (double*) dPH.begin());
  flds_out[i_out++] = EOS_Field("dhdT", "d_h_d_T_p", (int) dTH.size(), (double*) dTH.begin());

  if (ncomp == 1)
    {
      EOS_Field bT_fld("Temperature", "T", (int)bT.size(),(double*)bT.begin()), bP_fld("Pressure", "P", (int)bP.size(), (double*)bP.begin());
      fluide->compute(bP_fld, bT_fld, bflds_out, bferr) ;

      EOS_Field T_fld("Temperature", "T", (int)T.size(),(double*)T.begin()), P_fld("Pressure", "P", (int)P.size(), (double*)P.begin());
      fluide->compute(P_fld, T_fld, flds_out, ferr) ;
    }
  else
    {
      VectorD temp_((int)P.size()), btemp_((int)bP.size());
      SpanD TT(temp_), bTT(btemp_);
      for (auto& bval : bTT) bval = bT[bi_it * ncomp + id];
      for (auto& val : TT) val = T[i_it * ncomp + id];

      EOS_Field bT_fld("Temperature", "T", (int) bTT.size(), (double*) bTT.begin()), bP_fld("Pressure", "P", (int) bP.size(), (double*) bP.begin());
      fluide->compute(bP_fld, bT_fld, bflds_out, bferr);

      EOS_Field T_fld("Temperature", "T", (int) TT.size(), (double*) TT.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
      fluide->compute(P_fld, T_fld, flds_out, ferr);
    }

  // XXX : ATTENTION : need to put back T in C
  Tc_(T), Tc_(bT);
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}
