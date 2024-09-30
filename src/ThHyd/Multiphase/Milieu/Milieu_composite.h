/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Milieu_composite_included
#define Milieu_composite_included

#include <Saturation_base.h>
#include <Interface_base.h>
#include <TRUST_Deriv.h>
#include <Fluide_base.h>
#include <vector>
#include <set>

/*! @brief Classe Milieu_composite Cette classe represente un fluide reel ainsi que
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
class Milieu_composite: public Fluide_base
{
  Declare_instanciable(Milieu_composite);
public :
  int check_unknown_range() const override;
  int initialiser(const double temps) override;

  bool initTimeStep(double dt) override;
  bool has_saturation(int k, int l) const;
  bool has_interface(int k, int l) const;
  inline bool has_saturation() const { return has_saturation_; }
  inline bool has_interface() const { return has_interface_; }

  void discretiser(const Probleme_base& pb, const  Discretisation_base& dis) override;
  void abortTimeStep() override;
  void preparer_calcul() override;
  void mettre_a_jour(double temps) override;
  void associer_equation(const Equation_base* eqn) const override;

  Interface_base& get_interface(int k, int l) const;
  Saturation_base& get_saturation(int k, int l) const;

  const Fluide_base& get_fluid(const int i) const;
  inline const Noms& noms_phases() const { return noms_phases_; }

protected :
  OWN_PTR(Champ_Don_base) rho_m_, h_m_;
  Noms noms_phases_;
  double t_init_ = -1.;
  bool has_saturation_ = false, has_interface_ = false;
  bool res_en_T_ = true; // par defaut resolution en T
  std::vector<std::vector<Interface_base *>> tab_interface_;
  std::vector<OWN_PTR(Fluide_base)> fluides_;
  OWN_PTR(Interface_base) sat_lu_, inter_lu_;

  std::pair<std::string, int> check_fluid_name(const Nom& name);
  void mettre_a_jour_tabs();
  static void calculer_masse_volumique(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  static void calculer_energie_interne(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  static void calculer_enthalpie(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  static void calculer_temperature_multiphase(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
};

#endif /* Milieu_composite_included */
