/****************************************************************************
 * Copyright (c) 2021, CEA
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

int CoolProp_to_TRUST_generique::tppi_get_rho_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->rhomass();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->rhomass();
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_rho_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_rho_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP);
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_h_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->hmass();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->hmass();
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_h_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_h_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP);
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_cp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->cpmass();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->cpmass();
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_mu_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->viscosity();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->viscosity();
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_generique::tppi_get_lambda_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->conductivity();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->conductivity();
        }
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

// appel simple si besoin : cas incompressible
int CoolProp_to_TRUST_generique::tppi_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  const int sz = (int )R.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PT_INPUTS, P[i], T[i]);  // SI units
        R[i] = fluide->isobaric_expansion_coefficient();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD TT(temp_);
      for (auto& val : TT) val = T[i_it * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PT_INPUTS, P[i], TT[i]);  // SI units
          R[i] = fluide->isobaric_expansion_coefficient();
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
