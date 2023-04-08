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


/*! @brief struct EOS_Supported_Models_Fluids
 *
 *  Models supportes : CATHARE2, FLICA4 et REFPROP10
 *
 *  On a 851 Fluids !!!
 *
 */
struct EOS_Supported_Models_Fluids
{
  const std::array<Motcle, 3> AVAIL_MODELS = { { "CATHARE2", "FLICA4", "REFPROP10" } };

  const std::array<const char*, 3> EOS_MODELS = { { "EOS_Cathare2", "EOS_FLICA4", "EOS_Refprop10" } };

  /*
   * Si CATHARE2 : on a 20 fluids dans EOS
   */
  const std::array<Motcle, 20> AVAIL_FLUIDS_C2 =
  {
    {
      "DHOLIQUID", "DHOVAPOR",
      "HXNCLIQUID", "HXNCVAPOR",
      "IAPWSLIQUID", "IAPWSVAPOR",
      "PBLIQUID", "PBVAPOR",
      "PBBILIQUID", "PBBIVAPOR",
      "SODIUMLIQUID", "SODIUMVAPOR",
      "WATERLIQUID", "WATERVAPOR",
      "NITROGENINCONDENSABLE", "HYDROGENINCONDENSABLE",
      "OXYGENINCONDENSABLE", "AIRINCONDENSABLE",
      "ARGONINCONDENSABLE", "HELIUMINCONDENSABLE"
    }
  }; /* Majuscule car Motcle */

  const std::array<const char*, 20> EOS_FLUIDS_C2 =
  {
    {
      "DHOLiquid", "DHOVapor",
      "HXNCLiquid", "HXNCVapor",
      "IAPWSLiquid", "IAPWSVapor",
      "PBLiquid", "PBVapor",
      "PBBILiquid", "PBBIVapor",
      "SodiumLiquid", "SodiumVapor",
      "WaterLiquid", "WaterVapor",
      "NitrogenIncondensable", "HydrogenIncondensable",
      "OxygenIncondensable", "AirIncondensable",
      "ArgonIncondensable", "HeliumIncondensable"
    }
  };

  /*
   * Si FLICA4 : on a 15 fluids dans EOS (attention : xdr a ajouter ...)
   */
  const std::array<Motcle, 15> AVAIL_FLUIDS_F4 =
  {
    {
      "WATER", "WATERLIQUID", "WATERVAPOR",
      "WATER_10-210", "WATERLIQUID_10-210", "WATERVAPOR_10-210",
      "WATER_10-210-4500", "WATERLIQUID_10-210-4500", "WATERVAPOR_10-210-4500",
      "WATER_50-170", "WATERLIQUID_50-170", "WATERVAPOR_50-170",
      "WATER_140-210", "WATERLIQUID_140-210", "WATERVAPOR_140-210"
    }
  }; /* Majuscule car Motcle */

  const std::array<const char*, 15> EOS_FLUIDS_F4 =
  {
    {
      "Water", "WaterLiquid", "WaterVapor",
      "Water.10-210", "WaterLiquid.10-210", "WaterVapor.10-210",
      "Water.10-210-4500", "WaterLiquid.10-210-4500", "WaterVapor.10-210-4500",
      "Water.50-170", "WaterLiquid.50-170", "WaterVapor.50-170",
      "WaterWater.140-210", "WaterLiquidWater.140-210", "WaterVaporWater.140-210"
    }
  };

