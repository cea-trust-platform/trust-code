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
// File:        Milieu_composite.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Milieu_composite.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <EChaine.h>
#include <Zone_VF.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Interprete.h>

Implemente_instanciable(Milieu_composite, "Milieu_composite", Fluide_base);

Entree& Milieu_composite::readOn(Entree& is)
{
  int i = 0;
  std::vector<std::pair<std::string, int>> especes;
  Nom mot;
  is >> mot;
  if (mot != "{") Cerr << "Milieu_composite : { expected instead of " << mot << finl, Process::exit();
  for (is >> mot; mot != "}"; is >> mot)
    {
      Cerr << "Milieu_composite : ajout de " << mot << " ... ";
      if (Interprete::objet_existant(mot))
        {
          Fluide_base& fluide = ref_cast(Fluide_base, Interprete::objet(mot));
          fluide.set_id_composite(i++);
          fluides.add(fluide);
          especes.push_back(check_fluid_name(fluide.le_nom()));
          Cerr << "ok!" << finl;
        }
      else Process::exit("not found!");
    }

  // saturation
  int N = fluides.size();
  for (int n = 0; n < N; n++)
    {
      std::vector<Saturation_base *> satn;
      for (int m = 0; m < N; m++)
        {
          const std::string espn = especes[n].first, espm = especes[m].first;
          const int pn = especes[n].second, pm = especes[m].second;
          mot = Nom((std::string("saturation_") + especes[n].first).c_str());
          if (espn == espm && pn != pm && Interprete::objet_existant(mot))
            {
              Cerr << "Saturation between fluid " << n << " : " << fluides[n].le_nom() << " and " << m << " : " << fluides[m].le_nom() << finl;
              satn.push_back(&ref_cast(Saturation_base, Interprete::objet(mot)));
              if (satn.back()->get_Pref() > 0) // pour loi en e = e0 + cp * (T - T0)
                {
                  fluides[n].set_T0(satn.back()->Tsat(satn.back()->get_Pref()));
                  fluides[m].set_T0(satn.back()->Tsat(satn.back()->get_Pref()));
                }
            }
          else satn.push_back(NULL);
        }
      tab_saturation.push_back(satn);
    }
  return is;
}

std::pair<std::string, int> Milieu_composite::check_fluid_name(const Nom& name)
{
  int phase = name.finit_par("gaz");
  Nom espece = phase ? name.getPrefix("_gaz") : name.getPrefix("_liquide");

  return std::make_pair(espece.getString(), phase);
}

bool Milieu_composite::has_saturation(int k, int l) const
{
  if (tab_saturation[k][l]) return true;
  return false;
}

Saturation_base& Milieu_composite::get_saturation(int k, int l) const
{
  return *(tab_saturation[k][l]);
}

Sortie& Milieu_composite::printOn(Sortie& os) const
{
  return os;
}

int Milieu_composite::initialiser(const double& temps)
{
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].initialiser(temps);

  mettre_a_jour_tabs();

  const Equation_base& eqn = *equation.at("temperature");
  ref_cast(Champ_Inc_base, rho.valeur()).associer_eqn(eqn);
  ref_cast(Champ_Inc_base, rho.valeur()).init_champ_calcule(calculer_masse_volumique);
  ref_cast(Champ_Inc_base, e_int.valeur()).associer_eqn(eqn);
  ref_cast(Champ_Inc_base, e_int.valeur()).init_champ_calcule(calculer_energie_interne);
  ref_cast(Champ_Inc_base, h.valeur()).associer_eqn(eqn);
  ref_cast(Champ_Inc_base, h.valeur()).init_champ_calcule(calculer_enthalpie);
  return 1;
}


