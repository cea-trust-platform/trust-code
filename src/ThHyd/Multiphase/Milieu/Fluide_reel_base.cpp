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

#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Fluide_reel_base.h>
#include <Pb_Multiphase.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <TPPI_tools.h>
#include <EChaine.h>
#include <cfloat>
#include <cmath>

Implemente_base_sans_constructeur(Fluide_reel_base, "Fluide_reel_base", Fluide_base);
// XD fluide_reel_base fluide_base fluide_reel_base -1 Class for real fluids.

Sortie& Fluide_reel_base::printOn(Sortie& os) const { return os; }
Entree& Fluide_reel_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  if ((P_ref_ >= 0) != (T_ref_ >= 0))
    Process::exit(que_suis_je() + ": T_ref and P_ref must both be specified!");
  return is;
}

void Fluide_reel_base::set_param(Param& param)
{
  param.ajouter("T_ref", &T_ref_);
  param.ajouter("P_ref", &P_ref_);
  set_additional_params(param);
}

void Fluide_reel_base::discretiser(const Probleme_base& pb, const Discretisation_base& dis)
{
  Cerr << "Medium discretization." << finl;

  const Domaine_dis_base& domaine_dis = pb.equation(0).domaine_dis();
  const double temps = pb.schema_temps().temps_courant();

  /* masse volumique, energie interne, enthalpie : champ_inc */
  Champ_Inc rho_inc, ei_inc, h_inc;
  int nc = pb.equation(0).inconnue()->nb_valeurs_temporelles();
  if (is_incompressible()) /* cas incompressible  -> rho champ uniforme */
    {
      EChaine str(Nom("Champ_Uniforme 1 ") + Nom(_rho_(T_ref_, P_ref_) /* point-to-point */));
      str >> rho;
      dis.nommer_completer_champ_physique(domaine_dis, "masse_volumique", "kg/m^3", rho.valeur(), pb);
    }
  else
    dis.discretiser_champ("champ_elem", domaine_dis, "masse_volumique", "kg/m^3", 1, nc, temps, rho_inc), rho = rho_inc;

  dis.discretiser_champ("champ_elem", domaine_dis, "energie_interne", "J/kg", 1, nc, temps, ei_inc);
  dis.discretiser_champ("champ_elem", domaine_dis, "enthalpie", "J/kg", 1, nc, temps, h_inc);
  e_int = ei_inc, h = h_inc;

  dis.discretiser_champ("champ_elem", domaine_dis, "viscosite_dynamique", "kg/m/s", 1, temps, mu);
  dis.discretiser_champ("champ_elem", domaine_dis, "viscosite_cinematique", "m2/s", 1, temps, nu);
  dis.discretiser_champ("champ_elem", domaine_dis, "diffusivite", "m2/s", 1, temps, alpha);
  dis.discretiser_champ("champ_elem", domaine_dis, "conductivite", "W/m/K", 1, temps, lambda);
  dis.discretiser_champ("champ_elem", domaine_dis, "capacite_calorifique", "J/kg/K", 1, temps, Cp);
  dis.discretiser_champ("champ_elem", domaine_dis, "dilatabilite", "K-1", 1, temps, beta_th);
  dis.discretiser_champ("temperature", domaine_dis, "rho_cp_comme_T", "  J/m^3/K", 1, temps, rho_cp_comme_T_);
  for (auto &&pch : { &rho, &e_int, &h, (Champ*) &mu, (Champ*) &nu, (Champ*) &alpha, (Champ*) &lambda, (Champ*) &Cp, (Champ*) &beta_th, (Champ*) &rho_cp_comme_T_ })
    champs_compris_.ajoute_champ(pch->valeur());

  if (id_composite == -1)
    {
      Milieu_base::discretiser_porosite(pb, dis);
      Milieu_base::discretiser_diametre_hydro(pb, dis);
    }
}

int Fluide_reel_base::initialiser(const double temps)
{
  const Equation_base& eqn = equation("temperature");
  Champ_Inc_base *pch_rho = sub_type(Champ_Inc_base, rho.valeur()) ? &ref_cast(Champ_Inc_base, rho.valeur()) : NULL;
  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, e_int.valeur()), &ch_h = ref_cast(Champ_Inc_base, h.valeur());

  if (pch_rho) pch_rho->associer_eqn(eqn), pch_rho->resize_val_bord(), pch_rho->set_val_bord_fluide_multiphase(true);
  ch_h.associer_eqn(eqn), ch_h.resize_val_bord(), ch_h.set_val_bord_fluide_multiphase(true);
  ch_e.associer_eqn(eqn), ch_e.resize_val_bord(), ch_e.set_val_bord_fluide_multiphase(true);

  Cp.initialiser(temps);
  mu.initialiser(temps);
  nu.initialiser(temps);
  alpha.initialiser(temps);
  lambda.initialiser(temps);
  beta_th.initialiser(temps);
  rho_cp_comme_T_.initialiser(temps);
  t_init_ = temps;
  if (is_incompressible()) mettre_a_jour(temps); // ne depend pas de p et T : on peut terminer l'initialisation
  if (id_composite == -1) Milieu_base::initialiser_porosite(temps);
  if (id_composite < 0 && g.non_nul()) g->initialiser(temps);
  return 1;
}

