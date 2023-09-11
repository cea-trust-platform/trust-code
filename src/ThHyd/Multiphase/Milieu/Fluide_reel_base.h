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

#ifndef Fluide_reel_base_included
#define Fluide_reel_base_included

#include <Fluide_base.h>
#include <functional>
#include <span.hpp>
#include <Param.h>
#include <vector>
#include <array>
#include <map>

enum class Loi_en_T;

using MLoiSpanD = std::map<Loi_en_T, tcb::span<double>>;
using MSpanD = std::map<std::string, tcb::span<double>>;
using VectorD = std::vector<double>;
using ArrayD = std::array<double,1>;
using SpanD = tcb::span<double>;


/*! @brief Classe Fluide_reel_base Cette classe represente un fluide reel ainsi que
 *
 *     ses proprietes:
 *         - viscosite cinematique, (mu)
 *         - viscosite dynamique,   (nu)
 *         - masse volumique,       (rho)
 *         - diffusivite,           (alpha)
 *         - conductivite,          (lambda)
 *         - capacite calorifique,  (Cp)
 *         - dilatabilite thermique du constituant (beta_co)
 *
 * @sa Milieu_base
 */
class Fluide_reel_base: public Fluide_base
{
  Declare_base_sans_constructeur(Fluide_reel_base);
public :
  Fluide_reel_base() { converter_h_T_.set_instance(*this); }

  bool initTimeStep(double dt) override;
  int initialiser(const double temps) override;
  int check_unknown_range() const override; //verifie que chaque inconnue "inco" est entre val_min[inco] et val_max[inco]
  int is_incompressible() const override { return P_ref_ >= 0 && T_ref_ >= 0; }
  void abortTimeStep() override;
  void mettre_a_jour(double temps) override;
  void preparer_calcul() override;
  void set_param(Param& param) override;
  void discretiser(const Probleme_base& pb, const  Discretisation_base& dis) override;

  //gamme range[inco] = { min, max} : par defaut, rien a controler
  virtual std::map<std::string, std::array<double, 2>> unknown_range() const { return {}; }

protected :
  double T_ref_ = -1., P_ref_ = -1., t_init_ = -1.;
  int first_maj_ = 1;

  void calculate_fluid_properties_incompressible();
  void calculate_fluid_properties();

  // densite
  virtual void rho_(const SpanD T, const SpanD P, SpanD R, int ncomp = 1, int id = 0) const = 0;
  virtual void dP_rho_(const SpanD T, const SpanD P, SpanD dP_R, int ncomp = 1, int id = 0) const = 0;
  virtual void dT_rho_(const SpanD T, const SpanD P, SpanD dT_R, int ncomp = 1, int id = 0) const = 0;
  // enthalpie
  virtual void h_(const SpanD T, const SpanD P, SpanD H, int ncomp = 1, int id = 0) const = 0;
  virtual void dP_h_(const SpanD T, const SpanD P, SpanD dP_H, int ncomp = 1, int id = 0) const = 0;
  virtual void dT_h_(const SpanD T, const SpanD P, SpanD dT_H, int ncomp = 1, int id = 0) const = 0;
  // lois champs "faibles" -> pas de derivees
  virtual void cp_(const SpanD T, const SpanD P, SpanD CP, int ncomp = 1, int id = 0) const = 0;
  virtual void beta_(const SpanD T, const SpanD P, SpanD B, int ncomp = 1, int id = 0) const = 0;
  virtual void mu_(const SpanD T, const SpanD P, SpanD M, int ncomp = 1, int id = 0) const = 0;
  virtual void lambda_(const SpanD T, const SpanD P, SpanD L, int ncomp = 1, int id = 0) const = 0;

  // methods particuliers par application pour gagner en performance : utilise dans Pb_Multiphase (pour le moment !)
  virtual void compute_CPMLB_pb_multiphase_(const MSpanD , MLoiSpanD, int ncomp = 1, int id = 0) const;
  virtual void compute_all_pb_multiphase_(const MSpanD , MLoiSpanD, MLoiSpanD , int ncomp = 1, int id = 0) const;

