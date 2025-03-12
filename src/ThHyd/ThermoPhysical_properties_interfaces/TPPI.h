/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef TPPI_included
#define TPPI_included

#include <TPPI_tools.h>

/*! @brief classe EOS_to_TRUST
 *
 *  Interface commune pour TRUST et ses baltiks qui permet appeler les methodes de la lib EOS/CoolProp
 *  Methods disponibles en temperature et enthalpie, egalement pour la saturation
 *
 */
class TPPI
{
public:
  TPPI() = default;
  virtual ~TPPI() = default;

  virtual void set_fluide_generique(const char *const , const char *const ) { return not_implemented<void>(__func__); }
  virtual void set_saturation_generique(const char *const , const char *const ) { return not_implemented<void>(__func__); }
  virtual void set_phase(const Motcle& ) { return not_implemented<void>(__func__); }
  virtual void set_path_refprop() { return not_implemented<void>(__func__); }
  virtual void verify_phase(const Motcle& ) { return not_implemented<void>(__func__); }

  virtual void desactivate_handler(bool op = true) { return not_implemented<void>(__func__); }

  virtual double tppi_get_p_min() const = 0;
  virtual double tppi_get_p_max() const = 0;

  virtual double tppi_get_h_min() const = 0;
  virtual double tppi_get_h_max() const = 0;

  virtual double tppi_get_T_min() const = 0;
  virtual double tppi_get_T_max() const = 0;

  virtual double tppi_get_rho_min() const = 0;
  virtual double tppi_get_rho_max() const = 0;

  virtual double tppi_get_p_crit() const = 0;
  virtual double tppi_get_h_crit() const = 0;
  virtual double tppi_get_T_crit() const = 0;
  virtual double tppi_get_rho_crit() const = 0;

  virtual MRange all_unknowns_range();

  template <typename TYPE>
  TYPE not_implemented(const char * nom_funct) const
  {
    cerr << "TPPI::" << nom_funct << " should be implemented in a derived class !" << endl;
    throw;
  }