void Fluide_reel_base::preparer_calcul() { mettre_a_jour(t_init_); }

void Fluide_reel_base::mettre_a_jour(double t)
{
  double tp = ref_cast(Champ_Inc_base, e_int.valeur()).temps(); //pour savoir si on va tourner la roue
  rho.mettre_a_jour(t);
  e_int.mettre_a_jour(t);
  h.mettre_a_jour(t);

  // on calcule les props (EOS)
  is_incompressible() ? calculate_fluid_properties_incompressible() : calculate_fluid_properties();

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
      MLoiSpanD spans_interne = { { Loi_en_T::CP, tab_Cp.get_span_tot() }, { Loi_en_T::MU, tab_mu.get_span_tot() },
        { Loi_en_T::LAMBDA, tab_lambda.get_span_tot() }, { Loi_en_T::BETA, tab_beta.get_span_tot() }
      };

      if (is_incompressible()) _compute_CPMLB_pb_multiphase_(spans_interne);
      else
        {
          assert(pres.line_size() == 1 && tab_Cp.line_size() == 1 && tab_mu.line_size() == 1 && tab_lambda.line_size() == 1 && tab_beta.line_size() == 1);
          const int n_comp = temp.line_size(); /* on a temp(xx,id_composite) */
          MSpanD spans_input = { { "temperature", temp.get_span_tot() }, { "pressure", pres.get_span_tot() } };
          compute_CPMLB_pb_multiphase_(spans_input, spans_interne, n_comp, id_composite);
        }

      for (int i = 0; i < Ni; i++) // fill values
        {
          tab_nu(i) = tab_mu(i) / tab_rho(!cR * i);
          tab_alpha(i) = tab_lambda(i) / tab_rho(!cR * i) / tab_Cp(i);
          tab_rCp(i) = tab_rho(!cR * i) * tab_Cp(i);
        }
    }
  first_maj_ = 0;
  if (id_composite == -1) Milieu_base::mettre_a_jour_porosite(t);
}

int Fluide_reel_base::check_unknown_range() const
{
  if (is_incompressible()) return 1;
  int ok = 1, zero = 0, nl = e_int.valeurs().dimension_tot(0); //on n'impose pas de contraintes aux lignes correspondant a des variables auxiliaires (eg pressions aux faces dans PolyMAC_P0P1NC)
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

void Fluide_reel_base::calculate_fluid_properties_incompressible()
{
  assert (is_incompressible());
  const Champ_Inc_base& ch_T = equation("temperature").inconnue().valeur();
  const DoubleTab& T = ch_T.valeurs(), &bT = ch_T.valeur_aux_bords();

  Champ_Inc_base& ch_h = ref_cast_non_const(Champ_Inc_base, h.valeur());
  DoubleTab& val_h = ch_h.valeurs(), &bval_h = ch_h.val_bord();

  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, e_int.valeur());
  DoubleTab& val_e = ch_e.valeurs(), &bval_e = ch_e.val_bord();

  const int zero = 0, Ni = val_h.dimension_tot(0), Nb = bval_h.dimension_tot(0), n = std::max(id_composite, zero);

  DoubleTab& dT_h = ch_h.derivees()["temperature"], &dT_e = ch_e.derivees()["temperature"];
  dT_h.resize(Ni, 1);
  dT_e.resize(Ni, 1);

  VectorD H_(Ni), dTH_(Ni), bH_(Nb), bdTH_(Nb); // Je suis desole ...
  MLoiSpanD spans_interne = { { Loi_en_T::H, SpanD(H_) }, { Loi_en_T::H_DT, SpanD(dTH_) } };
  MLoiSpanD spans_bord = { { Loi_en_T::H, SpanD(bH_) }, { Loi_en_T::H_DT, SpanD(bdTH_) } };
  _compute_all_pb_multiphase_(spans_interne, spans_bord);

  for (int i = 0; i < Ni; i++) /* interne */
    {
      val_h(i) = H_[i] + dTH_[i] * (T(i, n) - T_ref_);
      val_e(i) = val_h(i);
      dT_h(i) = dTH_[i]; /* la seule derivee en incompressible */
      dT_e(i) = dT_h(i);
    }

  for (int i = 0; i < Nb; i++) /* bord */
    {
      bval_h(i) = bH_[i] + bdTH_[i] * (bT(i, n) - T_ref_);
      bval_e(i) = bval_h(i);
    }
}

