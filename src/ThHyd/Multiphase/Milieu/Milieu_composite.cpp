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

  // interfaces
  int N = fluides.size();
  for (int n = 0; n < N; n++)
    {
      std::vector<Interface_base *> inter;
      for (int m = 0; m < N; m++)
        {
          const std::string espn = especes[n].first, espm = especes[m].first;
          const int pn = especes[n].second, pm = especes[m].second;
          Nom mot_inter = Nom((std::string("interface_") + especes[n].first + "_" + especes[m].first)), // "Interface_[...]_[...]"
              mot_sat   = Nom((std::string("saturation_") + especes[n].first)); //"Saturation_[...]"
          if (espn == espm && pn != pm && (Interprete::objet_existant(mot_inter) || Interprete::objet_existant(mot_sat)))
            {
              Cerr << "Interface between fluid " << n << " : " << fluides[n].le_nom() << " and " << m << " : " << fluides[m].le_nom() << finl;
              phases_melange[especes[n].first].insert(n), phases_melange[especes[n].first].insert(m);
              inter.push_back(&ref_cast(Interface_base, Interprete::objet(Interprete::objet_existant(mot_sat) ? mot_sat : mot_inter)));
              const Saturation_base *sat = sub_type(Saturation_base, *inter.back()) ? &ref_cast(Saturation_base, *inter.back()) : NULL;
              if (sat && sat->get_Pref() > 0) // pour loi en e = e0 + cp * (T - T0)
                {
                  const double hn = pn ? sat->Hvs(sat->get_Pref()) : sat->Hls(sat->get_Pref()),
                               hm = pm ? sat->Hvs(sat->get_Pref()) : sat->Hls(sat->get_Pref()),
                               T0 = sat->Tsat(sat->get_Pref());
                  fluides[n].set_h0_T0(hn, T0), fluides[m].set_h0_T0(hm, T0);
                }
            }
          else inter.push_back(NULL);
        }
      tab_interface.push_back(inter);
    }
  return is;
}

std::pair<std::string, int> Milieu_composite::check_fluid_name(const Nom& name)
{
  int phase = name.finit_par("gaz");
  Nom espece = phase ? name.getPrefix("_gaz") : name.getPrefix("_liquide");

  return std::make_pair(espece.getString(), phase);
}

bool Milieu_composite::has_interface(int k, int l) const
{
  if (tab_interface[k][l]) return true;
  return false;
}

Interface_base& Milieu_composite::get_interface(int k, int l) const
{
  return *(tab_interface[k][l]);
}

bool Milieu_composite::has_saturation(int k, int l) const
{
  if (tab_interface[k][l] && sub_type(Saturation_base, *tab_interface[k][l])) return true;
  return false;
}

Saturation_base& Milieu_composite::get_saturation(int k, int l) const
{
  return ref_cast(Saturation_base, *(tab_interface[k][l]));
}

Sortie& Milieu_composite::printOn(Sortie& os) const
{
  return os;
}

