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

void CoolProp_to_TRUST_Sat_generique::set_CoolProp_Sat_generique(const char *const model_name, const char *const fluid_name)
{
#ifdef HAS_COOLPROP
  fluide = std::shared_ptr<AbstractState>(AbstractState::factory(std::string(model_name), std::string(fluid_name)));
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_T_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        res[i] = fluide->T();
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          RR[i] = fluide->T();
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1 /* FIXME : 1 is vapor ? OK like that ? */);  // SI units
        res[i] = fluide->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1 /* FIXME : 1 is vapor ? OK like that ? */);  // SI units
          RR[i] = fluide->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_p_sat_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert ((int)T.size() == (int)res.size());
  const int sz = (int )res.size();
  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::QT_INPUTS, 1 /* FIXME : 1 is vapor ? OK like that ? */, T[i]);  // SI units
      res[i] = fluide->saturated_vapor_keyed_output(CoolProp::iP); /* FIXME : j'utilise vapor (normalement pareil) */
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert ((int)T.size() == (int)res.size());
  const int sz = (int )res.size();
  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::QT_INPUTS, 1 /* FIXME : 1 is vapor ? OK like that ? */, T[i]);  // SI units
      res[i] = fluide->first_saturation_deriv(CoolProp::iP, CoolProp::iT);
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        res[i] = fluide->saturated_liquid_keyed_output(CoolProp::iHmass);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          RR[i] = fluide->saturated_liquid_keyed_output(CoolProp::iHmass);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        res[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          RR[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        res[i] = fluide->saturated_vapor_keyed_output(CoolProp::iHmass);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          RR[i] = fluide->saturated_vapor_keyed_output(CoolProp::iHmass);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        res[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          RR[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_lvap_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        const double hv_ = fluide->hmass();
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        const double hl_ = fluide->hmass();
        res[i] = hv_ - hl_;
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          const double hv_ = fluide->hmass();
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          const double hl_ = fluide->hmass();
          RR[i] = hv_ - hl_;
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_lvap_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        const double hv_dp_ = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        const double hl_dp_ = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
        res[i] = hv_dp_ - hl_dp_;
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          const double hv_dp_ = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          const double hl_dp_ = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);
          RR[i] = hv_dp_ - hl_dp_;
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        res[i] = fluide->saturated_liquid_keyed_output(CoolProp::iDmass);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          RR[i] = fluide->saturated_liquid_keyed_output(CoolProp::iDmass);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        res[i] = fluide->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          RR[i] = fluide->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        res[i] = fluide->saturated_vapor_keyed_output(CoolProp::iDmass);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          RR[i] = fluide->saturated_vapor_keyed_output(CoolProp::iDmass);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_rho_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        res[i] = fluide->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          RR[i] = fluide->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_l_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        res[i] = fluide->saturated_liquid_keyed_output(CoolProp::iCpmass);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          RR[i] = fluide->saturated_liquid_keyed_output(CoolProp::iCpmass);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
        res[i] = fluide->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
          RR[i] = fluide->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_v_sat_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        res[i] = fluide->saturated_vapor_keyed_output(CoolProp::iCpmass);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          RR[i] = fluide->saturated_vapor_keyed_output(CoolProp::iCpmass);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_cp_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert (ncomp * (int)P.size() == (int)res.size());
  const int sz = (int )res.size();
  if (ncomp == 1)
    for (int i = 0; i < sz; i++)
      {
        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        res[i] = fluide->saturated_liquid_keyed_output(CoolProp::iCpmass);
      }
  else /* attention stride */
    {
      VectorD temp_((int)P.size());
      SpanD RR(temp_);
      for (auto& val : RR) val = res[i_it2 * ncomp + ind];

      for (int i = 0; i < sz; i++)
        {
          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          RR[i] = fluide->saturated_liquid_keyed_output(CoolProp::iCpmass);
        }

      for (auto& val : RR) res[i_it2 * ncomp + ind] = val;
    }
  return 0; // FIXME : on suppose que tout OK
#else
  Cerr << "CoolProp_to_TRUST_Sat_generique::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

int CoolProp_to_TRUST_Sat_generique::tppi_get_sigma_pT(const SpanD P, const SpanD T, SpanD res, int ncomp, int ind) const
{
#ifdef HAS_COOLPROP
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )res.size());
  const int sz = (int )res.size();
  for (int i = 0; i < sz; i++)
    {
      fluide->update(CoolProp::PQ_INPUTS,  P[i], 0);  // SI units
      res[i] = fluide->surface_tension();
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
        Hls__[i] = fluide->hmass();
        Ts__[i] = fluide->T();
        dPTs__[i] = fluide->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
        dPHls__[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);

        fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
        Hvs__[i] = fluide->hmass();
        dPHvs__[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);

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
          Hls_[i] = fluide->hmass();
          Ts_[i] = fluide->T();
          dPTs_[i] = fluide->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
          dPHls_[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);

          fluide->update(CoolProp::PQ_INPUTS,  P[i], 1);  // SI units
          Hvs_[i] = fluide->hmass();
          dPHvs_[i] = fluide->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP);


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
