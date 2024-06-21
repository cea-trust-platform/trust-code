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

#include <EOS_to_TRUST_generique.h>

using namespace NEPTUNE ;

// iterator index !
#define i_it std::distance(TT.begin(), &val)

void EOS_to_TRUST_generique::set_fluide_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_EOS
  fluide = new NEPTUNE::EOS(model_name, fluid_name);
  fluide->set_error_handler(handler); // Set error handler
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_generique::tppi_get_single_property_T_(Loi_en_T enum_prop, const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  if (ncomp == 1)
    return compute_eos_field(P, T, R, EOS_prop_en_T[(int)enum_prop][0], EOS_prop_en_T[(int)enum_prop][1]);
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + id];
      return compute_eos_field(P, TT, R, EOS_prop_en_T[(int)enum_prop][0], EOS_prop_en_T[(int)enum_prop][1]);
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_generique::tppi_get_single_property_h_(Loi_en_h enum_prop, const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )R.size());
  assert (ncomp == 1);
  if (ncomp == 1)
    return compute_eos_field_h(P, H, R, EOS_prop_en_h[(int)enum_prop][0], EOS_prop_en_h[(int)enum_prop][1]);
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = H[i_it * ncomp + id];
      return compute_eos_field_h(P, TT, R, EOS_prop_en_h[(int)enum_prop][0], EOS_prop_en_h[(int)enum_prop][1]);
    }
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_generique::tppi_get_beta_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int id) const
{
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " is not implemented ! Use EOS_to_TRUST_generique::tppi_get_beta_pT or call the 911 for help !! " << finl;
  throw;
}

int EOS_to_TRUST_generique::tppi_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )T.size() == ncomp * (int )R.size() && (int )T.size() == ncomp * (int )P.size());
  VectorD drho_dt_((int)P.size()), rho_((int)P.size()), temp_((int)P.size());
  SpanD drho_dt(drho_dt_), rho(rho_), TT(temp_);
  int err1_, err2_;
  for (auto& val : TT) val = T[i_it * ncomp + id];
  err1_ = compute_eos_field(P, TT, rho, EOS_prop_en_T[(int) Loi_en_T::RHO][0], EOS_prop_en_T[(int) Loi_en_T::RHO][1]);
  err2_ = compute_eos_field(P, TT, drho_dt, EOS_prop_en_T[(int) Loi_en_T::RHO_DT][0], EOS_prop_en_T[(int) Loi_en_T::RHO_DT][1]);
  // fill beta ...
  for (int i = 0; i < (int) R.size(); i++) R[i] = drho_dt[i] / rho[i];
  return std::max(err1_, err2_);
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

