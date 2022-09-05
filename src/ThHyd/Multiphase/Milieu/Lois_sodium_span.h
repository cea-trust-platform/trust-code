/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef lois_sodium_span_included
#define lois_sodium_span_included

#include <assert.h>
#include <span.hpp>
#include <vector>
#include <cmath>

/* Lois physiques du sodium issues de fits sur DEN/DANS/DM2S/STMF/LMES/RT/12-018/A */

// iterator index !
#define i_it std::distance(res.begin(), &val)

using VectorD = std::vector<double>;
using SpanD = tcb::span<double>;

const double Ksil = 1e-9;

/* inverse de la densite du liquide (hors pression) */
inline void IRhoL_span(const SpanD T, SpanD res, int ncomp, int id)
{
  for (auto& val : res)
    {
      const double tk = T[i_it * ncomp + id] + 273.15;
      val = (9.829728e-4 + tk * (2.641186e-7 + tk * (-3.340743e-11 + tk * 6.680973e-14)));
    }
}

/* sa derivee */
inline void DTIRhoL_span(const SpanD T, SpanD res, int ncomp, int id)
{
  for (auto& val : res)
    {
      const double tk = T[i_it * ncomp + id] + 273.15;
      val = (2.641186e-7 + tk * (-3.340743e-11 * 2 + tk * 6.680973e-14 * 3));
    }
}

inline void RhoL_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  IRhoL_span(T,res,ncomp,id);
  for (auto& val : res) val = exp(Ksil * (P[i_it] - 1e5)) / val;
}

/* derivees */
inline void DPRhoL_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  RhoL_span(T,P,res,ncomp,id);
  for (auto& val : res) val = Ksil * val;
}

inline void DTRhoL_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  VectorD invRhoL((int)res.size()), DinvRhoL((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  IRhoL_span(T,SpanD(invRhoL),ncomp,id);
  DTIRhoL_span(T,SpanD(DinvRhoL),ncomp,id);
  for (auto& val : res) val = - exp(Ksil * (P[i_it] - 1e5)) * DinvRhoL[i_it] / (invRhoL[i_it] * invRhoL[i_it]);
}

/* enthalpie du liquide */
inline void HL_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  VectorD invRhoL((int)res.size()), DinvRhoL((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  IRhoL_span(T,SpanD(invRhoL),ncomp,id);
  DTIRhoL_span(T,SpanD(DinvRhoL),ncomp,id);
  for (auto& val : res)
    {
      const double tk = T[i_it * ncomp + id] + 273.15;
      val = (2992600 / tk - 365487.2 +  tk * (1658.2 + tk * (-.42395 +  tk * 1.4847e-4)))+ (invRhoL[i_it] - tk * DinvRhoL[i_it]) * (1 - exp(Ksil*(1e5 - P[i_it]))) / Ksil;
    }
}

/* derivees */
inline void DPHL_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  VectorD invRhoL((int)res.size()), DinvRhoL((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  IRhoL_span(T,SpanD(invRhoL),ncomp,id);
  DTIRhoL_span(T,SpanD(DinvRhoL),ncomp,id);
  for (auto& val : res)
    {
      const double tk = T[i_it * ncomp + id] + 273.15;
      val = (invRhoL[i_it] - tk * DinvRhoL[i_it]) * exp(Ksil * (1e5 - P[i_it]));
    }
}

inline void DTHL_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size() && (int)T.size() == ncomp * (int)P.size());
  for (auto& val : res)
    {
      const double tk = T[i_it * ncomp + id] + 273.15;
      val = (-2992600 / (tk*tk) + 1658.2 + tk * (-.42395 * 2 + tk * 1.4847e-4 * 3))- tk * (-3.340743e-11 * 2 + tk * 6.680973e-14 * 6) * (1 - exp(Ksil*(1e5 - P[i_it]))) / Ksil;
    }
}

inline void BETAL_span(const SpanD T, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size());
  VectorD invRhoL((int)res.size()), DinvRhoL((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  IRhoL_span(T,SpanD(invRhoL),ncomp,id);
  DTIRhoL_span(T,SpanD(DinvRhoL),ncomp,id);
  for (auto& val : res) val = DinvRhoL[i_it] / invRhoL[i_it];
}

/* viscosite du liquide*/
inline void MuL_span(const SpanD T, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size());
  for (auto& val : res)
    {
      const double tk = T[i_it * ncomp + id] + 273.15;
      val = exp( -6.4406 - 0.3958 * log(tk) + 556.835 / tk );
    }
}

/* conductivite du liquide */
inline void LambdaL_span(const SpanD T, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size());
  for (auto& val : res)
    {
      const double tk = T[i_it * ncomp + id] + 273.15;
      val = std::max(124.67 + tk * (-.11381 + tk * (5.5226e-5 - tk * 1.1848e-8)), 0.045);
    }
}