  /*
   * Si REFPROP10 : on a 816 fluids dans EOS
   */
  const std::array<Motcle, 816> AVAIL_FLUIDS_REFP10 =
  {
    {
      "13BUTADIENE", "13BUTADIENELIQUID", "13BUTADIENEVAPOR",
      "1BUTENE", "1BUTENELIQUID", "1BUTENEVAPOR",
      "1BUTYNE", "1BUTYNELIQUID", "1BUTYNEVAPOR",
      "1PENTENE", "1PENTENELIQUID", "1PENTENEVAPOR",
      "22DIMETHYLBUTANE", "22DIMETHYLBUTANELIQUID", "22DIMETHYLBUTANEVAPOR",
      "23DIMETHYLBUTANE", "23DIMETHYLBUTANELIQUID", "23DIMETHYLBUTANEVAPOR",
      "3METHYLPENTANE", "3METHYLPENTANELIQUID", "3METHYLPENTANEVAPOR",
      "ACETONE", "ACETONELIQUID", "ACETONEVAPOR",
      "ACETYLENE", "ACETYLENELIQUID", "ACETYLENEVAPOR",
      "AMMONIA", "AMMONIALIQUID", "AMMONIAVAPOR",
      "ARGON", "ARGONLIQUID", "ARGONVAPOR",
      "BENZENE", "BENZENELIQUID", "BENZENEVAPOR",
      "BUTANE", "BUTANELIQUID", "BUTANEVAPOR",
      "C11", "C11LIQUID", "C11VAPOR",
      "C12", "C12LIQUID", "C12VAPOR",
      "C16", "C16LIQUID", "C16VAPOR",
      "C1CC6", "C1CC6LIQUID", "C1CC6VAPOR",
      "C22", "C22LIQUID", "C22VAPOR",
      "C2BUTENE", "C2BUTENELIQUID", "C2BUTENEVAPOR",
      "C3CC6", "C3CC6LIQUID", "C3CC6VAPOR",
      "C4F10", "C4F10LIQUID", "C4F10VAPOR",
      "C5F12", "C5F12LIQUID", "C5F12VAPOR",
      "C6F14", "C6F14LIQUID", "C6F14VAPOR",
      "CF3I", "CF3ILIQUID", "CF3IVAPOR",
      "CHLORINE", "CHLORINELIQUID", "CHLORINEVAPOR",
      "CHLOROBENZENE", "CHLOROBENZENELIQUID", "CHLOROBENZENEVAPOR",
      "CO", "COLIQUID", "COVAPOR",
      "CO2", "CO2LIQUID", "CO2VAPOR",
      "COS", "COSLIQUID", "COSVAPOR",
      "CYCLOBUTENE", "CYCLOBUTENELIQUID", "CYCLOBUTENEVAPOR",
      "CYCLOHEX", "CYCLOHEXLIQUID", "CYCLOHEXVAPOR",
      "CYCLOPEN", "CYCLOPENLIQUID", "CYCLOPENVAPOR",
      "CYCLOPRO", "CYCLOPROLIQUID", "CYCLOPROVAPOR",
      "D2", "D2LIQUID", "D2VAPOR",
      "D2O", "D2OLIQUID", "D2OVAPOR",
      "D4", "D4LIQUID", "D4VAPOR",
      "D5", "D5LIQUID", "D5VAPOR",
      "D6", "D6LIQUID", "D6VAPOR",
      "DEA", "DEALIQUID", "DEAVAPOR",
      "DECANE", "DECANELIQUID", "DECANEVAPOR",
      "DEE", "DEELIQUID", "DEEVAPOR",
      "DMC", "DMCLIQUID", "DMCVAPOR",
      "DME", "DMELIQUID", "DMEVAPOR",
      "EBENZENE", "EBENZENELIQUID", "EBENZENEVAPOR",
      "EGLYCOL", "EGLYCOLLIQUID", "EGLYCOLVAPOR",
      "ETHANE", "ETHANELIQUID", "ETHANEVAPOR",
      "ETHANOL", "ETHANOLLIQUID", "ETHANOLVAPOR",
      "ETHYLENE", "ETHYLENELIQUID", "ETHYLENEVAPOR",
      "ETHYLENEOXIDE", "ETHYLENEOXIDELIQUID", "ETHYLENEOXIDEVAPOR",
      "FLUORINE", "FLUORINELIQUID", "FLUORINEVAPOR",
      "H2S", "H2SLIQUID", "H2SVAPOR",
      "HCL", "HCLLIQUID", "HCLVAPOR",
      "HELIUM", "HELIUMLIQUID", "HELIUMVAPOR",
      "HEPTANE", "HEPTANELIQUID", "HEPTANEVAPOR",
      "HEXANE", "HEXANELIQUID", "HEXANEVAPOR",
      "HYDROGEN", "HYDROGENLIQUID", "HYDROGENVAPOR",
      "IBUTENE", "IBUTENELIQUID", "IBUTENEVAPOR",
      "IHEXANE", "IHEXANELIQUID", "IHEXANEVAPOR",
      "IOCTANE", "IOCTANELIQUID", "IOCTANEVAPOR",
      "IPENTANE", "IPENTANELIQUID", "IPENTANEVAPOR",
      "ISOBUTAN", "ISOBUTANLIQUID", "ISOBUTANVAPOR",
      "KRYPTON", "KRYPTONLIQUID", "KRYPTONVAPOR",
      "MD2M", "MD2MLIQUID", "MD2MVAPOR",
      "MD3M", "MD3MLIQUID", "MD3MVAPOR",
      "MD4M", "MD4MLIQUID", "MD4MVAPOR",
      "MDM", "MDMLIQUID", "MDMVAPOR",
      "MEA", "MEALIQUID", "MEAVAPOR",
      "METHANE", "METHANELIQUID", "METHANEVAPOR",
      "METHANOL", "METHANOLLIQUID", "METHANOLVAPOR",
      "MLINOLEA", "MLINOLEALIQUID", "MLINOLEAVAPOR",
      "MLINOLEN", "MLINOLENLIQUID", "MLINOLENVAPOR",
      "MM", "MMLIQUID", "MMVAPOR",
      "MOLEATE", "MOLEATELIQUID", "MOLEATEVAPOR",
      "MPALMITA", "MPALMITALIQUID", "MPALMITAVAPOR",
      "MSTEARAT", "MSTEARATLIQUID", "MSTEARATVAPOR",
      "MXYLENE", "MXYLENELIQUID", "MXYLENEVAPOR",
      "N2O", "N2OLIQUID", "N2OVAPOR",
      "NEON", "NEONLIQUID", "NEONVAPOR",
      "NEOPENTN", "NEOPENTNLIQUID", "NEOPENTNVAPOR",
      "NF3", "NF3LIQUID", "NF3VAPOR",
      "NITROGEN", "NITROGENLIQUID", "NITROGENVAPOR",
      "NONANE", "NONANELIQUID", "NONANEVAPOR",
      "NOVEC649", "NOVEC649LIQUID", "NOVEC649VAPOR",
      "OCTANE", "OCTANELIQUID", "OCTANEVAPOR",
      "ORTHOHYD", "ORTHOHYDLIQUID", "ORTHOHYDVAPOR",
      "OXYGEN", "OXYGENLIQUID", "OXYGENVAPOR",
      "OXYLENE", "OXYLENELIQUID", "OXYLENEVAPOR",
      "PARAHYD", "PARAHYDLIQUID", "PARAHYDVAPOR",
      "PENTANE", "PENTANELIQUID", "PENTANEVAPOR",
      "PROPADIENE", "PROPADIENELIQUID", "PROPADIENEVAPOR",
      "PROPANE", "PROPANELIQUID", "PROPANEVAPOR",
      "PROPYLEN", "PROPYLENLIQUID", "PROPYLENVAPOR",
      "PROPYLENEOXIDE", "PROPYLENEOXIDELIQUID", "PROPYLENEOXIDEVAPOR",
      "PROPYNE", "PROPYNELIQUID", "PROPYNEVAPOR",
      "PXYLENE", "PXYLENELIQUID", "PXYLENEVAPOR",
      "R11", "R11LIQUID", "R11VAPOR",
      "R1123", "R1123LIQUID", "R1123VAPOR",
      "R113", "R113LIQUID", "R113VAPOR",
      "R114", "R114LIQUID", "R114VAPOR",
      "R115", "R115LIQUID", "R115VAPOR",
      "R116", "R116LIQUID", "R116VAPOR",
      "R12", "R12LIQUID", "R12VAPOR",
      "R1216", "R1216LIQUID", "R1216VAPOR",
      "R1224YDZ", "R1224YDZLIQUID", "R1224YDZVAPOR",
      "R123", "R123LIQUID", "R123VAPOR",
      "R1233ZDE", "R1233ZDELIQUID", "R1233ZDEVAPOR",
      "R1234YF", "R1234YFLIQUID", "R1234YFVAPOR",
      "R1234ZEE", "R1234ZEELIQUID", "R1234ZEEVAPOR",
      "R1234ZEZ", "R1234ZEZLIQUID", "R1234ZEZVAPOR",
      "R124", "R124LIQUID", "R124VAPOR",
      "R1243ZF", "R1243ZFLIQUID", "R1243ZFVAPOR",
      "R125", "R125LIQUID", "R125VAPOR",
      "R13", "R13LIQUID", "R13VAPOR",
      "R1336MZZZ", "R1336MZZZLIQUID", "R1336MZZZVAPOR",
      "R134A", "R134ALIQUID", "R134AVAPOR",
      "R14", "R14LIQUID", "R14VAPOR",
      "R141B", "R141BLIQUID", "R141BVAPOR",
      "R142B", "R142BLIQUID", "R142BVAPOR",
      "R143A", "R143ALIQUID", "R143AVAPOR",
      "R150", "R150LIQUID", "R150VAPOR",
      "R152A", "R152ALIQUID", "R152AVAPOR",
      "R161", "R161LIQUID", "R161VAPOR",
      "R21", "R21LIQUID", "R21VAPOR",
      "R218", "R218LIQUID", "R218VAPOR",
      "R22", "R22LIQUID", "R22VAPOR",
      "R227EA", "R227EALIQUID", "R227EAVAPOR",
      "R23", "R23LIQUID", "R23VAPOR",
      "R236EA", "R236EALIQUID", "R236EAVAPOR",
      "R236FA", "R236FALIQUID", "R236FAVAPOR",
      "R245CA", "R245CALIQUID", "R245CAVAPOR",
      "R245FA", "R245FALIQUID", "R245FAVAPOR",
      "R32", "R32LIQUID", "R32VAPOR",
      "R365MFC", "R365MFCLIQUID", "R365MFCVAPOR",
      "R40", "R40LIQUID", "R40VAPOR",
      "R41", "R41LIQUID", "R41VAPOR",
      "RC318", "RC318LIQUID", "RC318VAPOR",
      "RE143A", "RE143ALIQUID", "RE143AVAPOR",
      "RE245CB2", "RE245CB2LIQUID", "RE245CB2VAPOR",
      "RE245FA2", "RE245FA2LIQUID", "RE245FA2VAPOR",
      "RE347MCC", "RE347MCCLIQUID", "RE347MCCVAPOR",
      "SF6", "SF6LIQUID", "SF6VAPOR",
      "SO2", "SO2LIQUID", "SO2VAPOR",
      "T2BUTENE", "T2BUTENELIQUID", "T2BUTENEVAPOR",
      "TOLUENE", "TOLUENELIQUID", "TOLUENEVAPOR",
      "VINYLCHLORIDE", "VINYLCHLORIDELIQUID", "VINYLCHLORIDEVAPOR",
      "WATER", "WATERLIQUID", "WATERVAPOR",
      "XENON", "XENONLIQUID", "XENONVAPOR",
      "AIR_PPF", "AIR_PPFLIQUID", "AIR_PPFVAPOR",
      "R404A_PPF", "R404A_PPFLIQUID", "R404A_PPFVAPOR",
      "R407C_PPF", "R407C_PPFLIQUID", "R407C_PPFVAPOR",
      "R410A_PPF", "R410A_PPFLIQUID", "R410A_PPFVAPOR",
      "R507A_PPF", "R507A_PPFLIQUID", "R507A_PPFVAPOR",
      "AIR", "AIRLIQUID", "AIRVAPOR",
      "AMARILLO", "AMARILLOLIQUID", "AMARILLOVAPOR",
      "EKOFISK", "EKOFISKLIQUID", "EKOFISKVAPOR",
      "GLFCOAST", "GLFCOASTLIQUID", "GLFCOASTVAPOR",
      "HIGHCO2", "HIGHCO2LIQUID", "HIGHCO2VAPOR",
      "HIGHN2", "HIGHN2LIQUID", "HIGHN2VAPOR",
      "NGSAMPLE", "NGSAMPLELIQUID", "NGSAMPLEVAPOR",
      "R401A", "R401ALIQUID", "R401AVAPOR",
      "R401B", "R401BLIQUID", "R401BVAPOR",
      "R401C", "R401CLIQUID", "R401CVAPOR",
      "R402A", "R402ALIQUID", "R402AVAPOR",
      "R402B", "R402BLIQUID", "R402BVAPOR",
      "R403A", "R403ALIQUID", "R403AVAPOR",
      "R403B", "R403BLIQUID", "R403BVAPOR",
      "R404A", "R404ALIQUID", "R404AVAPOR",
      "R405A", "R405ALIQUID", "R405AVAPOR",
      "R406A", "R406ALIQUID", "R406AVAPOR",
      "R407A", "R407ALIQUID", "R407AVAPOR",
      "R407B", "R407BLIQUID", "R407BVAPOR",
      "R407C", "R407CLIQUID", "R407CVAPOR",
      "R407D", "R407DLIQUID", "R407DVAPOR",
      "R407E", "R407ELIQUID", "R407EVAPOR",
      "R407F", "R407FLIQUID", "R407FVAPOR",
      "R407G", "R407GLIQUID", "R407GVAPOR",
      "R407H", "R407HLIQUID", "R407HVAPOR",
      "R408A", "R408ALIQUID", "R408AVAPOR",
      "R409A", "R409ALIQUID", "R409AVAPOR",
      "R409B", "R409BLIQUID", "R409BVAPOR",
      "R410A", "R410ALIQUID", "R410AVAPOR",
      "R410B", "R410BLIQUID", "R410BVAPOR",
      "R411A", "R411ALIQUID", "R411AVAPOR",
      "R411B", "R411BLIQUID", "R411BVAPOR",
      "R412A", "R412ALIQUID", "R412AVAPOR",
      "R413A", "R413ALIQUID", "R413AVAPOR",
      "R414A", "R414ALIQUID", "R414AVAPOR",
      "R414B", "R414BLIQUID", "R414BVAPOR",
      "R415A", "R415ALIQUID", "R415AVAPOR",
      "R415B", "R415BLIQUID", "R415BVAPOR",
      "R416A", "R416ALIQUID", "R416AVAPOR",
      "R417A", "R417ALIQUID", "R417AVAPOR",
      "R417B", "R417BLIQUID", "R417BVAPOR",
      "R417C", "R417CLIQUID", "R417CVAPOR",
      "R418A", "R418ALIQUID", "R418AVAPOR",
      "R419A", "R419ALIQUID", "R419AVAPOR",
      "R419B", "R419BLIQUID", "R419BVAPOR",
      "R420A", "R420ALIQUID", "R420AVAPOR",
      "R421A", "R421ALIQUID", "R421AVAPOR",
      "R421B", "R421BLIQUID", "R421BVAPOR",
      "R422A", "R422ALIQUID", "R422AVAPOR",
      "R422B", "R422BLIQUID", "R422BVAPOR",
      "R422C", "R422CLIQUID", "R422CVAPOR",
      "R422D", "R422DLIQUID", "R422DVAPOR",
      "R422E", "R422ELIQUID", "R422EVAPOR",
      "R423A", "R423ALIQUID", "R423AVAPOR",
      "R424A", "R424ALIQUID", "R424AVAPOR",
      "R425A", "R425ALIQUID", "R425AVAPOR",
      "R426A", "R426ALIQUID", "R426AVAPOR",
      "R427A", "R427ALIQUID", "R427AVAPOR",
      "R428A", "R428ALIQUID", "R428AVAPOR",
      "R429A", "R429ALIQUID", "R429AVAPOR",
      "R430A", "R430ALIQUID", "R430AVAPOR",
      "R431A", "R431ALIQUID", "R431AVAPOR",
      "R432A", "R432ALIQUID", "R432AVAPOR",
      "R433A", "R433ALIQUID", "R433AVAPOR",
      "R433B", "R433BLIQUID", "R433BVAPOR",
      "R433C", "R433CLIQUID", "R433CVAPOR",
      "R434A", "R434ALIQUID", "R434AVAPOR",
      "R435A", "R435ALIQUID", "R435AVAPOR",
      "R436A", "R436ALIQUID", "R436AVAPOR",
      "R436B", "R436BLIQUID", "R436BVAPOR",
      "R437A", "R437ALIQUID", "R437AVAPOR",
      "R438A", "R438ALIQUID", "R438AVAPOR",
      "R439A", "R439ALIQUID", "R439AVAPOR",
      "R440A", "R440ALIQUID", "R440AVAPOR",
      "R441A", "R441ALIQUID", "R441AVAPOR",
      "R442A", "R442ALIQUID", "R442AVAPOR",
      "R443A", "R443ALIQUID", "R443AVAPOR",
      "R444A", "R444ALIQUID", "R444AVAPOR",
      "R444B", "R444BLIQUID", "R444BVAPOR",
      "R445A", "R445ALIQUID", "R445AVAPOR",
      "R446A", "R446ALIQUID", "R446AVAPOR",
      "R447A", "R447ALIQUID", "R447AVAPOR",
      "R447B", "R447BLIQUID", "R447BVAPOR",
      "R448A", "R448ALIQUID", "R448AVAPOR",
      "R449A", "R449ALIQUID", "R449AVAPOR",
      "R449B", "R449BLIQUID", "R449BVAPOR",
      "R449C", "R449CLIQUID", "R449CVAPOR",
      "R450A", "R450ALIQUID", "R450AVAPOR",
      "R451A", "R451ALIQUID", "R451AVAPOR",
      "R451B", "R451BLIQUID", "R451BVAPOR",
      "R452A", "R452ALIQUID", "R452AVAPOR",
      "R452B", "R452BLIQUID", "R452BVAPOR",
      "R452C", "R452CLIQUID", "R452CVAPOR",
      "R453A", "R453ALIQUID", "R453AVAPOR",
      "R454A", "R454ALIQUID", "R454AVAPOR",
      "R454B", "R454BLIQUID", "R454BVAPOR",
      "R454C", "R454CLIQUID", "R454CVAPOR",
      "R455A", "R455ALIQUID", "R455AVAPOR",
      "R456A", "R456ALIQUID", "R456AVAPOR",
      "R457A", "R457ALIQUID", "R457AVAPOR",
      "R458A", "R458ALIQUID", "R458AVAPOR",
      "R459A", "R459ALIQUID", "R459AVAPOR",
      "R459B", "R459BLIQUID", "R459BVAPOR",
      "R460A", "R460ALIQUID", "R460AVAPOR",
      "R460B", "R460BLIQUID", "R460BVAPOR",
      "R500", "R500LIQUID", "R500VAPOR",
      "R501", "R501LIQUID", "R501VAPOR",
      "R502", "R502LIQUID", "R502VAPOR",
      "R503", "R503LIQUID", "R503VAPOR",
      "R504", "R504LIQUID", "R504VAPOR",
      "R507A", "R507ALIQUID", "R507AVAPOR",
      "R508A", "R508ALIQUID", "R508AVAPOR",
      "R508B", "R508BLIQUID", "R508BVAPOR",
      "R509A", "R509ALIQUID", "R509AVAPOR",
      "R510A", "R510ALIQUID", "R510AVAPOR",
      "R511A", "R511ALIQUID", "R511AVAPOR",
      "R512A", "R512ALIQUID", "R512AVAPOR",
      "R513A", "R513ALIQUID", "R513AVAPOR",
      "R513B", "R513BLIQUID", "R513BVAPOR",
      "R515A", "R515ALIQUID", "R515AVAPOR"
    }
  }; /* Majuscule car Motcle */