int Milieu_composite::initialiser(const double temps)
{
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].initialiser(temps);

  const Equation_base& eqn = equation("temperature");
  Champ_Inc_base& ch_rho = ref_cast(Champ_Inc_base, rho.valeur()),
                  &ch_e   = ref_cast(Champ_Inc_base, e_int.valeur()),
                   &ch_h   = ref_cast(Champ_Inc_base, h.valeur());
  ch_rho.associer_eqn(eqn), ch_rho.init_champ_calcule(*this, calculer_masse_volumique);
  ch_e.associer_eqn(eqn),   ch_e.init_champ_calcule(*this, calculer_energie_interne);
  ch_h.associer_eqn(eqn),   ch_h.init_champ_calcule(*this, calculer_enthalpie);
  mettre_a_jour(temps);
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

  /* masse volumique, energie interne, enthalpie : champ_inc */
  Champ_Inc rho_inc, ei_inc, h_inc;
  dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique",    "kg/m^3", N, nc, temps, rho_inc);
  dis.discretiser_champ("champ_elem", zone_dis,       "energie_interne",     "J/m^3", N, nc, temps, ei_inc);
  dis.discretiser_champ("champ_elem", zone_dis,             "enthalpie",     "J/m^3", N, nc, temps, h_inc);
  rho = rho_inc, e_int = ei_inc, h = h_inc;

  /* autres champs : champ_fonc */
  dis.discretiser_champ("champ_elem", zone_dis,   "viscosite_dynamique",    "kg/m/s", N, temps,       mu);
  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_cinematique",      "m2/s", N, temps,       nu);
  dis.discretiser_champ("champ_elem", zone_dis,           "diffusivite",      "m2/s", N, temps,    alpha);
  dis.discretiser_champ("champ_elem", zone_dis,          "conductivite",     "W/m/K", N, temps,   lambda);
  dis.discretiser_champ("champ_elem", zone_dis,  "capacite_calorifique",    "J/kg/K", N, temps,       Cp);

  /* champs de quantites melange */
  for (auto &&kv : phases_melange)
    {
      dis.discretiser_champ("champ_elem", zone_dis,   Nom("masse_volumique_") + kv.first + "_melange", "kg/m^3", 1, temps, rho_m);
      dis.discretiser_champ("champ_elem", zone_dis,         Nom("enthalpie_") + kv.first + "_melange",  "J/m^3", 1, temps,   h_m);
      champs_compris_.ajoute_champ(rho_m);
      champs_compris_.ajoute_champ(h_m);
    }

  champs_compris_.ajoute_champ(rho);
  champs_compris_.ajoute_champ(mu.valeur());
  champs_compris_.ajoute_champ(nu.valeur());
  champs_compris_.ajoute_champ(alpha.valeur());
  champs_compris_.ajoute_champ(lambda.valeur());
  champs_compris_.ajoute_champ(Cp.valeur());
  champs_compris_.ajoute_champ(e_int);
  champs_compris_.ajoute_champ(h);
}

void Milieu_composite::mettre_a_jour(double temps)
{
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].mettre_a_jour(temps);
  rho.mettre_a_jour(temps);
  e_int.mettre_a_jour(temps);
  h.mettre_a_jour(temps);

  mu.mettre_a_jour(temps);
  nu.mettre_a_jour(temps);
  lambda.mettre_a_jour(temps);
  alpha.mettre_a_jour(temps);
  Cp.mettre_a_jour(temps);
  if (rho_m.non_nul()) rho_m.mettre_a_jour(temps);
  if (h_m.non_nul()) h_m.mettre_a_jour(temps);
  mettre_a_jour_tabs();
  // Fluide_base::mettre_a_jour(temps);
}

void Milieu_composite::mettre_a_jour_tabs()
{
  const int N = fluides.size();

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

  if (rho_m.non_nul() && h_m.non_nul())
    {
      DoubleTab& trm = rho_m.valeurs(), &thm = h_m.valeurs();
      trm = 0, thm = 0;

      const Equation_base& eqn = equation("alpha");
      const DoubleTab& a = eqn.inconnue().valeurs(),
                       &r = rho.valeurs(), &ent = h.valeurs();
      const Nom ph = (rho_m.le_nom().getSuffix("masse_volumique_")).getPrefix("_melange");
      const int Nl = rho_m.valeurs().dimension_tot(0);
      // masse volumique melange
      for (auto n : phases_melange[ph.getString()])
        for (int i = 0; i < Nl; i++) trm(i) += a(i, n) * r(i, n);
      // enthalpie melange
      for (auto n : phases_melange[ph.getString()])
        for (int i = 0; i < Nl; i++) thm(i) += a(i, n) * r(i, n) * ent(i, n);
      for (int i = 0; i < Nl; i++) thm(i) /= trm(i);
    }
}

