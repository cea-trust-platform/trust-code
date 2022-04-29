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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Milieu_composite.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Milieu_composite_included
#define Milieu_composite_included

#include <List_Fluide_base.h>
#include <Interface.h>
#include <Saturation_base.h>
#include <vector>
#include <set>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Milieu_composite
//    Cette classe represente un fluide reel ainsi que
//    ses proprietes:
//        - viscosite cinematique, (mu)
//        - viscosite dynamique,   (nu)
//        - masse volumique,       (rho)
//        - diffusivite,           (alpha)
//        - conductivite,          (lambda)
//        - capacite calorifique,  (Cp)
//        - dilatabilite thermique du constituant (beta_co)
// .SECTION voir aussi
//     Milieu_base
//////////////////////////////////////////////////////////////////////////////
class Milieu_composite: public Fluide_base
{
  Declare_instanciable(Milieu_composite);
public :
  void discretiser(const Probleme_base& pb, const  Discretisation_base& dis) override;
  void mettre_a_jour(double temps) override;
  int initialiser(const double temps) override;
  void preparer_calcul() override;
  void associer_equation(const Equation_base* eqn) const override;
  bool has_interface(int k, int l) const;
  Interface_base& get_interface(int k, int l) const;
  bool has_saturation(int k, int l) const;
  Saturation_base& get_saturation(int k, int l) const;
  void abortTimeStep() override;
  bool initTimeStep(double dt) override;

  int check_unknown_range() const override;

  LIST(Fluide_base) fluides;

protected :
  Champ_Fonc rho_m, h_m;
  void mettre_a_jour_tabs();
  static void calculer_masse_volumique(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  static void calculer_energie_interne(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  static void calculer_enthalpie(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  std::pair<std::string, int> check_fluid_name(const Nom& name);
  std::vector<std::vector<Interface_base *>> tab_interface;
  std::map<std::string, std::set<int>> phases_melange;
  double t_init_;
};

#endif
