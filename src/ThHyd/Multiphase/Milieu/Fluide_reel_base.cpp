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

#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Fluide_reel_base.h>
#include <Pb_Multiphase.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <EChaine.h>
#include <Zone_VF.h>
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
  if (is_incompressible()) /* cas incompressible  -> rho champ uniforme */
    {
      EChaine str(Nom("Champ_Uniforme 1 ") + Nom(_rho_(T_ref_, P_ref_) /* point-to-point */));
      str >> rho;
      dis.nommer_completer_champ_physique(zone_dis,"masse_volumique","kg/m^3",rho.valeur(),pb);
    }
  else
    dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique",    "kg/m^3", 1, nc, temps, rho_inc), rho = rho_inc;

  dis.discretiser_champ("champ_elem", zone_dis, "energie_interne", "J/kg", 1, nc, temps, ei_inc);
  dis.discretiser_champ("champ_elem", zone_dis, "enthalpie", "J/kg", 1, nc, temps, h_inc);
  e_int = ei_inc, h = h_inc;

  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_dynamique", "kg/m/s", 1, temps, mu);
  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_cinematique", "m2/s", 1, temps, nu);
  dis.discretiser_champ("champ_elem", zone_dis, "diffusivite", "m2/s", 1, temps, alpha);
  dis.discretiser_champ("champ_elem", zone_dis, "conductivite", "W/m/K", 1, temps, lambda);
  dis.discretiser_champ("champ_elem", zone_dis, "capacite_calorifique", "J/kg/K", 1, temps, Cp);
  dis.discretiser_champ("champ_elem", zone_dis, "dilatabilite", "K-1", 1, temps, beta_th);
  dis.discretiser_champ("temperature", zone_dis, "rho_cp_comme_T", "  J/m^3/K", 1, temps, rho_cp_comme_T_);
  for (auto &&pch : { &rho, &e_int, &h, (Champ*) &mu, (Champ*) &nu, (Champ*) &alpha, (Champ*) &lambda, (Champ*) &Cp, (Champ*) &beta_th, (Champ*) &rho_cp_comme_T_ })
    champs_compris_.ajoute_champ(pch->valeur());
}

int Fluide_reel_base::initialiser(const double temps)
{
  const Equation_base& eqn = equation("temperature");
  Champ_Inc_base *pch_rho = sub_type(Champ_Inc_base, rho.valeur()) ? &ref_cast(Champ_Inc_base, rho.valeur()) : NULL;
  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, e_int.valeur()), &ch_h = ref_cast(Champ_Inc_base, h.valeur());

  if (pch_rho) pch_rho->associer_eqn(eqn), pch_rho->init_champ_calcule(*this, calculer_masse_volumique);

  ch_e.associer_eqn(eqn), ch_e.init_champ_calcule(*this, calculer_energie_interne);
  ch_h.associer_eqn(eqn), ch_h.init_champ_calcule(*this, calculer_enthalpie);

  Cp.initialiser(temps);
  mu.initialiser(temps);
  nu.initialiser(temps);
  alpha.initialiser(temps);
  lambda.initialiser(temps);
  beta_th.initialiser(temps);
  rho_cp_comme_T_.initialiser(temps);
  t_init_ = temps;
  return 1;
}

void Fluide_reel_base::preparer_calcul()
{
  mettre_a_jour(t_init_);
}

