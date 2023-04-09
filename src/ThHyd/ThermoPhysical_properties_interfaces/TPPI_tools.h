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

#ifndef TPPI_tools_included
#define TPPI_tools_included

#include <Motcle.h>
#include <span.hpp>
#include <vector>
#include <map>

static constexpr double EPS = 1.e-6;

using MRange = std::map<std::string, std::array<double, 2>>;
using MSpanD = std::map<std::string, tcb::span<double>>;
using PairCharArray = std::array<const char*, 2>;
using ArrayD = std::array<double,1>;
using VectorD = std::vector<double>;
using SpanD = tcb::span<double>;

// convert Temp span to kelvin ...
inline SpanD Tk_(const SpanD& T)
{
  for (auto& itr : T) itr += 273.15;
  return T;
}

// convert Temp span to Celsius ...
inline SpanD Tc_(const SpanD& T)
{
  for (auto& itr : T) itr -= 273.15;
  return T;
}

// XXX : Elie Saikali : ATTENTION A L'ORDRE SINON RIEN VA MARCHER
enum class Loi_en_T
{
  RHO, RHO_DP, RHO_DT,
  H, H_DP, H_DT,
  CP, CP_DP, CP_DT,
  MU, MU_DP, MU_DT,
  LAMBDA, LAMBDA_DP, LAMBDA_DT,
  SIGMA, SIGMA_DP, SIGMA_DT,
  BETA /* pour l'incompressible si besoin */
};

static constexpr std::array<PairCharArray, 19> EOS_prop_en_T =
{
  {
    { "rho", "rho" }, { "drhodp", "d_rho_d_p_T" }, { "drhodT", "d_rho_d_T_p" },
    { "enthalpie", "h" }, { "dhdp", "d_h_d_p_T" }, { "dhdT", "d_h_d_T_p" },
    { "cp", "cp" }, { "dcpdp", "d_cp_d_p_T" }, { "dcpdT", "d_cp_d_T_p" },
    { "mu", "mu" }, { "dmudp", "d_mu_d_p_T" }, { "dmudT", "d_mu_d_T_p" },
    { "lambda", "lambda" }, { "dlambdadp", "d_lambda_d_p_T" }, { "dlambdadT", "d_lambda_d_T_p" },
    { "sigma", "sigma" }, { "dsigmadp", "d_sigma_d_p_T" }, { "dsigmadT", "d_sigma_d_T_p" },
    { "beta", "beta" }
  }
};

enum class Loi_en_h
{
  RHO, RHO_DP, RHO_DH,
  T, T_DP, T_DH,
  CP, CP_DP, CP_DH,
  MU, MU_DP, MU_DH,
  LAMBDA, LAMBDA_DP, LAMBDA_DH,
  SIGMA, SIGMA_DP, SIGMA_DH,
  BETA /* pour l'incompressible si besoin */
};

static constexpr std::array<PairCharArray, 19> EOS_prop_en_h =
{
  {
    { "rho", "rho" }, { "drhodp", "d_rho_d_p_h" }, { "drhodh", "d_rho_d_h_p" },
    { "temperature", "T" }, { "dTdp", "d_T_d_p_h" }, { "dTdh", "d_T_d_h_p" },
    { "cp", "cp" }, { "dcpdp", "d_cp_d_p_h" }, { "dcpdh", "d_cp_d_h_p" },
    { "mu", "mu" }, { "dmudp", "d_mu_d_p_h" }, { "dmudh", "d_mu_d_h_p" },
    { "lambda", "lambda" }, { "dlambdadp", "d_lambda_d_p_h" }, { "dlambdadh", "d_lambda_d_h_p" },
    { "sigma", "sigma" }, { "dsigmadp", "d_sigma_d_p_h" }, { "dsigmadh", "d_sigma_d_h_p" },
    { "beta", "beta" }
  }
};

enum class SAT
{
  T_SAT, T_SAT_DP,
  P_SAT, P_SAT_DT,
  HL_SAT, HL_SAT_DP,
  HV_SAT, HV_SAT_DP,
  RHOL_SAT, RHOL_SAT_DP,
  RHOV_SAT, RHOV_SAT_DP,
  CPL_SAT, CPL_SAT_DP,
  CPV_SAT, CPV_SAT_DP,
  SIGMA /* pour coco seulement */,
  LV_SAT, LV_SAT_DP /* juste dans l'enum mais pas dans prop directe */
};

static constexpr std::array<PairCharArray, 17> EOS_prop_sat =
{
  {
    { "tsat", "T_sat" }, { "dtsatdp", "d_T_sat_d_p" },
    { "psat", "P_sat" }, { "dpsatdt", "d_P_sat_d_T" },
    { "hlsat", "h_l_sat" }, { "dhlsatdp", "d_h_l_sat_d_p" },
    { "hvsat", "h_v_sat" }, { "dhvsatdp", "d_h_v_sat_d_p" },
    { "rholsat", "rho_l_sat" }, { "drholsatdp", "d_rho_l_sat_d_p" },
    { "rhovsat", "rho_v_sat" }, { "drhovsatdp", "d_rho_v_sat_d_p" },
    { "cplsat", "cp_l_sat" }, { "dcplsatdp", "d_cp_l_sat_d_p" },
    { "cpvsat", "cp_v_sat" }, {  "dcpvsatdp", "d_cp_v_sat_d_p" },
    { "sigma", "sigma" }
  }
};

using MLoiSpanD_h = std::map<Loi_en_h, tcb::span<double>>;
using MLoiSpanD = std::map<Loi_en_T, tcb::span<double>>;
using MSatSpanD = std::map<SAT, tcb::span<double>>;

#endif /* TPPI_tools_included */