// methodes particulieres par application pour gagner en performance : utilisees dans Pb_Multiphase (pour le moment !)
#ifdef HAS_EOS
int EOS_to_TRUST_generique::tppi_get_all_properties_T_(const MSpanD input , EOS_Fields& flds_out, EOS_Error_Field& ferr, int ncomp, int id) const
{
  const SpanD T = input.at("temperature"), P = input.at("pressure");
  if (ncomp == 1)
    {
      EOS_Field T_fld("Temperature", "T", (int) T.size(), (double*) T.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
      return (int)fluide->compute(P_fld, T_fld, flds_out, ferr);
    }
  else /* attention stride */
    {
      VectorD temp_((int) P.size());
      SpanD TT(temp_);
      for (auto &val : TT) val = T[i_it * ncomp + id];

      EOS_Field T_fld("Temperature", "T", (int) TT.size(), (double*) TT.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
      return (int)fluide->compute(P_fld, T_fld, flds_out, ferr);
    }
}

int EOS_to_TRUST_generique::tppi_get_all_properties_h_(const MSpanD input , EOS_Fields& flds_out, EOS_Error_Field& ferr, int ncomp, int id) const
{
  const SpanD T = input.at("enthalpie"), P = input.at("pressure");
  if (ncomp == 1)
    {
      EOS_Field T_fld("Enthalpy", "h", (int) T.size(), (double*) T.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
      return (int)fluide->compute(P_fld, T_fld, flds_out, ferr);
    }
  else /* attention stride */
    {
      VectorD temp_((int) P.size());
      SpanD TT(temp_);
      for (auto &val : TT) val = T[i_it * ncomp + id];

      EOS_Field T_fld("Enthalpy", "h", (int) TT.size(), (double*) TT.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
      return (int)fluide->compute(P_fld, T_fld, flds_out, ferr);
    }
}
#endif

int EOS_to_TRUST_generique::tppi_get_CPMLB_pb_multiphase_pT(const MSpanD input, MLoiSpanD prop, int ncomp, int id) const
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
      Loi_en_T prop_ = itr.first;
      SpanD span_ = itr.second;
      if (prop_ != Loi_en_T::BETA)
        flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) prop_][0], EOS_prop_en_T[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
      else /* pour beta on recalcule sans appel a beta de eos ... */
        {
          flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) Loi_en_T::RHO][0], EOS_prop_en_T[(int) Loi_en_T::RHO][1], (int) rho.size(), (double*) rho.begin());
          flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) Loi_en_T::RHO_DT][0], EOS_prop_en_T[(int) Loi_en_T::RHO_DT][1], (int) drho_dt.size(), (double*) drho_dt.begin());
        }
    }

  int err_ = tppi_get_all_properties_T_(input, flds_out, ferr, ncomp, id);

  /* beta */
  for (int i = 0; i < (int) B.size(); i++) B[i] = drho_dt[i] / rho[i];

  // XXX : ATTENTION : need to put back T in C
  Tc_(T);
  return err_;
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_generique::tppi_get_all_pb_multiphase_pT(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp, int id) const
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
      Loi_en_T prop_ = itr.first;
      SpanD span_ = itr.second;
      assert((int ) bT.size() == ncomp * (int ) span_.size());
      bflds_out[bi_out++] = EOS_Field(EOS_prop_en_T[(int) prop_][0], EOS_prop_en_T[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
    }

  for (auto& itr : inter)
    {
      Loi_en_T prop_ = itr.first;
      SpanD span_ = itr.second;
      assert((int ) T.size() == ncomp * (int ) span_.size());
      flds_out[i_out++] = EOS_Field(EOS_prop_en_T[(int) prop_][0], EOS_prop_en_T[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
    }

  int err1_ = tppi_get_all_properties_T_( { { "temperature", bT }, { "pressure", bP } }, bflds_out, bferr, ncomp, id); // bords
  int err2_ = tppi_get_all_properties_T_( { { "temperature", T }, { "pressure", P } }, flds_out, ferr, ncomp, id); // interne

  // XXX : ATTENTION : need to put back T in C
  Tc_(T), Tc_(bT);
  return std::max(err1_, err2_);
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_generique::tppi_get_all_prop_loi_F5(const MSpanD input, MLoiSpanD_h spans, int ncomp, int id, bool is_liq) const
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
      Loi_en_h prop_ = itr.first;
      SpanD span_ = itr.second;
      assert(sz == (int ) span_.size());
      flds_out[i_out++] = EOS_Field(EOS_prop_en_h[(int) prop_][0], EOS_prop_en_h[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
    }

  EOS_Field T_fld("Enthalpy", "h", (int) H.size(), (double*) H.begin()), P_fld("Pressure", "P", (int) P.size(), (double*) P.begin());
  return (int)fluide->compute(P_fld, T_fld, flds_out, ferr);

#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_generique::tppi_get_CPMLB_pb_multiphase_ph(const MSpanD input, MLoiSpanD_h prop, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert((int )prop.size() == 4 && (int )input.size() == 2);

  const SpanD H = input.at("enthalpie"), P = input.at("pressure");
  if ((int )H.size() != ncomp * (int )P.size()) Process::exit("Ah bon ? NON !");

  const int nb_out = 4; /* 5 variables to fill */
  ArrOfInt tmp((int)P.size());
  EOS_Error_Field ferr(tmp);
  EOS_Fields flds_out(nb_out);
  int i_out = 0;

  for (auto& itr : prop)
    {
      assert((int )H.size() == ncomp * (int )itr.second.size());
      Loi_en_h prop_ = itr.first;
      SpanD span_ = itr.second;
      if (prop_ != Loi_en_h::BETA)
        flds_out[i_out++] = EOS_Field(EOS_prop_en_h[(int) prop_][0], EOS_prop_en_h[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
    }

  /* beta  FIXME */
  SpanD B = prop.at(Loi_en_h::BETA);
  for (int i = 0; i < (int) B.size(); i++) B[i] = 0.;

  int err_ = tppi_get_all_properties_h_(input, flds_out, ferr, ncomp, id);

  return err_;
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST_generique::tppi_get_all_pb_multiphase_ph(const MSpanD input, MLoiSpanD_h inter, MLoiSpanD_h bord, int ncomp, int id) const
{
#ifdef HAS_EOS
  assert( (int )input.size() == 4 && (int )inter.size() == 6 && (int )bord.size() == 2);
  const SpanD H = input.at("enthalpie"), P = input.at("pressure"), bH = input.at("bord_enthalpie"), bP = input.at("bord_pressure");
  assert ((int )bH.size() == ncomp * (int )bP.size() && (int )H.size() == ncomp * (int )P.size());

  const int nb_out = (int )inter.size(), bnb_out = (int )bord.size();
  ArrOfInt tmp((int)P.size()), btmp((int)bP.size());
  EOS_Error_Field ferr(tmp), bferr(btmp);
  EOS_Fields flds_out(nb_out), bflds_out(bnb_out);

  int i_out = 0, bi_out = 0;

  for (auto& itr : bord)
    {
      Loi_en_h prop_ = itr.first;
      SpanD span_ = itr.second;
      assert((int ) bH.size() == ncomp * (int ) span_.size());
      bflds_out[bi_out++] = EOS_Field(EOS_prop_en_h[(int) prop_][0], EOS_prop_en_h[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
    }

  for (auto& itr : inter)
    {
      Loi_en_h prop_ = itr.first;
      SpanD span_ = itr.second;
      assert((int ) H.size() == ncomp * (int ) span_.size());
      flds_out[i_out++] = EOS_Field(EOS_prop_en_h[(int) prop_][0], EOS_prop_en_h[(int) prop_][1], (int) span_.size(), (double*) span_.begin());
    }

  int err1_ = tppi_get_all_properties_h_( { { "enthalpie", bH }, { "pressure", bP } }, bflds_out, bferr, ncomp, id); // bords
  int err2_ = tppi_get_all_properties_h_( { { "enthalpie", H }, { "pressure", P } }, flds_out, ferr, ncomp, id); // interne

  // XXX : put T in C !!
  SpanD T = inter.at(Loi_en_h::T), bT = bord.at(Loi_en_h::T);
  Tc_(T), Tc_(bT);

  return std::max(err1_, err2_);
#else
  Cerr << "EOS_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}