void Fluide_reel_base::mettre_a_jour(double t)
{
  double tp = ref_cast(Champ_Inc_base, e_int.valeur()).temps(); //pour savoir si on va tourner la roue
  rho.mettre_a_jour(t);
  e_int.mettre_a_jour(t);
  h.mettre_a_jour(t);
  Cp.mettre_a_jour(t);
  mu.mettre_a_jour(t);
  lambda.mettre_a_jour(t);
  nu.mettre_a_jour(t);
  alpha.mettre_a_jour(t);
  beta_th.mettre_a_jour(t);
  rho_cp_comme_T_.mettre_a_jour(t);

  const Champ_Inc_base& ch_T = equation("temperature").inconnue().valeur(), &ch_p = ref_cast(Navier_Stokes_std, equation("vitesse")).pression().valeur();
  const DoubleTab& temp = ch_T.valeurs(), &pres = ch_p.valeurs();

  DoubleTab& tab_Cp = Cp.valeurs(), &tab_mu = mu.valeurs(), &tab_lambda = lambda.valeurs(), &tab_nu = nu.valeurs(), &tab_alpha = alpha.valeurs(),
             &tab_beta = beta_th.valeurs(), &tab_rCp = rho_cp_comme_T_.valeurs();

  const DoubleTab& tab_rho = masse_volumique().valeurs();

  int Ni = mu.valeurs().dimension_tot(0), cR = tab_rho.dimension_tot(0) == 1;

  if (t > tp || first_maj_)
    {
      if (is_incompressible())
        {
          _cp_(T_ref_, P_ref_, tab_Cp.get_span_tot() /* elem reel + virt */);
          _mu_(T_ref_, P_ref_, tab_mu.get_span_tot());
          _lambda_(T_ref_, P_ref_, tab_lambda.get_span_tot());
          _beta_(T_ref_, P_ref_, tab_beta.get_span_tot());
        }
      else
        {
          assert (pres.line_size() == 1 && tab_Cp.line_size() == 1 && tab_mu.line_size() == 1 && tab_lambda.line_size() == 1 && tab_beta.line_size() == 1);
          const int n_comp = temp.line_size(); /* on a temp(xx,id_composite) */
          cp_(temp.get_span_tot(), pres.get_span_tot(), tab_Cp.get_span_tot(), n_comp, id_composite);
          mu_(temp.get_span_tot(), pres.get_span_tot(), tab_mu.get_span_tot(), n_comp, id_composite);
          lambda_(temp.get_span_tot(), pres.get_span_tot(), tab_lambda.get_span_tot(), n_comp, id_composite);
          beta_(temp.get_span_tot(), pres.get_span_tot(), tab_beta.get_span_tot(), n_comp, id_composite);
        }

      for (int i = 0; i < Ni; i++) // fill values
        {
          tab_nu(i) = tab_mu(i) / tab_rho(!cR * i);
          tab_alpha(i) = tab_lambda(i) / tab_rho(!cR * i) / tab_Cp(i);
          tab_rCp(i) = tab_rho(!cR * i) * tab_Cp(i);
        }
    }
  first_maj_ = 0;
}