void Milieu_composite::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Cerr << "Composite medium discretization." << finl;

  const int N = fluides.size(), nc = pb.equation(0).inconnue()->nb_valeurs_temporelles();
  const Zone_dis_base& zone_dis = pb.equation(0).zone_dis();
  const double temps = pb.schema_temps().temps_courant();

  // Fluide_base::discretiser(pb, dis);
  for (int n = 0; n < N; n++)
    fluides[n].discretiser(pb, dis);

  /* masse volumique : champ_inc */
  Champ_Inc rho_inc;
  dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique",    "kg/m^3", N, nc, temps, rho_inc);
  dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique",    "kg/m^3", N, temps, rho_fonc);
  dis.discretiser_champ("champ_elem", zone_dis,    "dT_masse_volumique",  "kg/m^3/K", N, temps,   dT_rho);
  dis.discretiser_champ("champ_elem", zone_dis,    "dP_masse_volumique", "kg/m^3/Pa", N, temps,   dP_rho);
  rho = rho_inc;

  /* energie interne : champ_inc */
  Champ_Inc ei_inc;
  dis.discretiser_champ("champ_elem", zone_dis,       "energie_interne",     "J/m^3", N, nc, temps, ei_inc);
  dis.discretiser_champ("champ_elem", zone_dis,       "energie_interne",     "J/m^3", N, temps,  ei_fonc);
  dis.discretiser_champ("champ_elem", zone_dis,    "DT_energie_interne",   "J/m^3/K", N, temps, dT_e_int);
  dis.discretiser_champ("champ_elem", zone_dis,    "DP_energie_interne",  "J/m^3/Pa", N, temps, dP_e_int);
  e_int = ei_inc;

  /* enthalpie : champ_inc */
  Champ_Inc h_inc;
  dis.discretiser_champ("champ_elem", zone_dis,             "enthalpie",     "J/m^3", N, nc, temps, h_inc);
  dis.discretiser_champ("champ_elem", zone_dis,             "enthalpie",     "J/m^3", N, temps,   h_fonc);
  dis.discretiser_champ("champ_elem", zone_dis,          "DT_enthalpie",   "J/m^3/K", N, temps,     dT_h);
  dis.discretiser_champ("champ_elem", zone_dis,          "DP_enthalpie",  "J/m^3/Pa", N, temps,     dP_h);
  h = h_inc;

  /* autres champs : champ_fonc */
  dis.discretiser_champ("champ_elem", zone_dis,   "viscosite_dynamique",    "kg/m/s", N, temps,       mu);
  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_cinematique",      "m2/s", N, temps,       nu);
  dis.discretiser_champ("champ_elem", zone_dis,           "diffusivite",      "m2/s", N, temps,    alpha);
  dis.discretiser_champ("champ_elem", zone_dis,          "conductivite",     "W/m/K", N, temps,   lambda);
  dis.discretiser_champ("champ_elem", zone_dis,  "capacite_calorifique",    "J/kg/K", N, temps,       Cp);

  champs_compris_.ajoute_champ(rho);
  champs_compris_.ajoute_champ(mu.valeur());
  champs_compris_.ajoute_champ(nu.valeur());
  champs_compris_.ajoute_champ(alpha.valeur());
  champs_compris_.ajoute_champ(lambda.valeur());
  champs_compris_.ajoute_champ(Cp.valeur());
  champs_compris_.ajoute_champ(e_int);
  champs_compris_.ajoute_champ(h);

  const DoubleTab& xv_bord = ref_cast(Zone_VF, rho_inc->zone_dis_base()).xv_bord();
  rho_bord.resize(xv_bord.dimension_tot(0), rho_inc->valeurs().line_size());
  e_int_bord.resize(xv_bord.dimension_tot(0), ei_inc->valeurs().line_size());
  h_bord.resize(xv_bord.dimension_tot(0), h_inc->valeurs().line_size());

}

void Milieu_composite::mettre_a_jour(double temps)
{
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].mettre_a_jour(temps);
  rho.changer_temps(temps);
  rho.valeur().changer_temps(temps);
  e_int.changer_temps(temps);
  e_int.valeur().changer_temps(temps);
  h.changer_temps(temps);
  h.valeur().changer_temps(temps);
  mu.valeur().changer_temps(temps);
  nu.valeur().changer_temps(temps);
  lambda.valeur().changer_temps(temps);
  alpha.valeur().changer_temps(temps);
  Cp.valeur().changer_temps(temps);
  mettre_a_jour_tabs();
  Fluide_base::mettre_a_jour(temps);
}