  // Methods that can be called if point-to-point calculation is required
  double _rho_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::rho_>(T,P); }
  double _dP_rho_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::dP_rho_>(T,P); }
  double _dT_rho_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::dT_rho_>(T,P); }
  double _h_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::h_>(T,P); }
  double _dP_h_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::dP_h_>(T,P); }
  double _dT_h_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::dT_h_>(T,P); }
  double _cp_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::cp_>(T,P); }
  double _beta_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::beta_>(T,P); }
  double _mu_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::mu_>(T,P); }
  double _lambda_(const double T, const double P) const { return double_to_span<&Fluide_reel_base::lambda_>(T,P); }

private:
  typedef void(Fluide_reel_base::*function_span_generic)(const SpanD , const SpanD , SpanD , int , int ) const;

  template <function_span_generic FUNC>
  void double_to_span(const double T, const double P, SpanD res) const
  {
    ArrayD Tt = {T}, Pp = {P}, res_ = {0.};
    (this->*FUNC)(SpanD(Tt), SpanD(Pp), SpanD(res_),1,0); // fill res_
    for (auto& val : res) val = res_[0]; // fill res
  }

  template <function_span_generic FUNC>
  double double_to_span(const double T, const double P) const
  {
    ArrayD Tt = {T}, Pp = {P}, res_ = {0.};
    (this->*FUNC)(SpanD(Tt), SpanD(Pp), SpanD(res_),1,0); // fill res_
    return res_[0];
  }

  // pour l'incompressible
  void _rho_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::rho_>(T,P,res); }
  void _dP_rho_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dP_rho_>(T,P,res); }
  void _dT_rho_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dT_rho_>(T,P,res); }
  void _h_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::h_>(T,P,res); }
  void _dP_h_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dP_h_>(T,P,res); }
  void _dT_h_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dT_h_>(T,P,res); }
  void _cp_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::cp_>(T,P,res); }
  void _beta_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::beta_>(T,P,res); }
  void _mu_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::mu_>(T,P,res); }
  void _lambda_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::lambda_>(T,P,res); }

  void _compute_CPMLB_pb_multiphase_(MLoiSpanD ) const;
  void _compute_all_pb_multiphase_(MLoiSpanD , MLoiSpanD ) const;

public:
  /*
   * Elie Saikali : struct interne pour convertir les derivees en h a T (pour Pb_Multiphase).
   *
   * XXX : VOIR AVEC LE CAHIER D'ANTOINE SI T'ES PAS D'ACCORD (ici, au contraire d'EOS, the doc is available :-) )
   *
   * On cherche dX/dP|T et dX/dT|P
   *
   * On sait que dX = dX/dP|T dP + dX/dT|P dT = dX/dP|h dP + dX/dh|P dh
   *
   * Lets Go :
   *
   *    dX/dP|h dP + dX/dh|P dh = dX/dP|h dP + dX/dh|P {  dh/dP|T dP + dh/dT|P dT }
   *                            = { dX/dP|h + dX/dh|P * dh/dP|T } dP + { dX/dh|P * dh/dT|P } dT
   *                            = dX/dP|T dP + dX/dT|P dT
   *
   * Alors,
   *
   *    dX/dP|T = dX/dP|h + dX/dh|P * dh/dP|T
   *    dX/dT|P = dX/dh|P * dh/dT|P
   *
   *    /         \   /             \   /         \
   *    | dX/dP|T |   | 1  dh/dP|T  |   | dX/dP|h |
   *    |         | = |             | * |         |
   *    | dX/dT|P |   | 0  dh/dT|P  |   | dX/dh|P |
   *    \         /   \             /   \         /
   */

  struct H_to_T
  {
    void set_instance(const Fluide_reel_base& fld) { z_fld_ = fld; }
    void dX_dP_T(const SpanD dX_dP_h, const SpanD dX_dh_P, SpanD dX_dP);
    void dX_dT_P(const SpanD dX_dP_h, const SpanD dX_dh_P, SpanD dX_dT);

  private:
    REF(Fluide_reel_base) z_fld_;
  };

  H_to_T converter_h_T_;
};

#endif /* Fluide_reel_base_included */