/*
 * ****************************
 * **********  GAZ  ***********
 * ****************************
 */

/* temperature de saturation */
inline void Tsat_Na_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  const double A = 7.8130, B = 11209, C = 5.249e5;
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] =  2 * C / ( -B + sqrt(B*B + 4 * A * C - 4 * C * log(P[i] / 1e6))) - 273.15;
}

/* sa derivee */
inline void DTsat_Na_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  const double A = 7.8130, B = 11209, C = 5.249e5;
  Tsat_Na_span(P,res,ncomp,ind); // XXX : si c'est complique va falloir creer un tableau local ...
  for (int i =0; i < (int)P.size(); i++)
    {
      const double Tsk = res[i * ncomp + ind] + 273.15; // XXX : need in Kelvin
      res[i * ncomp + ind] = Tsk * Tsk / ( P[i] * sqrt(B*B + 4 * A * C - 4 * C * log(P[i] / 1e6)));
    }
}

/* enthalpie massique de saturation */
inline void Hsat_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  VectorD Ts_((int)P.size()), HL_((int)P.size());
  Tsat_Na_span(P,SpanD(Ts_),1,0); // 1D
  HL_span(SpanD(Ts_),P,SpanD(HL_),1,0); // 1D
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = HL_[i];
}

/* sa derivee */
inline void DHsat_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  VectorD Ts_((int)P.size()), DTs_((int)P.size()), DTHL_((int)P.size()), DPHL_((int)P.size());
  Tsat_Na_span(P,SpanD(Ts_),1,0); // 1D
  DTsat_Na_span(P,SpanD(DTs_),1,0); // 1D
  DTHL_span(SpanD(Ts_),P,SpanD(DTHL_),1,0);
  DPHL_span(SpanD(Ts_),P,SpanD(DPHL_),1,0);
  for (int i =0; i < (int)P.size(); i++)
    {
      res[i * ncomp + ind] = DTs_[i] * DTHL_[i] + DPHL_[i];
    }
}

/* densite de la vapeur : (gaz parfait) * f1(Tsat_Na) * f2(DTsat_Na)*/
#define f1 (2.49121 + Tsk * (-5.53796e-3 + Tsk * (7.5465e-6 + Tsk * (-4.20217e-9 + Tsk * 8.59212e-13))))
#define f2 (1 + dT * (-5e-4 + dT * (6.25e-7 - dT * 4.1359e-25)))
#define Df1 (-5.53796e-3 + Tsk * (2 * 7.5465e-6 + Tsk * (-3 * 4.20217e-9 + Tsk * 4 * 8.59212e-13)))
#define Df2 (-5e-4 + dT * (2 * 6.25e-7 - dT * 3 * 4.1359e-25))

inline void RhoV_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  Tsat_Na_span(P,res,1,0); // XXX : attention on utilise res
  for (auto& val : res)
    {
      const double Tsk = val + 273.15; // XXX : need in Kelvin
      const double tk = T[i_it * ncomp + id] + 273.15;
      const double dT = tk - Tsk;
      val = P[i_it] * 2.7650313e-3 * f1 * f2 / tk;
    }
}