void Fluide_reel_base::calculate_fluid_properties()
{
  const Champ_Inc_base& ch_T = equation("temperature").inconnue().valeur(), &ch_p = ref_cast(Navier_Stokes_std, equation("vitesse")).pression().valeur();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs(), &bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();

  Champ_Inc_base& ch_rho = ref_cast_non_const(Champ_Inc_base, rho.valeur());
  DoubleTab& val_rho = ch_rho.valeurs(), &bval_rho = ch_rho.val_bord();

  Champ_Inc_base& ch_h = ref_cast_non_const(Champ_Inc_base, h.valeur());
  DoubleTab& val_h = ch_h.valeurs(), &bval_h = ch_h.val_bord();

  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, e_int.valeur());
  DoubleTab& val_e = ch_e.valeurs(), &bval_e = ch_e.val_bord();

  const int n_comp = T.line_size(), zero = 0, Ni = val_h.dimension_tot(0), Nb = bval_h.dimension_tot(0), n = std::max(id_composite, zero), m = p.line_size() == T.line_size() ? n : 0;

  if (m != 0) Process::exit("Fluide_reel_base currently supports single-component pressure field ! Call the 911 !");

  DoubleTab& dT_rho = ch_rho.derivees()["temperature"], &dp_rho = ch_rho.derivees()["pression"];
  DoubleTab& dT_h = ch_h.derivees()["temperature"], &dp_h = ch_h.derivees()["pression"];
  DoubleTab& dT_e = ch_e.derivees()["temperature"], &dp_e = ch_e.derivees()["pression"];

  dT_rho.resize(Ni,1);
  dp_rho.resize(Ni,1);
  dT_h.resize(Ni, 1);
  dp_h.resize(Ni, 1);
  dT_e.resize(Ni, 1);
  dp_e.resize(Ni, 1);

  MSpanD spans_input = { { "temperature", T.get_span_tot() }, { "pressure", p.get_span_tot() }, { "bord_temperature", bT.get_span_tot() }, { "bord_pressure", bp.get_span_tot() } };
  MLoiSpanD spans_interne = { }, spans_bord = { };

  // pour rho
  spans_interne.insert( { Loi_en_T::RHO, val_rho.get_span_tot() });
  spans_interne.insert( { Loi_en_T::RHO_DP, dp_rho.get_span_tot() });
  spans_interne.insert( { Loi_en_T::RHO_DT, dT_rho.get_span_tot() });
  spans_bord.insert( { Loi_en_T::RHO, bval_rho.get_span_tot() });

  // pour h
  spans_interne.insert( { Loi_en_T::H, val_h.get_span_tot() });
  spans_interne.insert( { Loi_en_T::H_DP, dp_h.get_span_tot() });
  spans_interne.insert( { Loi_en_T::H_DT, dT_h.get_span_tot() });
  spans_bord.insert( { Loi_en_T::H, bval_h.get_span_tot() });

  compute_all_pb_multiphase_(spans_input, spans_interne, spans_bord, n_comp, n);

  // energie_interne
  for (int i = 0; i < Ni; i++) /* interne */
    {
      val_e(i) = val_h(i) - p(i, m) / val_rho(i);
      dp_e(i) = dp_h(i) - 1. / val_rho(i) + p(i, m) * dp_rho(i) / std::pow(val_rho(i) , 2);
      dT_e(i) = dT_h(i) + p(i, m) * dT_rho(i) / std::pow(val_rho(i) , 2);
    }

  for (int i = 0; i < Nb; i++) bval_e(i) = bval_h(i) -  bp(i, m) / bval_rho(i); /* bord */
}

void Fluide_reel_base::_compute_CPMLB_pb_multiphase_(MLoiSpanD prop) const
{
  assert((int )prop.size() == 4);
  SpanD CP = prop.at(Loi_en_T::CP), M = prop.at(Loi_en_T::MU), L = prop.at(Loi_en_T::LAMBDA), B = prop.at(Loi_en_T::BETA);
  _cp_(T_ref_, P_ref_, CP);
  _mu_(T_ref_, P_ref_, M);
  _lambda_(T_ref_, P_ref_, L);
  _beta_(T_ref_, P_ref_, B);
}

