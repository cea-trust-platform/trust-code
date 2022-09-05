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

#ifndef Fluide_reel_base_included
#define Fluide_reel_base_included

#include <Fluide_base.h>
#include <functional>
#include <span.hpp>
#include <Param.h>
#include <vector>
#include <array>
#include <map>

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
  Declare_base(Fluide_reel_base);
public :
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

  static void calculer_masse_volumique(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  static void calculer_energie_interne(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  static void calculer_enthalpie(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);

  // TODO : REMOVE ...
  virtual double rho_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double dP_rho_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double dT_rho_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double h_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double dP_h_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double dT_h_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double cp_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double beta_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double mu_(const double T, const double P) const { return not_implemented<double>(__func__); }
  virtual double lambda_(const double T, const double P) const { return not_implemented<double>(__func__); }

  // Span version
  // densite
  virtual void rho__(const SpanD T, const SpanD P, SpanD R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual void dP_rho__(const SpanD T, const SpanD P, SpanD dP_R, int ncomp = 1, int id = 0) const  { return not_implemented<void>(__func__); }
  virtual void dT_rho__(const SpanD T, const SpanD P, SpanD dT_R, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  // enthalpie
  virtual void h__(const SpanD T, const SpanD P, SpanD H, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual void dP_h__(const SpanD T, const SpanD P, SpanD dP_H, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual void dT_h__(const SpanD T, const SpanD P, SpanD dT_H, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  // lois champs "faibles" -> pas de derivees
  virtual void cp__(const SpanD T, const SpanD P, SpanD CP, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual void beta__(const SpanD T, const SpanD P, SpanD B, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual void mu__(const SpanD T, const SpanD P, SpanD M, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }
  virtual void lambda__(const SpanD T, const SpanD P, SpanD L, int ncomp = 1, int id = 0) const { return not_implemented<void>(__func__); }

private:
  typedef void(Fluide_reel_base::*function_span_generic)(const SpanD , const SpanD , SpanD , int , int ) const;

  template <function_span_generic FUNC>
  inline void double_to_span(const double T, const double P, SpanD res) const
  {
    ArrayD Tt = {T}, Pp = {P}, res_ = {0.};
    (this->*FUNC)(SpanD(Tt), SpanD(Pp), SpanD(res_),1,0); // fill res_
    for (auto& val : res) val = res_[0]; // fill res
  }

  inline void _rho_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::rho__>(T,P,res); }
  inline void _dP_rho_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dP_rho__>(T,P,res); }
  inline void _dT_rho_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dT_rho__>(T,P,res); }
  inline void _h_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::h__>(T,P,res); }
  inline void _dP_h_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dP_h__>(T,P,res); }
  inline void _dT_h_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::dT_h__>(T,P,res); }
  inline void _cp_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::cp__>(T,P,res); }
  inline void _beta_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::beta__>(T,P,res); }
  inline void _mu_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::mu__>(T,P,res); }
  inline void _lambda_(const double T, const double P, SpanD res) const { double_to_span<&Fluide_reel_base::lambda__>(T,P,res); }

  template <typename RETURN_TYPE>
  RETURN_TYPE not_implemented(const char * nom_funct) const
  {
    std::cerr << "Fluide_reel_base::" << nom_funct << " should be implemented in a derived class !" << std::endl;
    throw;
  }

};

#endif /* Fluide_reel_base_included */