/* ses derivees */
inline void DPRhoV_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  VectorD Tsk_((int)res.size()), DTsk_((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  Tsat_Na_span(P,SpanD(Tsk_),1,0);
  DTsat_Na_span(P,SpanD(DTsk_),1,0);
  for (auto& val : res)
    {
      const double Tsk = Tsk_[i_it] + 273.15; // XXX : need in Kelvin
      const double tk = T[i_it * ncomp + id] + 273.15;
      const double dT = tk - Tsk;
      val = 2.7650313e-3 * (f1 * f2 + P[i_it] * DTsk_[i_it] * (Df1 * f2 - f1 * Df2)) / tk;
    }
}

inline void DTRhoV_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  Tsat_Na_span(P,res,1,0); // XXX : attention on utilise res
  for (auto& val : res)
    {
      const double Tsk = val + 273.15; // XXX : need in Kelvin
      const double tk = T[i_it * ncomp + id] + 273.15;
      const double dT = tk - Tsk;
      val = P[i_it] * 2.7650313e-3 * f1 * (Df2 - f2  / tk) / tk;
    }
}

#undef f1
#undef Df1
#undef f2
#undef Df2

/* chaleur latente, a prendre a Tsat_Na */
inline void Lvap_Na_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  Tsat_Na_span(P,res,ncomp,ind); // XXX : attention on utilise res. XXX : si c'est complique va falloir creer un tableau local ...
  const double Tc = 2503.7;
  for (int i =0; i < (int)P.size(); i++)
    {
      const double Tsk =  res[i * ncomp + ind] + 273.15; // XXX : need in Kelvin
      res[i * ncomp + ind] = 3.9337e5 * ( 1 - Tsk / Tc) + 4.3986e6 * pow( 1 - Tsk / Tc, .29302);
    }
}

/* sa derivee */
inline void DLvap_Na_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  const double Tc = 2503.7;
  VectorD Tsk_((int)P.size()), DTsk_((int)P.size());
  Tsat_Na_span(P,SpanD(Tsk_),1,0); // 1D
  DTsat_Na_span(P,SpanD(DTsk_),1,0); // 1D
  for (int i =0; i < (int)P.size(); i++)
    {
      const double Tsk = Tsk_[i] + 273.15; // XXX : need in Kelvin
      res[i * ncomp + ind] = DTsk_[i] * (-3.9337e5 / Tc - 4.3986e6  * .29302 * pow( 1 - Tsk / Tc, .29302 - 1) / Tc);
    }
}

/* enthalpie massique de saturation (vapeur = Hsat(P) + Lvap_Na(P)) */
inline void HVsat_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  VectorD Hs_((int)P.size()), Lv_((int)P.size());
  Hsat_span(P,SpanD(Hs_),1,0);
  Lvap_Na_span(P,SpanD(Lv_),1,0);
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = Hs_[i] + Lv_[i];
}

/* sa derivee (vapeur = DHsat(P) + DLvap_Na(P)) */
inline void DHVsat_span(const SpanD P, SpanD res, int ncomp, int ind)
{
  assert (ncomp * (int)P.size() == (int)res.size());
  VectorD DHs_((int)P.size()), DLv_((int)P.size());
  DHsat_span(P,SpanD(DHs_),1,0);
  DLvap_Na_span(P,SpanD(DLv_),1,0);
  for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = DHs_[i] + DLv_[i];
}