void Milieu_composite::associer_equation(const Equation_base *eqn) const
{
  Fluide_base::associer_equation(eqn);
  // on fait suivre aux milieux sous-jacents (les fluide_reel en ont besoin!)
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].associer_equation(eqn);
}

int Milieu_composite::check_unknown_range() const
{
  int ok = 1;
  for (int i = 0; ok && i < fluides.size(); i++) ok &= fluides[i].check_unknown_range(); //chaque fluide controle
  return ok;
}

void Milieu_composite::calculer_masse_volumique(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Milieu_composite& mil = ref_cast(Milieu_composite, obj);
  const Zone_VF& zvf = ref_cast(Zone_VF, mil.equation_.begin()->second->zone_dis().valeur());
  int i, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n, N = mil.fluides.size();
  std::vector<const DoubleTab *> split(N);
  for (n = 0; n < N; n++) split[n] = &mil.fluides[n].masse_volumique().valeurs();
  for (i = 0; i < Ni; i++) for (n = 0; n < N; n++) val(i, n) = (*split[n])(i * (split[n]->dimension(0) > 1), 0);

  std::vector<DoubleTab> bsplit(N);
  for (n = 0; n < N; n++) if (mil.fluides[n].masse_volumique()->a_une_zone_dis_base())
      bsplit[n] = mil.fluides[n].masse_volumique()->valeur_aux_bords();
    else bsplit[n].resize(bval.dimension_tot(0), 1), mil.fluides[n].masse_volumique()->valeur_aux(zvf.xv_bord(), bsplit[n]);
  for (i = 0; i < Nb; i++) for (n = 0; n < N; n++) bval(i, n) = bsplit[n](i * (split[n]->dimension(0) > 1), 0);

  /* derivees */
  std::vector<const tabs_t *> split_der(N);
  for (n = 0; n < N; n++) split_der[n] = sub_type(Champ_Inc_base, mil.fluides[n].masse_volumique().valeur()) ? &ref_cast(Champ_Inc_base, mil.fluides[n].masse_volumique().valeur()).derivees() : NULL;
  std::set<std::string> noms_der;
  for (n = 0; n < N; n++) if (split_der[n]) for (auto &&n_d : *split_der[n]) noms_der.insert(n_d.first);
  for (auto &&nom : noms_der)
    {
      for (n = 0; n < N; n++) split[n] = split_der[n] && split_der[n]->count(nom) ? &split_der[n]->at(nom) : NULL;
      DoubleTab& der = deriv[nom];
      for (der.resize(Ni, N), i = 0; i < Ni; i++) for (n = 0; n < N; n++) der(i, n) = split[n] ? (*split[n])(i * (split[n]->dimension(0) > 1)) : 0;
    }
}

void Milieu_composite::calculer_energie_interne(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Milieu_composite& mil = ref_cast(Milieu_composite, obj);
  int i, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n, N = mil.fluides.size();
  std::vector<const DoubleTab *> split(N);
  for (n = 0; n < N; n++) split[n] = &mil.fluides[n].energie_interne().valeurs();
  for (i = 0; i < Ni; i++) for (n = 0; n < N; n++) val(i, n) = (*split[n])(i * (split[n]->dimension(0) > 1), 0);

  std::vector<DoubleTab> bsplit(N);
  for (n = 0; n < N; n++) bsplit[n] = mil.fluides[n].energie_interne().valeur_aux_bords();
  for (i = 0; i < Nb; i++) for (n = 0; n < N; n++) bval(i, n) = bsplit[n](i * (split[n]->dimension(0) > 1), 0);

  /* derivees */
  std::vector<const tabs_t *> split_der(N);
  for (n = 0; n < N; n++) split_der[n] = sub_type(Champ_Inc_base, mil.fluides[n].energie_interne()) ? &ref_cast(Champ_Inc_base, mil.fluides[n].energie_interne()).derivees() : NULL;
  std::set<std::string> noms_der;
  for (n = 0; n < N; n++) if (split_der[n]) for (auto &&n_d : *split_der[n]) noms_der.insert(n_d.first);
  for (auto &&nom : noms_der)
    {
      for (n = 0; n < N; n++) split[n] = split_der[n] && split_der[n]->count(nom) ? &split_der[n]->at(nom) : NULL;
      DoubleTab& der = deriv[nom];
      for (der.resize(Ni, N), i = 0; i < Ni; i++) for (n = 0; n < N; n++) der(i, n) = split[n] ? (*split[n])(i * (split[n]->dimension(0) > 1)) : 0;
    }
}