void Fluide_reel_base::_compute_all_pb_multiphase_(MLoiSpanD inter, MLoiSpanD bord) const
{
  assert((int )inter.size() == 2 && (int )bord.size() == 2);
  SpanD H = inter.at(Loi_en_T::H), dTH = inter.at(Loi_en_T::H_DT), bH = bord.at(Loi_en_T::H), bdTH = bord.at(Loi_en_T::H_DT);
  _h_(T_ref_, P_ref_, H);
  _h_(T_ref_, P_ref_, bH);
  _dT_h_(T_ref_, P_ref_, dTH);
  _dT_h_(T_ref_, P_ref_, bdTH);
}

void Fluide_reel_base::compute_CPMLB_pb_multiphase_(const MSpanD input, MLoiSpanD prop, int ncomp, int id) const
{
  assert((int )prop.size() == 4);
  const SpanD T = input.at("temperature"), P = input.at("pressure");

  // BEEM
  SpanD CP = prop.at(Loi_en_T::CP), M = prop.at(Loi_en_T::MU), L = prop.at(Loi_en_T::LAMBDA), B = prop.at(Loi_en_T::BETA);
  assert((int )T.size() == ncomp * (int )CP.size() && (int )T.size() == ncomp * (int )P.size());
  assert((int )T.size() == ncomp * (int )B.size() && (int )T.size() == ncomp * (int )P.size());
  assert((int )T.size() == ncomp * (int )M.size() && (int )T.size() == ncomp * (int )P.size());
  assert((int )T.size() == ncomp * (int )L.size() && (int )T.size() == ncomp * (int )P.size());

  // BOOM
  cp_(T, P, CP, ncomp, id);
  mu_(T, P, M, ncomp, id);
  lambda_(T, P, L, ncomp, id);
  beta_(T, P, B, ncomp, id);
}

void Fluide_reel_base::compute_all_pb_multiphase_(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp, int id) const
{
  assert( (int )input.size() == 4 && (int )inter.size() == 6 && (int )bord.size() == 2);

  const SpanD T = input.at("temperature"), P = input.at("pressure"), bT = input.at("bord_temperature"), bP = input.at("bord_pressure");

  SpanD R = inter.at(Loi_en_T::RHO), dP = inter.at(Loi_en_T::RHO_DP), dT = inter.at(Loi_en_T::RHO_DT),
        H = inter.at(Loi_en_T::H), dPH = inter.at(Loi_en_T::H_DP), dTH = inter.at(Loi_en_T::H_DT);

  SpanD bR = bord.at(Loi_en_T::RHO), bH = bord.at(Loi_en_T::H);

  assert((int )bT.size() == ncomp * (int )bP.size() && (int )bT.size() == ncomp * (int )bR.size());
  assert((int )bT.size() == ncomp * (int )bP.size() && (int )bT.size() == ncomp * (int )bH.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )R.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dP.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dT.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )H.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dPH.size());
  assert((int )T.size() == ncomp * (int )P.size() && (int )T.size() == ncomp * (int )dTH.size());

  // bord
  rho_(bT, bP, bR, ncomp, id);
  h_(bT, bP, bH, ncomp, id);
  // interne
  rho_(T, P, R, ncomp, id);
  dP_rho_(T, P, dP, ncomp, id);
  dT_rho_(T, P, dT, ncomp, id);
  h_(T, P, H, ncomp, id);
  dP_h_(T, P, dPH, ncomp, id);
  dT_h_(T, P, dTH, ncomp, id);
}

/*
 * Methodes utiles pour convertir les derivees en h a T (pour Pb_Multiphase).
 */
void Fluide_reel_base::H_to_T::dX_dP_T(const SpanD dX_dP_h, const SpanD dX_dh_P, SpanD dX_dP)
{
  const SpanD dp_h = ref_cast(Champ_Inc_base, z_fld_->h.valeur()).derivees()["pression"].get_span_tot();
  assert((int )dX_dP_h.size() == (int )dX_dh_P.size() && (int )dX_dP_h.size() == (int )dp_h.size()  && (int )dX_dP.size() == (int )dp_h.size());
  for (int i = 0; i < (int) dX_dP_h.size(); i++)
    dX_dP[i] = dX_dP_h[i] + dp_h[i] * dX_dh_P[i];
}

void Fluide_reel_base::H_to_T::dX_dT_P(const SpanD dX_dP_h, const SpanD dX_dh_P, SpanD dX_dT )
{
  const SpanD dT_h = ref_cast(Champ_Inc_base, z_fld_->h.valeur()).derivees()["temperature"].get_span_tot();
  assert((int )dX_dP_h.size() == (int )dT_h.size() && (int )dX_dh_P.size() == (int )dT_h.size() && (int )dX_dT.size() == (int )dT_h.size() );
  for (int i = 0; i < (int) dX_dP_h.size(); i++)
    dX_dT[i] = dT_h[i] * dX_dh_P[i];
}