void Milieu_composite::mettre_a_jour_tabs()
{
  const int N = fluides.size();

  /* masse volumique */
  {
    DoubleTab& tab = rho_fonc.valeurs();
    DoubleTab& dT_tab = dT_rho.valeurs();
    DoubleTab& dP_tab = dP_rho.valeurs();
    const int Nl = rho.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].masse_volumique();
        const DoubleTab& tab_bord = fluides[n].masse_volumique_bord();
        const Champ_base& ch_n_dT = fluides[n].dT_masse_volumique();
        const Champ_base& ch_n_dP = fluides[n].dP_masse_volumique();
        const int crho = sub_type(Champ_Uniforme, ch_n), crho_b = (tab_bord.dimension_tot(0) == 1),
                  crho_dT = sub_type(Champ_Uniforme, ch_n_dT), crho_dP = sub_type(Champ_Uniforme, ch_n_dP);
        for (int i = 0; i < Nl; i++)
          {
            tab(i, n) = ch_n.valeurs()(!crho * i, 0);
            dT_tab(i, n) = ch_n_dT.valeurs()(!crho_dT * i, 0);
            dP_tab(i, n) = ch_n_dP.valeurs()(!crho_dP * i, 0);
          }
        for (int i = 0; i < rho_bord.dimension_tot(0); i++)
          rho_bord(i, n) = tab_bord(!crho_b * i, 0);
      }
  }

  /* energie interne */
  {
    DoubleTab& tab = ei_fonc.valeurs();
    DoubleTab& dT_tab = dT_e_int.valeurs();
    DoubleTab& dP_tab = dP_e_int.valeurs();
    const int Nl = e_int.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].energie_interne();
        const DoubleTab& tab_bord = fluides[n].energie_interne_bord();
        const Champ_base& ch_n_dT = fluides[n].dT_energie_interne();
        const Champ_base& ch_n_dP = fluides[n].dP_energie_interne();
        const int cei = sub_type(Champ_Uniforme, ch_n), cei_b = (tab_bord.dimension_tot(0) == 1),
                  cei_dT = sub_type(Champ_Uniforme, ch_n_dT), cei_dP = sub_type(Champ_Uniforme, ch_n_dP);
        for (int i = 0; i < Nl; i++)
          {
            tab(i, n) = ch_n.valeurs()(!cei * i, 0);
            dT_tab(i, n) = ch_n_dT.valeurs()(!cei_dT * i, 0);
            dP_tab(i, n) = ch_n_dP.valeurs()(!cei_dP * i, 0);
          }
        for (int i = 0; i < e_int_bord.dimension_tot(0); i++)
          e_int_bord(i, n) = tab_bord(!cei_b * i, 0);
      }
  }

  /* enthalpie */
  {
    DoubleTab& tab = h_fonc.valeurs();
    DoubleTab& dT_tab = dT_h.valeurs();
    DoubleTab& dP_tab = dP_h.valeurs();
    const int Nl = h.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].enthalpie();
        const DoubleTab& tab_bord = fluides[n].enthalpie_bord();
        const Champ_base& ch_n_dT = fluides[n].dT_enthalpie();
        const Champ_base& ch_n_dP = fluides[n].dP_enthalpie();
        const int cei = sub_type(Champ_Uniforme, ch_n), cei_b = (tab_bord.dimension_tot(0) == 1),
                  cei_dT = sub_type(Champ_Uniforme, ch_n_dT), cei_dP = sub_type(Champ_Uniforme, ch_n_dP);
        for (int i = 0; i < Nl; i++)
          {
            tab(i, n) = ch_n.valeurs()(!cei * i, 0);
            dT_tab(i, n) = ch_n_dT.valeurs()(!cei_dT * i, 0);
            dP_tab(i, n) = ch_n_dP.valeurs()(!cei_dP * i, 0);
          }
        for (int i = 0; i < e_int_bord.dimension_tot(0); i++)
          h_bord(i, n) = tab_bord(!cei_b * i, 0);
      }
  }

  /* viscosite dynamique */
  {
    DoubleTab& tab = mu.valeurs();
    const int Nl = mu.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].viscosite_dynamique();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs()(!cch * i, 0);
      }
  }

  /* viscosite cinematique */
  {
    DoubleTab& tab = nu.valeurs();
    const int Nl = nu.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].viscosite_cinematique();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs()(!cch * i, 0);
      }
  }

  /* diffusivite */
  {
    DoubleTab& tab = alpha.valeurs();
    const int Nl = alpha.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].diffusivite();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs()(!cch * i, 0);
      }
  }

  /* conductivite */
  {
    DoubleTab& tab = lambda.valeurs();
    const int Nl = lambda.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].conductivite();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs()(!cch * i, 0);
      }
  }

  /* capacite calorifique */
  {
    DoubleTab& tab = Cp.valeurs();
    const int Nl = Cp.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].capacite_calorifique();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs()(!cch * i, 0);
      }
  }
}