void Milieu_composite::calculer_enthalpie(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Milieu_composite& mil = ref_cast(Milieu_composite, obj);
  int i, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n, N = mil.fluides.size();
  std::vector<const DoubleTab *> split(N);
  for (n = 0; n < N; n++) split[n] = &mil.fluides[n].enthalpie().valeurs();
  for (i = 0; i < Ni; i++) for (n = 0; n < N; n++) val(i, n) = (*split[n])(i * (split[n]->dimension(0) > 1), 0);

  std::vector<DoubleTab> bsplit(N);
  for (n = 0; n < N; n++) bsplit[n] = mil.fluides[n].enthalpie().valeur_aux_bords();
  for (i = 0; i < Nb; i++) for (n = 0; n < N; n++) bval(i, n) = bsplit[n](i * (split[n]->dimension(0) > 1), 0);

  /* derivees */
  std::vector<const tabs_t *> split_der(N);
  for (n = 0; n < N; n++) split_der[n] = sub_type(Champ_Inc_base, mil.fluides[n].enthalpie()) ? &ref_cast(Champ_Inc_base, mil.fluides[n].enthalpie()).derivees() : NULL;
  std::set<std::string> noms_der;
  for (n = 0; n < N; n++) if (split_der[n]) for (auto &&n_d : *split_der[n]) noms_der.insert(n_d.first);
  for (auto &&nom : noms_der)
    {
      for (n = 0; n < N; n++) split[n] = split_der[n] && split_der[n]->count(nom) ? &split_der[n]->at(nom) : NULL;
      DoubleTab& der = deriv[nom];
      for (der.resize(Ni, N), i = 0; i < Ni; i++) for (n = 0; n < N; n++) der(i, n) = split[n] ? (*split[n])(i * (split[n]->dimension(0) > 1)) : 0;
    }
}

void Milieu_composite::abortTimeStep()
{
  Fluide_base::abortTimeStep();
  if (e_int.non_nul()) e_int->abortTimeStep();
  if (h.non_nul()) h->abortTimeStep();
}

bool Milieu_composite::initTimeStep(double dt)
{
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].initTimeStep(dt);
  if (!equation_.size()) return true; //pas d'equation associee -> ???
  const Schema_Temps_base& sch = equation_.begin()->second->schema_temps(); //on recupere le schema en temps par la 1ere equation

  /* champs dont on doit creer des cases */
  std::vector<Champ_Inc_base *> vch;
  if (rho.non_nul() && sub_type(Champ_Inc_base, rho.valeur())) vch.push_back(&ref_cast(Champ_Inc_base, rho.valeur()));
  if (e_int.non_nul() && sub_type(Champ_Inc_base, e_int.valeur())) vch.push_back(&ref_cast(Champ_Inc_base, e_int.valeur()));
  if (h.non_nul() && sub_type(Champ_Inc_base, h.valeur())) vch.push_back(&ref_cast(Champ_Inc_base, h.valeur()));

  for (auto &pch : vch) for (int i = 1; i <= sch.nb_valeurs_futures(); i++)
      pch->changer_temps_futur(sch.temps_futur(i), i), pch->futur(i) = pch->valeurs();
  return true;
}