  const std::array<const char*, 816> EOS_FLUIDS_REFP10 =
  {
    {
      "13butadiene", "13butadieneLiquid", "13butadieneVapor",
      "1butene", "1buteneLiquid", "1buteneVapor",
      "1butyne", "1butyneLiquid", "1butyneVapor",
      "1pentene", "1penteneLiquid", "1penteneVapor",
      "22dimethylbutane", "22dimethylbutaneLiquid", "22dimethylbutaneVapor",
      "23dimethylbutane", "23dimethylbutaneLiquid", "23dimethylbutaneVapor",
      "3methylpentane", "3methylpentaneLiquid", "3methylpentaneVapor",
      "Acetone", "AcetoneLiquid", "AcetoneVapor",
      "Acetylene", "AcetyleneLiquid", "AcetyleneVapor",
      "Ammonia", "AmmoniaLiquid", "AmmoniaVapor",
      "Argon", "ArgonLiquid", "ArgonVapor",
      "Benzene", "BenzeneLiquid", "BenzeneVapor",
      "Butane", "ButaneLiquid", "ButaneVapor",
      "C11", "C11Liquid", "C11Vapor",
      "C12", "C12Liquid", "C12Vapor",
      "C16", "C16Liquid", "C16Vapor",
      "C1cc6", "C1cc6Liquid", "C1cc6Vapor",
      "C22", "C22Liquid", "C22Vapor",
      "C2butene", "C2buteneLiquid", "C2buteneVapor",
      "C3cc6", "C3cc6Liquid", "C3cc6Vapor",
      "C4f10", "C4f10Liquid", "C4f10Vapor",
      "C5f12", "C5f12Liquid", "C5f12Vapor",
      "C6f14", "C6f14Liquid", "C6f14Vapor",
      "Cf3i", "Cf3iLiquid", "Cf3iVapor",
      "Chlorine", "ChlorineLiquid", "ChlorineVapor",
      "Chlorobenzene", "ChlorobenzeneLiquid", "ChlorobenzeneVapor",
      "Co", "CoLiquid", "CoVapor",
      "Co2", "Co2Liquid", "Co2Vapor",
      "Cos", "CosLiquid", "CosVapor",
      "Cyclobutene", "CyclobuteneLiquid", "CyclobuteneVapor",
      "Cyclohex", "CyclohexLiquid", "CyclohexVapor",
      "Cyclopen", "CyclopenLiquid", "CyclopenVapor",
      "Cyclopro", "CycloproLiquid", "CycloproVapor",
      "D2", "D2Liquid", "D2Vapor",
      "D2o", "D2oLiquid", "D2oVapor",
      "D4", "D4Liquid", "D4Vapor",
      "D5", "D5Liquid", "D5Vapor",
      "D6", "D6Liquid", "D6Vapor",
      "Dea", "DeaLiquid", "DeaVapor",
      "Decane", "DecaneLiquid", "DecaneVapor",
      "Dee", "DeeLiquid", "DeeVapor",
      "Dmc", "DmcLiquid", "DmcVapor",
      "Dme", "DmeLiquid", "DmeVapor",
      "Ebenzene", "EbenzeneLiquid", "EbenzeneVapor",
      "Eglycol", "EglycolLiquid", "EglycolVapor",
      "Ethane", "EthaneLiquid", "EthaneVapor",
      "Ethanol", "EthanolLiquid", "EthanolVapor",
      "Ethylene", "EthyleneLiquid", "EthyleneVapor",
      "Ethyleneoxide", "EthyleneoxideLiquid", "EthyleneoxideVapor",
      "Fluorine", "FluorineLiquid", "FluorineVapor",
      "H2s", "H2sLiquid", "H2sVapor",
      "Hcl", "HclLiquid", "HclVapor",
      "Helium", "HeliumLiquid", "HeliumVapor",
      "Heptane", "HeptaneLiquid", "HeptaneVapor",
      "Hexane", "HexaneLiquid", "HexaneVapor",
      "Hydrogen", "HydrogenLiquid", "HydrogenVapor",
      "Ibutene", "IbuteneLiquid", "IbuteneVapor",
      "Ihexane", "IhexaneLiquid", "IhexaneVapor",
      "Ioctane", "IoctaneLiquid", "IoctaneVapor",
      "Ipentane", "IpentaneLiquid", "IpentaneVapor",
      "Isobutan", "IsobutanLiquid", "IsobutanVapor",
      "Krypton", "KryptonLiquid", "KryptonVapor",
      "Md2m", "Md2mLiquid", "Md2mVapor",
      "Md3m", "Md3mLiquid", "Md3mVapor",
      "Md4m", "Md4mLiquid", "Md4mVapor",
      "Mdm", "MdmLiquid", "MdmVapor",
      "Mea", "MeaLiquid", "MeaVapor",
      "Methane", "MethaneLiquid", "MethaneVapor",
      "Methanol", "MethanolLiquid", "MethanolVapor",
      "Mlinolea", "MlinoleaLiquid", "MlinoleaVapor",
      "Mlinolen", "MlinolenLiquid", "MlinolenVapor",
      "Mm", "MmLiquid", "MmVapor",
      "Moleate", "MoleateLiquid", "MoleateVapor",
      "Mpalmita", "MpalmitaLiquid", "MpalmitaVapor",
      "Mstearat", "MstearatLiquid", "MstearatVapor",
      "Mxylene", "MxyleneLiquid", "MxyleneVapor",
      "N2o", "N2oLiquid", "N2oVapor",
      "Neon", "NeonLiquid", "NeonVapor",
      "Neopentn", "NeopentnLiquid", "NeopentnVapor",
      "Nf3", "Nf3Liquid", "Nf3Vapor",
      "Nitrogen", "NitrogenLiquid", "NitrogenVapor",
      "Nonane", "NonaneLiquid", "NonaneVapor",
      "Novec649", "Novec649Liquid", "Novec649Vapor",
      "Octane", "OctaneLiquid", "OctaneVapor",
      "Orthohyd", "OrthohydLiquid", "OrthohydVapor",
      "Oxygen", "OxygenLiquid", "OxygenVapor",
      "Oxylene", "OxyleneLiquid", "OxyleneVapor",
      "Parahyd", "ParahydLiquid", "ParahydVapor",
      "Pentane", "PentaneLiquid", "PentaneVapor",
      "Propadiene", "PropadieneLiquid", "PropadieneVapor",
      "Propane", "PropaneLiquid", "PropaneVapor",
      "Propylen", "PropylenLiquid", "PropylenVapor",
      "Propyleneoxide", "PropyleneoxideLiquid", "PropyleneoxideVapor",
      "Propyne", "PropyneLiquid", "PropyneVapor",
      "Pxylene", "PxyleneLiquid", "PxyleneVapor",
      "R11", "R11Liquid", "R11Vapor",
      "R1123", "R1123Liquid", "R1123Vapor",
      "R113", "R113Liquid", "R113Vapor",
      "R114", "R114Liquid", "R114Vapor",
      "R115", "R115Liquid", "R115Vapor",
      "R116", "R116Liquid", "R116Vapor",
      "R12", "R12Liquid", "R12Vapor",
      "R1216", "R1216Liquid", "R1216Vapor",
      "R1224ydz", "R1224ydzLiquid", "R1224ydzVapor",
      "R123", "R123Liquid", "R123Vapor",
      "R1233zde", "R1233zdeLiquid", "R1233zdeVapor",
      "R1234yf", "R1234yfLiquid", "R1234yfVapor",
      "R1234zee", "R1234zeeLiquid", "R1234zeeVapor",
      "R1234zez", "R1234zezLiquid", "R1234zezVapor",
      "R124", "R124Liquid", "R124Vapor",
      "R1243zf", "R1243zfLiquid", "R1243zfVapor",
      "R125", "R125Liquid", "R125Vapor",
      "R13", "R13Liquid", "R13Vapor",
      "R1336mzzz", "R1336mzzzLiquid", "R1336mzzzVapor",
      "R134a", "R134aLiquid", "R134aVapor",
      "R14", "R14Liquid", "R14Vapor",
      "R141b", "R141bLiquid", "R141bVapor",
      "R142b", "R142bLiquid", "R142bVapor",
      "R143a", "R143aLiquid", "R143aVapor",
      "R150", "R150Liquid", "R150Vapor",
      "R152a", "R152aLiquid", "R152aVapor",
      "R161", "R161Liquid", "R161Vapor",
      "R21", "R21Liquid", "R21Vapor",
      "R218", "R218Liquid", "R218Vapor",
      "R22", "R22Liquid", "R22Vapor",
      "R227ea", "R227eaLiquid", "R227eaVapor",
      "R23", "R23Liquid", "R23Vapor",
      "R236ea", "R236eaLiquid", "R236eaVapor",
      "R236fa", "R236faLiquid", "R236faVapor",
      "R245ca", "R245caLiquid", "R245caVapor",
      "R245fa", "R245faLiquid", "R245faVapor",
      "R32", "R32Liquid", "R32Vapor",
      "R365mfc", "R365mfcLiquid", "R365mfcVapor",
      "R40", "R40Liquid", "R40Vapor",
      "R41", "R41Liquid", "R41Vapor",
      "Rc318", "Rc318Liquid", "Rc318Vapor",
      "Re143a", "Re143aLiquid", "Re143aVapor",
      "Re245cb2", "Re245cb2Liquid", "Re245cb2Vapor",
      "Re245fa2", "Re245fa2Liquid", "Re245fa2Vapor",
      "Re347mcc", "Re347mccLiquid", "Re347mccVapor",
      "Sf6", "Sf6Liquid", "Sf6Vapor",
      "So2", "So2Liquid", "So2Vapor",
      "T2butene", "T2buteneLiquid", "T2buteneVapor",
      "Toluene", "TolueneLiquid", "TolueneVapor",
      "Vinylchloride", "VinylchlorideLiquid", "VinylchlorideVapor",
      "Water", "WaterLiquid", "WaterVapor",
      "Xenon", "XenonLiquid", "XenonVapor",
      "Air_ppf", "Air_ppfLiquid", "Air_ppfVapor",
      "R404a_ppf", "R404a_ppfLiquid", "R404a_ppfVapor",
      "R407c_ppf", "R407c_ppfLiquid", "R407c_ppfVapor",
      "R410a_ppf", "R410a_ppfLiquid", "R410a_ppfVapor",
      "R507a_ppf", "R507a_ppfLiquid", "R507a_ppfVapor",
      "Air", "AirLiquid", "AirVapor",
      "Amarillo", "AmarilloLiquid", "AmarilloVapor",
      "Ekofisk", "EkofiskLiquid", "EkofiskVapor",
      "Glfcoast", "GlfcoastLiquid", "GlfcoastVapor",
      "Highco2", "Highco2Liquid", "Highco2Vapor",
      "Highn2", "Highn2Liquid", "Highn2Vapor",
      "Ngsample", "NgsampleLiquid", "NgsampleVapor",
      "R401a", "R401aLiquid", "R401aVapor",
      "R401b", "R401bLiquid", "R401bVapor",
      "R401c", "R401cLiquid", "R401cVapor",
      "R402a", "R402aLiquid", "R402aVapor",
      "R402b", "R402bLiquid", "R402bVapor",
      "R403a", "R403aLiquid", "R403aVapor",
      "R403b", "R403bLiquid", "R403bVapor",
      "R404a", "R404aLiquid", "R404aVapor",
      "R405a", "R405aLiquid", "R405aVapor",
      "R406a", "R406aLiquid", "R406aVapor",
      "R407a", "R407aLiquid", "R407aVapor",
      "R407b", "R407bLiquid", "R407bVapor",
      "R407c", "R407cLiquid", "R407cVapor",
      "R407d", "R407dLiquid", "R407dVapor",
      "R407e", "R407eLiquid", "R407eVapor",
      "R407f", "R407fLiquid", "R407fVapor",
      "R407g", "R407gLiquid", "R407gVapor",
      "R407h", "R407hLiquid", "R407hVapor",
      "R408a", "R408aLiquid", "R408aVapor",
      "R409a", "R409aLiquid", "R409aVapor",
      "R409b", "R409bLiquid", "R409bVapor",
      "R410a", "R410aLiquid", "R410aVapor",
      "R410b", "R410bLiquid", "R410bVapor",
      "R411a", "R411aLiquid", "R411aVapor",
      "R411b", "R411bLiquid", "R411bVapor",
      "R412a", "R412aLiquid", "R412aVapor",
      "R413a", "R413aLiquid", "R413aVapor",
      "R414a", "R414aLiquid", "R414aVapor",
      "R414b", "R414bLiquid", "R414bVapor",
      "R415a", "R415aLiquid", "R415aVapor",
      "R415b", "R415bLiquid", "R415bVapor",
      "R416a", "R416aLiquid", "R416aVapor",
      "R417a", "R417aLiquid", "R417aVapor",
      "R417b", "R417bLiquid", "R417bVapor",
      "R417c", "R417cLiquid", "R417cVapor",
      "R418a", "R418aLiquid", "R418aVapor",
      "R419a", "R419aLiquid", "R419aVapor",
      "R419b", "R419bLiquid", "R419bVapor",
      "R420a", "R420aLiquid", "R420aVapor",
      "R421a", "R421aLiquid", "R421aVapor",
      "R421b", "R421bLiquid", "R421bVapor",
      "R422a", "R422aLiquid", "R422aVapor",
      "R422b", "R422bLiquid", "R422bVapor",
      "R422c", "R422cLiquid", "R422cVapor",
      "R422d", "R422dLiquid", "R422dVapor",
      "R422e", "R422eLiquid", "R422eVapor",
      "R423a", "R423aLiquid", "R423aVapor",
      "R424a", "R424aLiquid", "R424aVapor",
      "R425a", "R425aLiquid", "R425aVapor",
      "R426a", "R426aLiquid", "R426aVapor",
      "R427a", "R427aLiquid", "R427aVapor",
      "R428a", "R428aLiquid", "R428aVapor",
      "R429a", "R429aLiquid", "R429aVapor",
      "R430a", "R430aLiquid", "R430aVapor",
      "R431a", "R431aLiquid", "R431aVapor",
      "R432a", "R432aLiquid", "R432aVapor",
      "R433a", "R433aLiquid", "R433aVapor",
      "R433b", "R433bLiquid", "R433bVapor",
      "R433c", "R433cLiquid", "R433cVapor",
      "R434a", "R434aLiquid", "R434aVapor",
      "R435a", "R435aLiquid", "R435aVapor",
      "R436a", "R436aLiquid", "R436aVapor",
      "R436b", "R436bLiquid", "R436bVapor",
      "R437a", "R437aLiquid", "R437aVapor",
      "R438a", "R438aLiquid", "R438aVapor",
      "R439a", "R439aLiquid", "R439aVapor",
      "R440a", "R440aLiquid", "R440aVapor",
      "R441a", "R441aLiquid", "R441aVapor",
      "R442a", "R442aLiquid", "R442aVapor",
      "R443a", "R443aLiquid", "R443aVapor",
      "R444a", "R444aLiquid", "R444aVapor",
      "R444b", "R444bLiquid", "R444bVapor",
      "R445a", "R445aLiquid", "R445aVapor",
      "R446a", "R446aLiquid", "R446aVapor",
      "R447a", "R447aLiquid", "R447aVapor",
      "R447b", "R447bLiquid", "R447bVapor",
      "R448a", "R448aLiquid", "R448aVapor",
      "R449a", "R449aLiquid", "R449aVapor",
      "R449b", "R449bLiquid", "R449bVapor",
      "R449c", "R449cLiquid", "R449cVapor",
      "R450a", "R450aLiquid", "R450aVapor",
      "R451a", "R451aLiquid", "R451aVapor",
      "R451b", "R451bLiquid", "R451bVapor",
      "R452a", "R452aLiquid", "R452aVapor",
      "R452b", "R452bLiquid", "R452bVapor",
      "R452c", "R452cLiquid", "R452cVapor",
      "R453a", "R453aLiquid", "R453aVapor",
      "R454a", "R454aLiquid", "R454aVapor",
      "R454b", "R454bLiquid", "R454bVapor",
      "R454c", "R454cLiquid", "R454cVapor",
      "R455a", "R455aLiquid", "R455aVapor",
      "R456a", "R456aLiquid", "R456aVapor",
      "R457a", "R457aLiquid", "R457aVapor",
      "R458a", "R458aLiquid", "R458aVapor",
      "R459a", "R459aLiquid", "R459aVapor",
      "R459b", "R459bLiquid", "R459bVapor",
      "R460a", "R460aLiquid", "R460aVapor",
      "R460b", "R460bLiquid", "R460bVapor",
      "R500", "R500Liquid", "R500Vapor",
      "R501", "R501Liquid", "R501Vapor",
      "R502", "R502Liquid", "R502Vapor",
      "R503", "R503Liquid", "R503Vapor",
      "R504", "R504Liquid", "R504Vapor",
      "R507a", "R507aLiquid", "R507aVapor",
      "R508a", "R508aLiquid", "R508aVapor",
      "R508b", "R508bLiquid", "R508bVapor",
      "R509a", "R509aLiquid", "R509aVapor",
      "R510a", "R510aLiquid", "R510aVapor",
      "R511a", "R511aLiquid", "R511aVapor",
      "R512a", "R512aLiquid", "R512aVapor",
      "R513a", "R513aLiquid", "R513aVapor",
      "R513b", "R513bLiquid", "R513bVapor",
      "R515a", "R515aLiquid", "R515aVapor"
    }
  };
};

#endif /* TPPI_tools_included */
