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

#ifndef CoolProp_to_TRUST_Sat_generique_included
#define CoolProp_to_TRUST_Sat_generique_included

#include <CoolProp_to_TRUST.h>

/*! @brief classe CoolProp_to_TRUST_Sat_generique
 *
 *  Interface commune pour TRUST et ses baltiks qui permet appeler les methodes de la lib CoolProp
 *  Methods disponibles pour la saturation en temperature
 *
 *  @sa CoolProp_to_TRUST CoolProp_Supported_Models_Fluids
 *
 */
class CoolProp_to_TRUST_Sat_generique : public CoolProp_to_TRUST
{
public :
  void set_saturation_generique(const char *const model_name, const char *const fluid_name) override;

  // appels simples
  int tppi_get_T_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_p_sat_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_lvap_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_lvap_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_rho_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_rho_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_rho_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_rho_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_cp_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_cp_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  int tppi_get_cp_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  int tppi_get_cp_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  // pour les gens qui cherchent sigma de l'objet saturation
  int tppi_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const override;

  // methods particuliers par application pour gagner en performance : utilise dans Pb_Multiphase et F5 (pour le moment !)
  int tppi_get_all_flux_interfacial_pb_multiphase(const SpanD P, MSatSpanD sats, int ncomp = 1, int ind = 0) const override;
  int tppi_get_all_sat_loi_F5(const MSpanD input, MSatSpanD sats, int ncomp = 1, int ind = 0) const override;
  int tppi_get_all_sat_loi_F5_2(const MSpanD input, MSatSpanD sats, int ncomp = 1, int ind = 0) const;
  int tppi_get_all_sat_loi_F5_3(const MSpanD input, MSatSpanD sats, int ncomp = 1, int ind = 0) const;

private:
  int FD_derivative_p(SAT , const SpanD , SpanD , bool is_liq = true) const;
  int tppi_get_single_sat_p_(SAT , const SpanD , SpanD , int ncomp = 1, int id = 0, bool is_liq = true) const;
  int tppi_get_single_sat_p__(SAT , const SpanD , SpanD , bool is_liq = true) const;
};

#endif /* CoolProp_to_TRUST_Sat_generique_included */