/* enthalpie de la vapeur */
inline void HV_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  const double Cp0 = 910, k = -4.6e-3;
  VectorD Ts_((int)res.size()), Lvap_((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  Tsat_Na_span(P,SpanD(Ts_),1,0);
  Lvap_Na_span(P,SpanD(Lvap_),1,0);
  HL_span(SpanD(Ts_),P,res,ncomp,id); // XXX : attention on utilise res
  for (auto& val : res)
    {
      const double dT = T[i_it * ncomp + id] - Ts_[i_it]; // XXX : pas meme taille attention
      const double CpVs = (-1223.89 + Ts_[i_it] * (14.1191 + Ts_[i_it] * (-1.62025e-2 + Ts_[i_it] * 5.76923e-6)));
      val = val + Lvap_[i_it] + Cp0 * dT + (Cp0 - CpVs) * (1 - exp(k * dT)) / k;
    }
}

/* ses derivees */
inline void DPHV_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)P.size() && (int)T.size() == ncomp * (int)res.size());
  VectorD DTHL_((int)res.size()), DPHL_((int)res.size()), Ts_((int)res.size()), DTs_((int)res.size()), DLvap_((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  Tsat_Na_span(P,SpanD(Ts_),1,0);
  DTsat_Na_span(P,SpanD(DTs_),1,0);
  DLvap_Na_span(P,SpanD(DLvap_),1,0);
  DTHL_span(SpanD(Ts_),P,SpanD(DTHL_),ncomp,id);
  DPHL_span(SpanD(Ts_),P,SpanD(DPHL_),ncomp,id);

  const double Cp0 = 910, k = -4.6e-3;
  for (auto& val : res)
    {
      const double dT = T[i_it * ncomp + id] - Ts_[i_it]; // XXX : pas meme taille attention
      const double CpVs = (-1223.89 + Ts_[i_it] * (14.1191 + Ts_[i_it] * (-1.62025e-2 + Ts_[i_it] * 5.76923e-6))); // TODO : FIXME : a faire une methode
      const double DCpVs = (14.1191 + Ts_[i_it] * (- 2 * 1.62025e-2 + Ts_[i_it] * 3 * 5.76923e-6));
      val = DPHL_[i_it] + DLvap_[i_it] + DTs_[i_it] * (DTHL_[i_it] - Cp0 - DCpVs  * (1 - exp(k * dT)) / k + (Cp0 - CpVs) * exp(k * dT));
    }
}

inline void DTHV_span(const SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size() && (int)T.size() == ncomp * (int)P.size());
  const double Cp0 = 910, k = -4.6e-3;
  Tsat_Na_span(P,res,1,0); // XXX : attention on utilise res
  for (auto& val : res)
    {
      const double dT = T[i_it * ncomp + id] - val; // XXX : pas meme taille attention : val = res[i_it]
      const double CpVs = (-1223.89 + val * (14.1191 + val * (-1.62025e-2 + val * 5.76923e-6)));
      val = Cp0 - (Cp0 - CpVs) * exp(k * dT);
    }
}

/* inverse de la densite de la vapeur */
inline void IRhoV_vec(SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  RhoV_span(T,P,res,ncomp,id); // XXX : attention on utilise res
  for (auto& val : res) val = 1. / val;
}
/* ses derivees */
inline void DTIRhoV_vec(SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  VectorD RhoV_((int)res.size()), DTRhoV_((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  RhoV_span(T,P,SpanD(RhoV_),ncomp,id);
  DTRhoV_span(T,P,SpanD(DTRhoV_),ncomp,id);
  for (auto& val : res) val = -DTRhoV_[i_it] / ( RhoV_[i_it] * RhoV_[i_it] );
}

inline void BETAV_span(SpanD T, const SpanD P, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size() && (int)T.size() == ncomp * (int)P.size());
  VectorD IRhoV_((int)res.size()), DTIRhoV_((int)res.size()); // on peut utiliser res pour l'un de deux mais bon ...
  IRhoV_vec(T,P,SpanD(IRhoV_),ncomp,id);
  DTIRhoV_vec(T,P,SpanD(DTIRhoV_),ncomp,id);
  for (auto& val : res) val = DTIRhoV_[i_it] / IRhoV_[i_it];
}

/* viscosite de la vapeur */
inline void MuV_span(const SpanD T, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size());
  for (auto& val : res) val = 2.5e-6 + 1.5e-8 * (T[i_it * ncomp + id] + 273.15);
}

/* conductivite de la vapeur */
inline void LambdaV_span(const SpanD T, SpanD res, int ncomp, int id)
{
  assert ((int)T.size() == ncomp * (int)res.size());
  for (auto& val : res) val = 0.045;
}

#undef i_it
#endif /* lois_sodium_span_included */