int Fluide_reel_base::check_unknown_range() const
{
  int ok = 1, zero = 0, nl = e_int.valeurs().dimension_tot(0); //on n'impose pas de contraintes aux lignes correspondant a des variables auxiliaires (eg pressions aux faces dans PolyMAC)
  for (auto &&i_r : unknown_range())
    {
      const DoubleTab& vals = i_r.first == "pression" ? ref_cast(Navier_Stokes_std, equation("vitesse")).pression().valeurs() : equation(i_r.first).inconnue().valeurs();
      double vmin = DBL_MAX, vmax = -DBL_MAX;
      for (int i = 0, j = std::min(std::max(id_composite, zero), vals.dimension(1) - 1); i < nl; i++)
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
  std::vector<Champ_Inc_base *> vch = { sub_type(Champ_Inc_base, rho.valeur()) ?& ref_cast(Champ_Inc_base, rho.valeur()) : NULL, &ref_cast(Champ_Inc_base, e_int.valeur()), &ref_cast(Champ_Inc_base, h.valeur()) };
  for (auto &&pch : vch)
    if (pch)
      for (int i = 1; i <= sch.nb_valeurs_futures(); i++)
        pch->changer_temps_futur(sch.temps_futur(i), i), pch->futur(i) = pch->valeurs();
  return true;
}

void Fluide_reel_base::calculer_masse_volumique(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_reel_base& fl = ref_cast(Fluide_reel_base, obj);
  const Champ_Inc_base& ch_T = fl.equation("temperature").inconnue().valeur(), &ch_p = ref_cast(Navier_Stokes_std, fl.equation("vitesse")).pression().valeur();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs(), &bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();
  int n_comp = T.line_size(), zero = 0, Ni = val.dimension_tot(0), n = std::max(fl.id_composite, zero), m = p.line_size() == T.line_size() ? n : 0;

  if (fl.is_incompressible())
    {
      fl._rho_(fl.T_ref_, fl.P_ref_, val.get_span_tot()); /* interne */
      fl._rho_(fl.T_ref_, fl.P_ref_, bval.get_span_tot()); /* bord */
    }
  else
    {
      if (m != 0) throw; // TODO : FIXME : a voir si besoin faut surcharger les methodes
      fl.rho_(T.get_span_tot(), p.get_span_tot(), val.get_span_tot(), n_comp, n); /* interne */
      fl.rho_(bT.get_span_tot(), bp.get_span_tot(), bval.get_span_tot(), n_comp, n); /* bord */
    }

  if (fl.is_incompressible()) return; //pas de derivees en incompressible

  assert (m == 0);
  DoubleTab& dp_rho = deriv["pression"], &dT_rho = deriv["temperature"];
  dp_rho.resize(Ni,1);
  dT_rho.resize(Ni,1);
  fl.dP_rho_(T.get_span_tot(), p.get_span_tot(), dp_rho.get_span_tot(), n_comp, n); /* interne */
  fl.dT_rho_(T.get_span_tot(), p.get_span_tot(), dT_rho.get_span_tot(), n_comp, n); /* interne */
}

void Fluide_reel_base::calculer_enthalpie(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_reel_base& fl = ref_cast(Fluide_reel_base, obj);
  const Champ_Inc_base& ch_T = fl.equation("temperature").inconnue().valeur(), &ch_p = ref_cast(Navier_Stokes_std, fl.equation("vitesse")).pression().valeur();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs(), &bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();

  const int n_comp = T.line_size(), zero = 0, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n = std::max(fl.id_composite, zero), m = p.line_size() == T.line_size() ? n : 0;
  DoubleTab& dT_h = deriv["temperature"];
  dT_h.resize(Ni, 1);

  if (fl.is_incompressible())
    {
      VectorD H_(Ni), dTH_(Ni), bH_(Nb), bdTH_(Nb); // Je suis desole ....
      fl._h_(fl.T_ref_, fl.P_ref_, SpanD(H_));
      fl._dT_h_(fl.T_ref_, fl.P_ref_, SpanD(dTH_));
      for (int i = 0; i < Ni; i++) val(i) = H_[i] + dTH_[i] * (T(i, n) - fl.T_ref_); /* interne */

      fl._h_(fl.T_ref_, fl.P_ref_, SpanD(bH_));
      fl._dT_h_(fl.T_ref_, fl.P_ref_, SpanD(bdTH_));
      for (int i = 0; i < Nb; i++) bval(i) = bH_[i] + bdTH_[i] * (bT(i, n) - fl.T_ref_); /* bord */

      for (int i = 0; i < Ni; i++) dT_h(i) = dTH_[i]; /* la seule derivee en incompressible */
    }
  else
    {
      if (m != 0) throw; // TODO : FIXME : a voir si besoin faut surcharger les methodes
      fl.h_(T.get_span_tot(), p.get_span_tot(), val.get_span_tot(), n_comp, n); /* interne */
      fl.h_(bT.get_span_tot(), bp.get_span_tot(),bval.get_span_tot(), n_comp, n); /* bord */

      DoubleTab& dp_h = deriv["pression"];
      dp_h.resize(Ni, 1);
      fl.dP_h_(T.get_span_tot(), p.get_span_tot(), dp_h.get_span_tot(), n_comp, n); /* interne */
      fl.dT_h_(T.get_span_tot(), p.get_span_tot(), dT_h.get_span_tot(), n_comp, n); /* interne */
    }
}

void Fluide_reel_base::calculer_energie_interne(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_reel_base& fl = ref_cast(Fluide_reel_base, obj);
  const Champ_Inc_base& ch_T = fl.equation("temperature").inconnue().valeur(), &ch_p = ref_cast(Navier_Stokes_std, fl.equation("vitesse")).pression().valeur();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs();
  const int n_comp = T.line_size(), zero = 0, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n = std::max(fl.id_composite, zero), m = p.line_size() == T.line_size() ? n : 0;

  if (fl.is_incompressible()) return calculer_enthalpie(obj, val, bval, deriv); // en incompressible, on prend e_int = h

  const DoubleTab& bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();

  VectorD H_(Ni), Rho_(Ni), bH_(Nb), bRho_(Nb);

  fl.h_(T.get_span_tot(), p.get_span_tot(), SpanD(H_), n_comp, n);
  fl.rho_(T.get_span_tot(), p.get_span_tot(), SpanD(Rho_), n_comp, n);
  for (int i = 0; i < Ni; i++) val(i) = H_[i] - p(i, m) / Rho_[i]; /* interne */

  fl.h_(bT.get_span_tot(), bp.get_span_tot(), SpanD(bH_), n_comp, n);
  fl.rho_(bT.get_span_tot(), bp.get_span_tot(), SpanD(bRho_), n_comp, n);
  for (int i = 0; i < Nb; i++) bval(i) = bH_[i] -  bp(i, m) / bRho_[i]; /* bord */

  DoubleTab& dp_e = deriv["pression"], &dT_e = deriv["temperature"];
  VectorD dPH_(Ni), dTH_(Ni), dPRho_(Ni), dTRho_(Ni); // idem ...

  fl.dP_h_(T.get_span_tot(), p.get_span_tot(), SpanD(dPH_), n_comp, n);
  fl.dT_h_(T.get_span_tot(), p.get_span_tot(), SpanD(dTH_), n_comp, n);
  fl.dP_rho_(T.get_span_tot(), p.get_span_tot(), SpanD(dPRho_), n_comp, n);
  fl.dT_rho_(T.get_span_tot(), p.get_span_tot(), SpanD(dTRho_), n_comp, n);
  dp_e.resize(Ni, 1);
  dT_e.resize(Ni, 1);

  for (int i = 0; i < Ni; i++) dp_e(i) = dPH_[i] - 1. / Rho_[i] + p(i, m) * dPRho_[i] / std::pow(Rho_[i] , 2);
  for (int i = 0; i < Ni; i++) dT_e(i) = dTH_[i] + p(i, m) * dTRho_[i] / std::pow(Rho_[i] , 2);
}