void Milieu_composite::associer_equation(const Equation_base *eqn) const
{
  Fluide_base::associer_equation(eqn);
  // on fait suivre aux milieux sous-jacents (les fluide_reel en ont besoin!)
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].associer_equation(eqn);
}

const Champ_base& Milieu_composite::masse_volumique_fonc() const
{
  return rho_fonc;
}

const Champ_base& Milieu_composite::energie_interne_fonc() const
{
  return ei_fonc;
}

const Champ_base& Milieu_composite::enthalpie_fonc() const
{
  return h_fonc;
}

void Milieu_composite::calculer_masse_volumique(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only)
{
  const Milieu_composite& mil = ref_cast(Milieu_composite, ch.equation().milieu());
  val = mil.masse_volumique_fonc().valeurs();

  if (val_only) return;

  bval = mil.masse_volumique_bord();

  /* derivees */
  DoubleTab& derT = deriv["temperature"], &derP = deriv["pression"];
  derT = mil.dT_masse_volumique().valeurs(), derP = mil.dP_masse_volumique().valeurs();
}

void Milieu_composite::calculer_energie_interne(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only)
{
  const Milieu_composite& mil = ref_cast(Milieu_composite, ch.equation().milieu());
  val = mil.energie_interne_fonc().valeurs();

  if (val_only) return;

  bval = mil.energie_interne_bord();

  /* derivees */
  DoubleTab& derT = deriv["temperature"], &derP = deriv["pression"];
  derT = mil.dT_energie_interne().valeurs(), derP = mil.dP_energie_interne().valeurs();
}

void Milieu_composite::calculer_enthalpie(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only)
{
  const Milieu_composite& mil = ref_cast(Milieu_composite, ch.equation().milieu());
  val = mil.enthalpie_fonc().valeurs();

  if (val_only) return;

  bval = mil.enthalpie_bord();

  /* derivees */
  DoubleTab& derT = deriv["temperature"], &derP = deriv["pression"];
  derT = mil.dT_enthalpie().valeurs(), derP = mil.dP_enthalpie().valeurs();
}

void Milieu_composite::abortTimeStep()
{
  Fluide_base::abortTimeStep();
  if (e_int.non_nul()) e_int->abortTimeStep();
}

bool Milieu_composite::initTimeStep(double dt)
{
  if (!equation.size()) return true; //pas d'equation associee -> ???
  const Schema_Temps_base& sch = equation.begin()->second->schema_temps(); //on recupere le schema en temps par la 1ere equation

  /* champs dont on doit creer des cases */
  std::vector<Champ_Inc_base *> vch;
  if (rho.non_nul() && sub_type(Champ_Inc_base, rho.valeur())) vch.push_back(&ref_cast(Champ_Inc_base, rho.valeur()));
  if (e_int.non_nul() && sub_type(Champ_Inc_base, e_int.valeur())) vch.push_back(&ref_cast(Champ_Inc_base, e_int.valeur()));

  for (auto &pch : vch) for (int i = 1; i <= sch.nb_valeurs_futures(); i++)
      pch->changer_temps_futur(sch.temps_futur(i), i), pch->futur(i) = pch->valeurs();
  return true;
}
