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
// File:        Fluide_reel_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_reel_base.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <EChaine.h>
#include <Zone_VF.h>
#include <Pb_Multiphase.h>
#include <cfloat>
#include <cmath>

Implemente_base(Fluide_reel_base, "Fluide_reel_base", Fluide_base);
// XD fluide_reel_base fluide_base fluide_reel_base -1 Class for real fluids.

Entree& Fluide_reel_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  if ((P_ref_ >= 0 ) != (T_ref_ >= 0))
    Process::exit(que_suis_je() + ": T_ref and P_ref must both be specified!");
  return is;
}

void Fluide_reel_base::set_param(Param& param)
{
  param.ajouter("T_ref",&T_ref_);
  param.ajouter("P_ref",&P_ref_);
}

Sortie& Fluide_reel_base::printOn(Sortie& os) const
{
  return os;
}

void Fluide_reel_base::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Cerr << "Medium discretization." << finl;

  const Zone_dis_base& zone_dis = pb.equation(0).zone_dis();
  const double temps = pb.schema_temps().temps_courant();

  /* masse volumique, energie interne, enthalpie : champ_inc */
  Champ_Inc rho_inc, ei_inc, h_inc;
  int nc = pb.equation(0).inconnue()->nb_valeurs_temporelles();
  dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique",    "kg/m^3", 1, nc, temps, rho_inc);
  dis.discretiser_champ("champ_elem", zone_dis,       "energie_interne",     "J/kg", 1, nc, temps, ei_inc);
  dis.discretiser_champ("champ_elem", zone_dis,             "enthalpie",     "J/kg", 1, nc, temps, h_inc);
  rho = rho_inc, e_int = ei_inc, h = h_inc;

  dis.discretiser_champ("champ_elem", zone_dis,   "viscosite_dynamique",    "kg/m/s", 1, temps,       mu);
  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_cinematique",      "m2/s", 1, temps,       nu);
  dis.discretiser_champ("champ_elem", zone_dis,           "diffusivite",      "m2/s", 1, temps,    alpha);
  dis.discretiser_champ("champ_elem", zone_dis,          "conductivite",     "W/m/K", 1, temps,   lambda);
  dis.discretiser_champ("champ_elem", zone_dis,  "capacite_calorifique",    "J/kg/K", 1, temps,       Cp);
}

int Fluide_reel_base::initialiser(const double& temps)
{
  const Equation_base& eqn = equation("temperature");
  Champ_Inc_base& ch_rho = ref_cast(Champ_Inc_base, rho.valeur()),
                  &ch_e   = ref_cast(Champ_Inc_base, e_int.valeur()),
                   &ch_h   = ref_cast(Champ_Inc_base, h.valeur());
  ch_rho.associer_eqn(eqn), ch_rho.init_champ_calcule(*this, calculer_masse_volumique);
  ch_e.associer_eqn(eqn),   ch_e.init_champ_calcule(*this, calculer_energie_interne);
  ch_h.associer_eqn(eqn),   ch_h.init_champ_calcule(*this, calculer_enthalpie);

  Cp.initialiser(temps);
  mu.initialiser(temps);
  nu.initialiser(temps);
  alpha.initialiser(temps);
  lambda.initialiser(temps);
  mettre_a_jour(temps);
  return 1;
}

