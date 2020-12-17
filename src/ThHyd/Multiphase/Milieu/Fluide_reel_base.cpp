/****************************************************************************
* Copyright (c) 2021, CEA
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

Implemente_base(Fluide_reel_base, "Fluide_reel_base", Fluide_base);

Entree& Fluide_reel_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  isotherme_ = (T_ref_ > 0 && P_ref_ > 0);
  if (isotherme_)
    {
      // equivalent d'un Fluide incompressible
      // -> on fait des champs uniformes et on passe par
      // les methodes de Fluide_base
      Nom n_ch("champ_uniforme 1 "), n_rho(n_ch), n_cp(n_ch), n_beta(n_ch), n_lambda(n_ch), n_mu(n_ch);
      n_rho += Nom(rho_(T_ref_, P_ref_));
      n_cp += Nom(cp_(T_ref_, P_ref_));
      n_beta += Nom(beta_(T_ref_, P_ref_));
      n_lambda += Nom(lambda_(T_ref_));
      n_mu += Nom(mu_(T_ref_));
      Cerr << "rho = " << rho_(T_ref_, P_ref_) << " cp = " << cp_(T_ref_, P_ref_) << " lambda = " << lambda_(T_ref_) << " mu = " << mu_(T_ref_) << " beta = " << beta_(T_ref_, P_ref_) << finl;
      EChaine ch_rho(n_rho), ch_cp(n_cp), ch_beta(n_beta), ch_lambda(n_lambda), ch_mu(n_mu);
      ch_rho >> rho, ch_cp >> Cp, ch_beta >> beta_th, ch_lambda >> lambda, ch_mu >> mu;
      creer_champs_non_lus();
    }
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
  if (isotherme_) return Fluide_base::discretiser(pb, dis);

  Cerr << "Medium discretization." << finl;

  const Zone_dis_base& zone_dis = pb.equation(0).zone_dis();
  const double temps = pb.schema_temps().temps_courant();

  Champ_Fonc rho_fonc;
  dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique",   "kg/m^3", 1, temps, rho_fonc);
  rho = rho_fonc;
  dis.discretiser_champ("champ_elem", zone_dis,    "dT_masse_volumique",  "kg/m^3/K", 1, temps,   dT_rho);
  dis.discretiser_champ("champ_elem", zone_dis,    "dP_masse_volumique", "kg/m^3/Pa", 1, temps,   dP_rho);
  dis.discretiser_champ("champ_elem", zone_dis,   "viscosite_dynamique",    "kg/m/s", 1, temps,       mu);
  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_cinematique",      "m2/s", 1, temps,       nu);
  dis.discretiser_champ("champ_elem", zone_dis,           "diffusivite",      "m2/s", 1, temps,    alpha);
  dis.discretiser_champ("champ_elem", zone_dis,          "conductivite",     "W/m/K", 1, temps,   lambda);
  dis.discretiser_champ("champ_elem", zone_dis,  "capacite_calorifique",    "J/kg/K", 1, temps,       Cp);

  const DoubleTab& xv_bord = ref_cast(Zone_VF, rho->zone_dis_base()).xv_bord();
  rho_bord.resize(xv_bord.dimension_tot(0), 1);

}

int Fluide_reel_base::initialiser(const double& temps)
{
  if (isotherme_) return Fluide_base::initialiser(temps);

  if (sub_type(Champ_Don_base, rho.valeur())) ref_cast(Champ_Don_base, rho.valeur()).initialiser(temps);
  Cp.initialiser(temps);
  dT_rho.initialiser(temps);
  dP_rho.initialiser(temps);
  mu.initialiser(temps);
  nu.initialiser(temps);
  alpha.initialiser(temps);
  lambda.initialiser(temps);
  mettre_a_jour_tabs(temps);
  return 1;
}

void Fluide_reel_base::mettre_a_jour(double temps)
{
  if (isotherme_) return Fluide_base::mettre_a_jour(temps);
  Milieu_base::mettre_a_jour(temps);
  mettre_a_jour_tabs(temps);
}

void Fluide_reel_base::mettre_a_jour_tabs(const double t)
{
  if (isotherme_) Process::exit("should not be there!");

  const Equation_base& eqn = *equation.at("temperature");
  const Champ_base& ch_T = eqn.probleme().get_champ("Temperature");
  const Champ_base& ch_P = eqn.probleme().get_champ("Pression");
  const DoubleTab& temp = ch_T.valeurs(t), &temp_b = ch_T.valeur_aux_bords();
  const DoubleTab& pres = ch_P.valeurs(t), &pres_b = ch_P.valeur_aux_bords();

  const int Nl = mu.valeurs().dimension_tot(0);

  DoubleTab& tab_rho = rho.valeurs();
  DoubleTab& tab_dT_rho = dT_rho.valeurs();
  DoubleTab& tab_dP_rho = dP_rho.valeurs();
  DoubleTab& tab_Cp = Cp.valeurs();
  DoubleTab& tab_mu = mu.valeurs();
  DoubleTab& tab_nu = nu.valeurs();
  DoubleTab& tab_alpha = alpha.valeurs();
  DoubleTab& tab_lambda = lambda.valeurs();
  for (int i = 0; i < Nl; i++)
    {
      const double T = (T_ref_ > 0) ? T_ref_ : temp(i, id_composite), P = (P_ref_ > 0) ? P_ref_ : pres(i);
      tab_rho(i) = rho_(T, P);
      tab_dT_rho(i) = dT_rho_(T, P);
      tab_dP_rho(i) = dP_rho_(T, P);
      tab_Cp(i) = cp_(T, P);
      tab_mu(i) = mu_(T);
      tab_lambda(i) = lambda_(T);
      tab_nu(i) = tab_mu(i) / tab_rho(i);
      tab_alpha(i) = tab_lambda(i) / tab_rho(i) / tab_Cp(i);
    }
  for (int i = 0; i < rho_bord.dimension_tot(0); i++)
    {
      const double T = (T_ref_ > 0) ? T_ref_ : temp_b(i, id_composite), P = (P_ref_ > 0) ? P_ref_ : pres_b(i, 0);
      rho_bord(i, 0) = rho_(T, P);
    }
}
