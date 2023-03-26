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

#ifndef EOS_to_TRUST_included
#define EOS_to_TRUST_included

#include <EOS_to_TRUST_tools.h>
#include <eos++.h>

#ifdef HAS_EOS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <EOS/API/EOS_Std_Error_Handler.hxx>
#include <EOS/API/EOS.hxx>
#pragma GCC diagnostic pop
#endif

namespace NEPTUNE { class EOS; }

/*! @brief classe EOS_to_TRUST
 *
 *  Interface commune pour TRUST et ses baltiks qui permet appeler les methodes de la lib EOS
 *  Methods disponibles en temperature et enthalpie, egalement pour la saturation
 *
 *  On supporte 851 Fluids !!!
 *
 *  @sa EOS_Supported_Models_Fluids
 *
 */
class EOS_to_TRUST
{
public :
  EOS_to_TRUST() = default;
  virtual ~EOS_to_TRUST() { /* delete fluide; */ }

  void desactivate_handler(bool op = true);
  void verify_model_fluid(const Motcle& model_name, const Motcle& fluid_name);

  int get_model_index(const Motcle& model_name);
  int get_fluid_index(const Motcle& model_name, const Motcle& fluid_name);
  const char* get_eos_model_name(const int ind);
  const char* get_eos_fluid_name(const Motcle& model_name, const int ind);

  double eos_get_p_min() const;
  double eos_get_p_max() const;

  double eos_get_h_min() const;
  double eos_get_h_max() const;

  double eos_get_T_min() const;
  double eos_get_T_max() const;

  double eos_get_rho_min() const;
  double eos_get_rho_max() const;

  double eos_get_p_crit() const;
  double eos_get_h_crit() const;
  double eos_get_T_crit() const;
  double eos_get_rho_crit() const;

  MRange all_unknowns_range();

  /*
   * ******************* *
   * Lois en temperature
   * ******************* *
   */
  // appels simples
  virtual inline int eos_get_rho_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_rho_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_rho_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_h_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_h_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_h_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_cp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_cp_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_cp_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_mu_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_mu_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_mu_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_lambda_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_lambda_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_lambda_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_sigma_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_sigma_dp_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_sigma_dT_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  // appel simple si besoin : cas incompressible
  virtual inline int eos_get_beta_pT(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  // methods particuliers par application pour gagner en performance : utilise dans Pb_Multiphase (pour le moment !)
  virtual inline int eos_get_CPMLB_pb_multiphase_pT(const MSpanD input, MLoiSpanD prop, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_all_pb_multiphase_pT(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  /*
   * ***************** *
   * Lois en enthalpie
   * ***************** *
   */
  // appels simples
  virtual inline int eos_get_rho_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_rho_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_rho_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_T_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_T_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_T_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_cp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_cp_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_cp_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_mu_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_mu_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_mu_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_lambda_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_lambda_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_lambda_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_sigma_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_sigma_dp_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_sigma_dh_ph(const SpanD P, const SpanD H, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  // appel simple si besoin : cas incompressible
  virtual inline int eos_get_beta_ph(const SpanD P, const SpanD T, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

  // methods particuliers par application pour gagner en performance : utilise dans F5 (pour le moment !)
  virtual inline int eos_get_all_prop_loi_F5(const MSpanD , MLoiSpanD_h  , int ncomp = 1, int id = 0, bool is_liq = true) const { return not_implemented<int>(__func__); }

  /*
   * ****************** *
   * Pour la saturation
   * ****************** *
   */
  // appels simples
  virtual inline int eos_get_T_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_T_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_p_sat_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_p_sat_d_T_T(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_lvap_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_lvap_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_h_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_h_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_h_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_h_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_rho_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_rho_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_rho_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_rho_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_cp_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_cp_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_cp_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_cp_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_lambda_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_lambda_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_lambda_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_lambda_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_mu_l_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_mu_l_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  virtual inline int eos_get_mu_v_sat_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_mu_v_sat_d_p_p(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const { return not_implemented<int>(__func__); }

  // methods particuliers par application pour gagner en performance : utilise dans Pb_Multiphase et F5 (pour le moment !)
  virtual inline int eos_get_all_flux_interfacial_pb_multiphase(const SpanD P, MSatSpanD, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }
  virtual inline int eos_get_all_sat_loi_F5(const MSpanD input, MSatSpanD sats, int ncomp = 1, int id = 0) const { return not_implemented<int>(__func__); }

protected :

#ifdef HAS_EOS
  NEPTUNE::EOS_Std_Error_Handler handler ;
  NEPTUNE::EOS *fluide = nullptr;
#endif

  EOS_Supported_Models_Fluids supp_;

  int compute_eos_field(const SpanD P, SpanD res, const char *const pt, const char *const pn, bool is_T = false) const;
  int compute_eos_field(const SpanD P, const SpanD T, SpanD res, const char *const pt, const char *const pn) const;
  int compute_eos_field_h(const SpanD P, const SpanD H, SpanD res, const char *const pt, const char *const pn) const;

  template <typename TYPE>
  TYPE not_implemented(const char * nom_funct) const
  {
    cerr << "EOS_to_TRUST::" << nom_funct << " should be implemented in a derived class !" << endl;
    throw;
  }

private:

#ifdef HAS_EOS
  double tmin_ = -123., tmax_ = -123., pmin_ = -123., pmax_ = -123.;
  double hmin_ = -123., hmax_ = -123., rhomin_ = -123., rhomax_ = -123.;
#endif
};

#endif /* EOS_to_TRUST_included */