void Fluide_reel_base::mettre_a_jour(double t)
{
  double tp = ref_cast(Champ_Inc_base, rho.valeur()).temps(); //pour savoir si on va tourner la roue
  rho.mettre_a_jour(t);
  e_int.mettre_a_jour(t);
  h.mettre_a_jour(t);

  Cp.mettre_a_jour(t);
  mu.mettre_a_jour(t);
  lambda.mettre_a_jour(t);
  nu.mettre_a_jour(t);
  alpha.mettre_a_jour(t);

  const Champ_Inc_base& ch_T = equation("temperature").inconnue().valeur(), &ch_p = ref_cast(Navier_Stokes_std, equation("vitesse")).pression().valeur();
  const DoubleTab& temp = ch_T.valeurs(), &pres = ch_p.valeurs();

  int i, Ni = mu.valeurs().dimension_tot(0);
  DoubleTab& tab_Cp = Cp.valeurs(), &tab_mu = mu.valeurs(), &tab_lambda = lambda.valeurs(), &tab_nu = nu.valeurs(), &tab_alpha = alpha.valeurs();
  const DoubleTab& tab_rho = masse_volumique().valeurs();
  if (t > tp || first_maj_) for (i = 0; i < Ni; i++) /* maj uniquement en fin de pas de temps */
      {
        const double T = (T_ref_ > 0) ? T_ref_ : temp(i, id_composite), P = (P_ref_ > 0) ? P_ref_ : pres(i);
        tab_Cp(i) = cp_(T, P);
        tab_mu(i) = mu_(T, P);
        tab_lambda(i) = lambda_(T, P);
        tab_nu(i) = tab_mu(i) / tab_rho(i);
        tab_alpha(i) = tab_lambda(i) / tab_rho(i) / tab_Cp(i);
      }
  first_maj_ = 0;
}

int Fluide_reel_base::check_unknown_range() const
{
  int ok = 1, zero = 0;
  for (auto &&i_r : unknown_range())
    {
      const DoubleTab& vals = i_r.first == "pression" ? ref_cast(Navier_Stokes_std, equation("vitesse")).pression().valeurs() : equation(i_r.first).inconnue().valeurs();
      double vmin = DBL_MAX, vmax = -DBL_MAX;
      for (int i = 0, j = std::min(std::max(id_composite, zero), vals.dimension(1) - 1); i < vals.dimension(0); i++)
        vmin = std::min(vmin, vals(i, j)), vmax = std::max(vmax, vals(i, j));
      ok &= Process::mp_min(vmin) >= i_r.second[0] && Process::mp_max(vmax) <= i_r.second[1];
    }
  return ok;
}

void Fluide_reel_base::abortTimeStep()
{
  Fluide_base::abortTimeStep();
  rho->abortTimeStep();
  e_int->abortTimeStep();
  h->abortTimeStep();
}

bool Fluide_reel_base::initTimeStep(double dt)
{
  if (!equation_.size()) return true; //pas d'equation associee -> ???
  const Schema_Temps_base& sch = equation_.begin()->second->schema_temps(); //on recupere le schema en temps par la 1ere equation

  /* champs dont on doit creer des cases */
  std::vector<Champ_Inc_base *> vch = { &ref_cast(Champ_Inc_base, rho.valeur()), &ref_cast(Champ_Inc_base, e_int.valeur()), &ref_cast(Champ_Inc_base, h.valeur()) };
  for (auto &&pch : vch) for (int i = 1; i <= sch.nb_valeurs_futures(); i++)
      pch->changer_temps_futur(sch.temps_futur(i), i), pch->futur(i) = pch->valeurs();
  return true;
}

#define rhoc(T, p) (incomp ? rho0 : fl.rho_((T), (p)))

void Fluide_reel_base::calculer_masse_volumique(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_reel_base& fl = ref_cast(Fluide_reel_base, obj);
  const Champ_Inc_base& ch_T = fl.equation("temperature").inconnue().valeur(),
                        &ch_p = ref_cast(Navier_Stokes_std, fl.equation("vitesse")).pression().valeur();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs();
  int i, zero = 0, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n = std::max(fl.id_composite, zero), m = p.line_size() == T.line_size() ? n : 0,
         incomp = fl.T_ref_ >= 0 && fl.P_ref_ >= 0;
  double rho0 = incomp ? fl.rho_(fl.T_ref_, fl.P_ref_) : -1;
  for (i = 0; i < Ni; i++) val(i) = rhoc(T(i, n), p(i, m));

  const DoubleTab& bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();
  for (i = 0; i < Nb; i++) bval(i) = rhoc(bT(i, n), bp(i, m));

  if (incomp) return; //pas de derivees en incompressible
  DoubleTab& dp_rho = deriv["pression"], &dT_rho = deriv["temperature"];
  for (dp_rho.resize(Ni, 1), i = 0; i < Ni; i++) dp_rho(i) = fl.dP_rho_(T(i, n), p(i, m));
  for (dT_rho.resize(Ni, 1), i = 0; i < Ni; i++) dT_rho(i) = fl.dT_rho_(T(i, n), p(i, m));
}