  /*
   * ******************* *
   * Lois en temperature
   * ******************* *
   */
  virtual int tppi_get_rho_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::RHO, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_rho_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::RHO_DP, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_rho_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::RHO_DT, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_h_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::H, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_h_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::H_DP, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_h_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::H_DT, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_cp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::CP, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_cp_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::CP_DP, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_cp_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::CP_DT, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_mu_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::MU, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_mu_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::MU_DP, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_mu_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::MU_DT, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_lambda_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::LAMBDA, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_lambda_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::LAMBDA_DP, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_lambda_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::LAMBDA_DT, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::SIGMA, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_sigma_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::SIGMA_DP, P, T, R, ncomp, ind);
  }

  virtual int tppi_get_sigma_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::SIGMA_DT, P, T, R, ncomp, ind);
  }

  // appel simple si besoin : cas incompressible
  virtual int tppi_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_T_(Loi_en_T::BETA, P, T, R, ncomp, ind);
  }

  /*
   * ***************** *
   * Lois en enthalpie
   * ***************** *
   */
  virtual int tppi_get_rho_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::RHO, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_rho_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::RHO_DP, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_rho_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::RHO_DH, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_T_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::T, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_T_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::T_DP, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_T_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::T_DH, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_cp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::CP, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_cp_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::CP_DP, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_cp_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::CP_DH, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_mu_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::MU, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_mu_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::MU_DP, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_mu_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::MU_DH, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_lambda_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::LAMBDA, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_lambda_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::LAMBDA_DP, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_lambda_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::LAMBDA_DH, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_sigma_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::SIGMA, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_sigma_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::SIGMA_DP, P, H, R, ncomp, ind);
  }

  virtual int tppi_get_sigma_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_property_h_(Loi_en_h::SIGMA_DH, P, H, R, ncomp, ind);
  }

  // appel simple si besoin : cas incompressible
  virtual int tppi_get_beta_ph(const SpanD P, const SpanD H, SpanD R, int ncomp, int ind) const
  {
    return tppi_get_single_property_h_(Loi_en_h::BETA, P, H, R, ncomp, ind);
  }

  /*
   * ****************** *
   * Pour la saturation
   * ****************** *
   */
  virtual int tppi_get_T_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::T_SAT, P, res, ncomp, ind);
  }

  virtual int tppi_get_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::T_SAT_DP, P, res, ncomp, ind);
  }

  virtual int tppi_get_p_sat_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::P_SAT, T, res, ncomp, ind);
  }

  virtual int tppi_get_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::P_SAT_DT, T, res, ncomp, ind);
  }

  virtual int tppi_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::HL_SAT, P, res, ncomp, ind);
  }

  virtual int tppi_get_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::HL_SAT_DP, P, res, ncomp, ind);
  }

  virtual int tppi_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::HV_SAT, P, res, ncomp, ind, false /* vapor */);
  }

  virtual int tppi_get_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::HV_SAT_DP, P, res, ncomp, ind, false /* vapor */);
  }

  virtual int tppi_get_lvap_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::LV_SAT, P, res, ncomp, ind);
  }

  virtual int tppi_get_lvap_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::LV_SAT_DP, P, res, ncomp, ind);
  }

  virtual int tppi_get_rho_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::RHOL_SAT, P, res, ncomp, ind);
  }

  virtual int tppi_get_rho_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::RHOL_SAT_DP, P, res, ncomp, ind);
  }

  virtual int tppi_get_rho_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::RHOV_SAT, P, res, ncomp, ind, false /* vapor */);
  }

  virtual int tppi_get_rho_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::RHOV_SAT_DP, P, res, ncomp, ind, false /* vapor */);
  }

  virtual int tppi_get_cp_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::CPL_SAT, P, res, ncomp, ind);
  }

  virtual int tppi_get_cp_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::CPL_SAT_DP, P, res, ncomp, ind);
  }

  virtual int tppi_get_cp_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::CPV_SAT, P, res, ncomp, ind, false /* vapor */);
  }

  virtual int tppi_get_cp_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const
  {
    return tppi_get_single_sat_p_(SAT::CPV_SAT_DP, P, res, ncomp, ind, false /* vapor */);
  }

  virtual inline int tppi_get_lambda_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int tppi_get_lambda_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int tppi_get_lambda_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int tppi_get_lambda_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int tppi_get_mu_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int tppi_get_mu_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int tppi_get_mu_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int tppi_get_mu_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

protected:
  virtual int tppi_get_single_property_T_(Loi_en_T , const SpanD , const SpanD , SpanD , int , int ) const { return not_implemented<int>(__func__); }
  virtual int tppi_get_single_property_h_(Loi_en_h , const SpanD , const SpanD , SpanD , int , int ) const { return not_implemented<int>(__func__); }
  virtual int tppi_get_single_sat_p_(SAT , const SpanD , SpanD , int , int , bool is_liq = true) const { return not_implemented<int>(__func__); }

public:
  // methodes particulieres par application pour gagner en performance : utilisees dans Pb_Multiphase
  virtual inline int tppi_get_CPMLB_pb_multiphase_pT(const MSpanD input, MLoiSpanD prop, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int tppi_get_all_pb_multiphase_pT(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int tppi_get_CPMLB_pb_multiphase_ph(const MSpanD input, MLoiSpanD_h prop, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int tppi_get_all_pb_multiphase_ph(const MSpanD input, MLoiSpanD_h inter, MLoiSpanD_h bord, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int tppi_get_all_flux_interfacial_pb_multiphase(const SpanD P, MSatSpanD, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  // methodes particulieres par application pour gagner en performance : utilisees dans F5
  virtual inline int tppi_get_all_prop_loi_F5(const MSpanD , MLoiSpanD_h  , int ncomp = 1, int id = 0, bool is_liq = true) const { return not_implemented<int>(__func__); }
  virtual inline int tppi_get_all_sat_loi_F5(const MSpanD input, MSatSpanD sats, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual void set_user_uniform_sigma(const double ) { /* Do nothing */ }

};

#endif /* TPPI_included */