#define hc(T, p) (incomp ? h0 + Cp0 * ((T) - fl.T_ref_) : fl.h_((T), (p)))

void Fluide_reel_base::calculer_enthalpie(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_reel_base& fl = ref_cast(Fluide_reel_base, obj);
  const Champ_Inc_base& ch_T = fl.equation("temperature").inconnue().valeur(),
                        &ch_p = ref_cast(Navier_Stokes_std, fl.equation("vitesse")).pression().valeur();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs();
  int i, zero = 0, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n = std::max(fl.id_composite, zero), m = p.line_size() == T.line_size() ? n : 0,
         incomp = fl.T_ref_ >= 0 && fl.P_ref_ >= 0;
  double h0 = incomp ? fl.h_(fl.T_ref_, fl.P_ref_) : -1, Cp0 = incomp ? fl.dT_h_(fl.T_ref_, fl.P_ref_) : -1;

  for (i = 0; i < Ni; i++) val(i) = hc(T(i, n), p(i, m));

  const DoubleTab& bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();
  for (i = 0; i < Nb; i++) bval(i) = hc(bT(i, n), bp(i, m));

  DoubleTab  *dp_h = incomp ? NULL : &deriv["pression"], &dT_h = deriv["temperature"];//une seule derivee en incompressible
  if (dp_h) for (dp_h->resize(Ni, 1), i = 0; i < Ni; i++) (*dp_h)(i) = fl.dP_h_(T(i, n), p(i, m));
  for (dT_h.resize(Ni, 1), i = 0; i < Ni; i++) dT_h(i) = incomp ? Cp0 : fl.dT_h_(T(i, n), p(i, m));
}

void Fluide_reel_base::calculer_energie_interne(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_reel_base& fl = ref_cast(Fluide_reel_base, obj);
  const Champ_Inc_base& ch_T = fl.equation("temperature").inconnue().valeur(),
                        &ch_p = ref_cast(Navier_Stokes_std, fl.equation("vitesse")).pression().valeur();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs();
  int i, zero = 0, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n = std::max(fl.id_composite, zero), m = p.line_size() == T.line_size() ? n : 0,
         incomp = fl.T_ref_ >= 0 && fl.P_ref_ >= 0;
  if (incomp) return calculer_enthalpie(obj, val, bval, deriv); //en incompressible, on prend e_int = h

  for (i = 0; i < Ni; i++) val(i) = fl.h_(T(i, n), p(i, m)) - p(i, m) / fl.rho_(T(i, n), p(i, m));

  const DoubleTab& bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();
  for (i = 0; i < Nb; i++) bval(i) = fl.h_(bT(i, n), bp(i, m)) - bp(i, m) / fl.rho_(bT(i, n), bp(i, m));

  DoubleTab& dp_e = deriv["pression"], &dT_e = deriv["temperature"];
  for (dp_e.resize(Ni, 1), i = 0; i < Ni; i++) dp_e(i) = fl.dP_h_(T(i, n), p(i, m)) - 1. / fl.rho_(T(i, n), p(i, m)) + p(i, m) * fl.dP_rho_(T(i, n), p(i, m)) / std::pow(fl.rho_(T(i, n), p(i, m)), 2);
  for (dT_e.resize(Ni, 1), i = 0; i < Ni; i++) dT_e(i) = fl.dT_h_(T(i, n), p(i, m)) + p(i, m) * fl.dT_rho_(T(i, n), p(i, m)) / std::pow(fl.rho_(T(i, n), p(i, m)), 2);
}
