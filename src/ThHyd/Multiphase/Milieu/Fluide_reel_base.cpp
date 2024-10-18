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
  Fluide_base::readOn(is);

  if ((T_ref_ >= 0) && (h_ref_ >= 0)) Process::exit(que_suis_je() + ": T_ref and h_ref MUST NOT be both specified!");

  if (P_ref_ >= 0)
    if (T_ref_ < 0 &&  h_ref_ < 0)
      Process::exit(que_suis_je() + ": T_ref/P_ref or h_ref/P_ref MUST both be specified!");

  return is;
}

void Fluide_reel_base::set_param(Param& param)
{
  param.ajouter("T_ref", &T_ref_);
  param.ajouter("P_ref", &P_ref_);
  param.ajouter("H_ref", &h_ref_);
  set_additional_params(param);
}

void Fluide_reel_base::discretiser(const Probleme_base& pb, const Discretisation_base& dis)
{
  Cerr << "Medium discretization." << finl;

  const Domaine_dis_base& domaine_dis = pb.equation(0).domaine_dis();
  const double temps = pb.schema_temps().temps_courant();

  res_en_T_ = sub_type(Pb_Multiphase,pb) ? ref_cast(Pb_Multiphase,pb).resolution_en_T() : true;

  if (!res_en_T_ && is_incompressible())
    Process::exit("Incompressible fluid :: NOT YET PORTED TO ENTHALPY EQUATION (BUG DEDANS) ! TODO FIXME !!");

  /* masse volumique, energie interne, enthalpie : champ_inc */
  OWN_PTR(Champ_Inc_base) rho_inc, ei_inc, h_ou_T_inc;
  int nc = pb.equation(0).inconnue().nb_valeurs_temporelles();
  if (is_incompressible()) /* cas incompressible  -> rho champ uniforme */
    {
      Nom val_rho;
      val_rho = res_en_T_ ? Nom(_rho_(T_ref_, P_ref_)) : Nom(_rho_h_(h_ref_, P_ref_) /* point-to-point */);

      EChaine str(Nom("Champ_Uniforme 1 ") + val_rho);
      str >> ch_rho_;
      dis.nommer_completer_champ_physique(domaine_dis, "masse_volumique", "kg/m^3", ch_rho_, pb);
    }
  else
    {
      dis.discretiser_champ("champ_elem", domaine_dis, "masse_volumique", "kg/m^3", 1, nc, temps, rho_inc);
      ch_rho_ = rho_inc;
    }

  dis.discretiser_champ("champ_elem", domaine_dis, "energie_interne", "J/kg", 1, nc, temps, ei_inc);

  if (res_en_T_)
    dis.discretiser_champ("champ_elem", domaine_dis, "enthalpie", "J/kg", 1, nc, temps, h_ou_T_inc);
  else
    dis.discretiser_champ("champ_elem", domaine_dis, "temperature", "C", 1, nc, temps, h_ou_T_inc);

  ch_e_int_ = ei_inc, ch_h_ou_T_ = h_ou_T_inc;

  dis.discretiser_champ("champ_elem", domaine_dis, "viscosite_dynamique", "kg/m/s", 1, temps, ch_mu_);
  dis.discretiser_champ("champ_elem", domaine_dis, "viscosite_cinematique", "m2/s", 1, temps, ch_nu_);
  dis.discretiser_champ("champ_elem", domaine_dis, "diffusivite", "m2/s", 1, temps, ch_alpha_);
  dis.discretiser_champ("champ_elem", domaine_dis, "ch_alpha_fois_rho_", "kg/m/s", 1, temps, ch_alpha_fois_rho_);
  dis.discretiser_champ("champ_elem", domaine_dis, "conductivite", "W/m/K", 1, temps, ch_lambda_);
  dis.discretiser_champ("champ_elem", domaine_dis, "capacite_calorifique", "J/kg/K", 1, temps, ch_Cp_);
  dis.discretiser_champ("champ_elem", domaine_dis, "dilatabilite", "K-1", 1, temps, ch_beta_th_);
  dis.discretiser_champ("champ_elem", domaine_dis, "rho_cp_elem", "J/m^3/K", 1, temps, ch_rho_Cp_elem_);
  dis.discretiser_champ("temperature", domaine_dis, "rho_cp_comme_T", "J/m^3/K", 1, temps, ch_rho_Cp_comme_T_);

  for (auto &pch : { &ch_rho_, &ch_e_int_, &ch_h_ou_T_ })
    champs_compris_.ajoute_champ(pch->valeur());

  for (OWN_PTR(Champ_Don_base) * ch_don : { &ch_mu_, &ch_nu_, &ch_alpha_, &ch_alpha_fois_rho_, &ch_lambda_, &ch_Cp_, &ch_beta_th_ })
    champs_compris_.ajoute_champ(ch_don->valeur());

  for (Champ_Fonc_base * ch_fonc : { &(ch_rho_Cp_elem_.valeur()), &(ch_rho_Cp_comme_T_.valeur()) })
    champs_compris_.ajoute_champ(*ch_fonc);

  if (id_composite_ == -1)
    {
      Milieu_base::discretiser_porosite(pb, dis);
      Milieu_base::discretiser_diametre_hydro(pb, dis);
    }
}

int Fluide_reel_base::initialiser(const double temps)
{
  const Equation_base& eqn = res_en_T_ ? equation("temperature") : equation("enthalpie");
  Champ_Inc_base *pch_rho = sub_type(Champ_Inc_base, ch_rho_.valeur()) ? &ref_cast(Champ_Inc_base, ch_rho_.valeur()) : nullptr;
  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, ch_e_int_.valeur()), &ch_h_ou_T = ref_cast(Champ_Inc_base, ch_h_ou_T_.valeur());

  if (pch_rho)
    {
      pch_rho->associer_eqn(eqn);
      pch_rho->resize_val_bord();
      pch_rho->set_val_bord_fluide_multiphase(true);
    }

  ch_h_ou_T.associer_eqn(eqn);
  ch_h_ou_T.resize_val_bord();
  ch_h_ou_T.set_val_bord_fluide_multiphase(true);

  ch_e.associer_eqn(eqn);
  ch_e.resize_val_bord();
  ch_e.set_val_bord_fluide_multiphase(true);

  // XXX Elie Saikali : utile pour cas reprise !
  ch_e.changer_temps(temps);
  ch_h_ou_T.changer_temps(temps);

  ch_Cp_->initialiser(temps);
  ch_mu_->initialiser(temps);
  ch_nu_->initialiser(temps);
  ch_alpha_->initialiser(temps);
  ch_alpha_fois_rho_->initialiser(temps);
  ch_lambda_->initialiser(temps);
  ch_beta_th_->initialiser(temps);
  ch_rho_Cp_comme_T_->initialiser(temps);

  t_init_ = temps;

  if (is_incompressible())
    {
      mettre_a_jour(temps); // ne depend pas de p et T : on peut terminer l'initialisation
      Cerr << "The defined fluid_reel with T_ref = " << T_ref_ << " and P_ref = " << P_ref_ << " is equivalent to :" << finl;
      Cerr << "fluide_incompressible" << finl;
      Cerr << "{" << finl;
      Cerr << "    rho     champ_uniforme 1 " << ch_rho_->valeurs()(0, 0) << finl;
      Cerr << "    cp      champ_uniforme 1 " << ch_Cp_->valeurs()(0, 0) << finl;
      Cerr << "    lambda  champ_uniforme 1 " << ch_lambda_->valeurs()(0, 0) << finl;
      Cerr << "    mu      champ_uniforme 1 " << ch_mu_->valeurs()(0, 0) << finl;
      Cerr << "    beta_th champ_uniforme 1 " << ch_beta_th_->valeurs()(0, 0) << finl;
      Cerr << "}" << finl;
    }

  if (id_composite_ == -1)
    Milieu_base::initialiser_porosite(temps);

  if (id_composite_ < 0 && ch_g_.non_nul())
    ch_g_->initialiser(temps);

  return 1;
}

void Fluide_reel_base::preparer_calcul() { mettre_a_jour(t_init_); }

void Fluide_reel_base::mettre_a_jour(double t)
{
  double tp = ref_cast(Champ_Inc_base, ch_e_int_.valeur()).temps(); //pour savoir si on va tourner la roue
  ch_rho_->mettre_a_jour(t);
  ch_e_int_->mettre_a_jour(t);
  ch_h_ou_T_->mettre_a_jour(t);

  // on calcule les props (EOS)
  if (res_en_T_)
    is_incompressible() ? calculate_fluid_properties_incompressible() : calculate_fluid_properties();
  else
    is_incompressible() ? calculate_fluid_properties_enthalpie_incompressible() : calculate_fluid_properties_enthalpie();

  ch_Cp_->mettre_a_jour(t);
  ch_mu_->mettre_a_jour(t);
  ch_lambda_->mettre_a_jour(t);
  ch_nu_->mettre_a_jour(t);
  ch_alpha_->mettre_a_jour(t);
  ch_alpha_fois_rho_->mettre_a_jour(t);
  ch_beta_th_->mettre_a_jour(t);
  if (ch_rho_Cp_comme_T_.non_nul()) update_rho_cp(t);

  const Champ_Inc_base& ch_T_ou_h = res_en_T_ ? equation("temperature").inconnue() : equation("enthalpie").inconnue(),
                        &ch_p = ref_cast(Navier_Stokes_std, equation("vitesse")).pression();

  const DoubleTab& temp_ou_enthalp = ch_T_ou_h.valeurs(), &pres = ch_p.valeurs();

  DoubleTab& tab_Cp = ch_Cp_->valeurs(), &tab_mu = ch_mu_->valeurs(),
             &tab_lambda = ch_lambda_->valeurs(), &tab_alpha_fois_rho = ch_alpha_fois_rho_->valeurs(),
              &tab_nu = ch_nu_->valeurs(), &tab_alpha = ch_alpha_->valeurs(),
               &tab_beta = ch_beta_th_->valeurs(), &tab_rCp = ch_rho_Cp_comme_T_->valeurs();

  const DoubleTab& tab_rho = masse_volumique().valeurs();

  int Ni = ch_mu_->valeurs().dimension_tot(0), cR = tab_rho.dimension_tot(0) == 1;
  if (t > tp || first_maj_)
    {
      MLoiSpanD spans_interne;
      MLoiSpanD_h spans_interne_h;

      if (res_en_T_)
        spans_interne = { { Loi_en_T::CP, tab_Cp.get_span_tot() }, { Loi_en_T::MU, tab_mu.get_span_tot() },
          { Loi_en_T::LAMBDA, tab_lambda.get_span_tot() }, { Loi_en_T::BETA, tab_beta.get_span_tot() }
        };
      else
        spans_interne_h = { { Loi_en_h::CP, tab_Cp.get_span_tot() }, { Loi_en_h::MU, tab_mu.get_span_tot() },
          { Loi_en_h::LAMBDA, tab_lambda.get_span_tot() }, { Loi_en_h::BETA, tab_beta.get_span_tot() }
        };

      if (is_incompressible())
        res_en_T_ ? _compute_CPMLB_pb_multiphase_(spans_interne) : _compute_CPMLB_pb_multiphase_h_(spans_interne_h);
      else
        {
          assert(pres.line_size() == 1 && tab_Cp.line_size() == 1 && tab_mu.line_size() == 1 && tab_lambda.line_size() == 1 && tab_beta.line_size() == 1);
          const int n_comp = temp_ou_enthalp.line_size(); /* on a temp(xx,id_composite_) */
          MSpanD spans_input;

          if (res_en_T_)
            {
              spans_input = { { "temperature", temp_ou_enthalp.get_span_tot() }, { "pressure", pres.get_span_tot() } };
              compute_CPMLB_pb_multiphase_(spans_input, spans_interne, n_comp, id_composite_);
            }
          else
            {
              spans_input = { { "enthalpie", temp_ou_enthalp.get_span_tot() }, { "pressure", pres.get_span_tot() } };
              compute_CPMLB_pb_multiphase_h_(spans_input, spans_interne_h, n_comp, id_composite_);
            }
        }

      for (int i = 0; i < Ni; i++) // fill values
        {
          tab_nu(i) = tab_mu(i) / tab_rho(!cR * i);
          tab_alpha(i) = tab_lambda(i) / tab_rho(!cR * i) / tab_Cp(i);
          tab_alpha_fois_rho(i) = tab_lambda(i) / tab_Cp(i);
          tab_rCp(i) = tab_rho(!cR * i) * tab_Cp(i);
        }
    }
  first_maj_ = 0;
  if (id_composite_ == -1) Milieu_base::mettre_a_jour_porosite(t);
}

int Fluide_reel_base::check_unknown_range() const
{
  if (is_incompressible()) return 1;

  int ok = 1, zero = 0, nl = ch_e_int_->valeurs().dimension_tot(0); //on n'impose pas de contraintes aux lignes correspondant a des variables auxiliaires (eg pressions aux faces dans PolyMAC_P0P1NC)
  for (auto &&i_r : res_en_T_ ? unknown_range() : unknown_range_h())
    {
      const DoubleTab& vals = i_r.first == "pression" ? ref_cast(Navier_Stokes_std, equation("vitesse")).pression().valeurs() : equation(i_r.first).inconnue().valeurs();
      double vmin = DBL_MAX, vmax = -DBL_MAX;
      for (int i = 0, j = std::min(std::max(id_composite_, zero), vals.dimension(1) - 1); i < nl; i++)
        vmin = std::min(vmin, vals(i, j)), vmax = std::max(vmax, vals(i, j));
      ok &= Process::mp_min(vmin) >= i_r.second[0] && Process::mp_max(vmax) <= i_r.second[1];

      if (!ok)
        Cerr << "  *** WARNING <<< " << que_suis_je() << " >>> VALUES OUT OF RANGE *** : Variable : " << i_r.first << " , min/max : ( "
             << vmin << " , " << vmax << " ) & limits ( " << i_r.second[0] << " , " << i_r.second[1] << " )" << finl;
    }
  return ok;
}

void Fluide_reel_base::abortTimeStep()
{
  Fluide_base::abortTimeStep();
  ch_rho_->abortTimeStep();
  ch_e_int_->abortTimeStep();
  ch_h_ou_T_->abortTimeStep();
}

bool Fluide_reel_base::initTimeStep(double dt)
{
  if (!equation_.size()) return true; //pas d'equation associee -> ???
  const Schema_Temps_base& sch = equation_.begin()->second->schema_temps(); //on recupere le schema en temps par la 1ere equation

  /* champs dont on doit creer des cases */
  std::vector<Champ_Inc_base *> vch = { sub_type(Champ_Inc_base, ch_rho_.valeur()) ?& ref_cast(Champ_Inc_base, ch_rho_.valeur()) : nullptr,
                                        &ref_cast(Champ_Inc_base, ch_e_int_.valeur()), &ref_cast(Champ_Inc_base, ch_h_ou_T_.valeur())
                                      };
  for (auto &&pch : vch)
    if (pch)
      for (int i = 1; i <= sch.nb_valeurs_futures(); i++)
        {
          pch->changer_temps_futur(sch.temps_futur(i), i);
          pch->futur(i) = pch->valeurs();
        }
  return true;
}

void Fluide_reel_base::calculate_fluid_properties_incompressible()
{
  assert (is_incompressible() && res_en_T_);
  const Champ_Inc_base& ch_T = equation("temperature").inconnue();
  const DoubleTab& T = ch_T.valeurs(), &bT = ch_T.valeur_aux_bords();

  Champ_Inc_base& ch_h = ref_cast_non_const(Champ_Inc_base, ch_h_ou_T_.valeur());
  DoubleTab& val_h = ch_h.valeurs(), &bval_h = ch_h.val_bord();

  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, ch_e_int_.valeur());
  DoubleTab& val_e = ch_e.valeurs(), &bval_e = ch_e.val_bord();

  const int zero = 0, Ni = val_h.dimension_tot(0), Nb = bval_h.dimension_tot(0), n = std::max(id_composite_, zero);

  DoubleTab& dT_h = ch_h.derivees()["temperature"], &dT_e = ch_e.derivees()["temperature"];
  dT_h.resize(Ni, 1);
  dT_e.resize(Ni, 1);

  VectorD H_REF_(Ni), dTH_(Ni), bH_REF_(Nb), bdTH_(Nb); // Je suis desole ...
  MLoiSpanD spans_interne = { { Loi_en_T::H, SpanD(H_REF_) }, { Loi_en_T::H_DT, SpanD(dTH_) } };
  MLoiSpanD spans_bord = { { Loi_en_T::H, SpanD(bH_REF_) }, { Loi_en_T::H_DT, SpanD(bdTH_) } };
  _compute_all_pb_multiphase_(spans_interne, spans_bord);

  /*
   * On rempli h et e
   * En incompressible, on suppose e = h = h*ref + cp * (T -T_ref)
   * On a aussi cp = dh / dT
   */
  for (int i = 0; i < Ni; i++) /* interne */
    {
      val_h(i) = H_REF_[i] + dTH_[i] * (T(i, n) - T_ref_);
      val_e(i) = val_h(i);
      dT_h(i) = dTH_[i]; /* la seule derivee en incompressible */
      dT_e(i) = dT_h(i);
    }

  for (int i = 0; i < Nb; i++) /* bord */
    {
      bval_h(i) = bH_REF_[i] + bdTH_[i] * (bT(i, n) - T_ref_);
      bval_e(i) = bval_h(i);
    }
}

void Fluide_reel_base::calculate_fluid_properties_enthalpie_incompressible()
{
  assert (is_incompressible() && !res_en_T_) ;
  const Champ_Inc_base& ch_enth = equation("enthalpie").inconnue();
  const DoubleTab& enth = ch_enth.valeurs(), &benth = ch_enth.valeur_aux_bords();

  Champ_Inc_base& ch_temp = ref_cast_non_const(Champ_Inc_base, ch_h_ou_T_.valeur());
  DoubleTab& val_temp = ch_temp.valeurs(), &bval_temp = ch_temp.val_bord();

  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, ch_e_int_.valeur());
  DoubleTab& val_e = ch_e.valeurs(), &bval_e = ch_e.val_bord();

  const int zero = 0, Ni = val_temp.dimension_tot(0), Nb = bval_temp.dimension_tot(0), n = std::max(id_composite_, zero);

  DoubleTab& dh_T = ch_temp.derivees()["enthalpie"], &dh_e = ch_e.derivees()["enthalpie"];
  dh_T.resize(Ni, 1);
  dh_e.resize(Ni, 1);

  VectorD T_REF_(Ni), Cp_(Ni), bT_REF_(Nb), bCp_(Nb); // Je suis desole ...
  MLoiSpanD_h spans_interne = { { Loi_en_h::T, SpanD(T_REF_) }, { Loi_en_h::CP, SpanD(Cp_) } };
  MLoiSpanD_h spans_bord = { { Loi_en_h::T, SpanD(bT_REF_) }, { Loi_en_h::CP, SpanD(bCp_) } };
  _compute_all_pb_multiphase_h_(spans_interne, spans_bord);

  /*
   * On rempli T et e
   * En incompressible, on suppose e = h
   * T = ( h - h_ref ) / cp + T_ref
   */
  for (int i = 0; i < Ni; i++) /* interne */
    {
      val_temp(i,n) = T_REF_[i] + (( enth(i,n) - h_ref_ ) / Cp_[i] - 273.15);
      val_e(i) = enth(i);
      dh_T(i) = 1. / Cp_[i]; /* la seule derivee en incompressible */
      dh_e(i) = 1.;
    }

  for (int i = 0; i < Nb; i++) /* bord */
    {
      bval_temp(i,n) = bT_REF_[i] + (( benth(i,n) - h_ref_ ) / bCp_[i] - 273.15);
      bval_e(i) = benth(i);
    }
}

void Fluide_reel_base::calculate_fluid_properties()
{
  const Champ_Inc_base& ch_T = equation("temperature").inconnue(), &ch_p = ref_cast(Navier_Stokes_std, equation("vitesse")).pression();
  const DoubleTab& T = ch_T.valeurs(), &p = ch_p.valeurs(), &bT = ch_T.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();

  Champ_Inc_base& ch_rho = ref_cast_non_const(Champ_Inc_base, ch_rho_.valeur());
  DoubleTab& val_rho = ch_rho.valeurs(), &bval_rho = ch_rho.val_bord();

  Champ_Inc_base& ch_h = ref_cast_non_const(Champ_Inc_base, ch_h_ou_T_.valeur());
  DoubleTab& val_h = ch_h.valeurs(), &bval_h = ch_h.val_bord();

  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, ch_e_int_.valeur());
  DoubleTab& val_e = ch_e.valeurs(), &bval_e = ch_e.val_bord();

  const int n_comp = T.line_size(), zero = 0, Ni = val_h.dimension_tot(0), Nb = bval_h.dimension_tot(0), n = std::max(id_composite_, zero), m = p.line_size() == T.line_size() ? n : 0;

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

void Fluide_reel_base::calculate_fluid_properties_enthalpie()
{
  const Champ_Inc_base& ch_enth = equation("enthalpie").inconnue(),
                        &ch_p = ref_cast(Navier_Stokes_std, equation("vitesse")).pression();

  const DoubleTab& enth = ch_enth.valeurs(), &p = ch_p.valeurs(),
                   &benth = ch_enth.valeur_aux_bords(), &bp = ch_p.valeur_aux_bords();

  Champ_Inc_base& ch_rho = ref_cast_non_const(Champ_Inc_base, ch_rho_.valeur());
  DoubleTab& val_rho = ch_rho.valeurs(), &bval_rho = ch_rho.val_bord();

  Champ_Inc_base& ch_temp = ref_cast_non_const(Champ_Inc_base, ch_h_ou_T_.valeur());
  DoubleTab& val_temp = ch_temp.valeurs(), &bval_temp = ch_temp.val_bord();

  Champ_Inc_base& ch_e = ref_cast(Champ_Inc_base, ch_e_int_.valeur());
  DoubleTab& val_e = ch_e.valeurs(), &bval_e = ch_e.val_bord();

  const int n_comp = enth.line_size(), zero = 0, Ni = val_temp.dimension_tot(0),
            Nb = bval_temp.dimension_tot(0), n = std::max(id_composite_, zero),
            m = p.line_size() == enth.line_size() ? n : 0;

  if (m != 0) Process::exit("Fluide_reel_base currently supports single-component pressure field ! Call the 911 !");

  DoubleTab& dh_rho = ch_rho.derivees()["enthalpie"], &dp_rho = ch_rho.derivees()["pression"];
  DoubleTab& dh_T = ch_temp.derivees()["enthalpie"], &dp_T = ch_temp.derivees()["pression"];
  DoubleTab& dh_e = ch_e.derivees()["enthalpie"], &dp_e = ch_e.derivees()["pression"];

  dh_rho.resize(Ni,1);
  dp_rho.resize(Ni,1);
  dh_T.resize(Ni, 1);
  dp_T.resize(Ni, 1);
  dh_e.resize(Ni, 1);
  dp_e.resize(Ni, 1);

  MSpanD spans_input = { { "enthalpie", enth.get_span_tot() }, { "pressure", p.get_span_tot() }, { "bord_enthalpie", benth.get_span_tot() }, { "bord_pressure", bp.get_span_tot() } };
  MLoiSpanD_h spans_interne = { }, spans_bord = { };

  // pour rho
  spans_interne.insert( { Loi_en_h::RHO, val_rho.get_span_tot() });
  spans_interne.insert( { Loi_en_h::RHO_DP, dp_rho.get_span_tot() });
  spans_interne.insert( { Loi_en_h::RHO_DH, dh_rho.get_span_tot() });
  spans_bord.insert( { Loi_en_h::RHO, bval_rho.get_span_tot() });

  // pour h
  spans_interne.insert( { Loi_en_h::T, val_temp.get_span_tot() });
  spans_interne.insert( { Loi_en_h::T_DP, dp_T.get_span_tot() });
  spans_interne.insert( { Loi_en_h::T_DH, dh_T.get_span_tot() });
  spans_bord.insert( { Loi_en_h::T, bval_temp.get_span_tot() });

  compute_all_pb_multiphase_h_(spans_input, spans_interne, spans_bord, n_comp, n);

  // energie_interne
  for (int i = 0; i < Ni; i++) /* interne */
    {
      val_e(i) = enth(i, n) - p(i, m) / val_rho(i);
      dp_e(i) =  - 1. / val_rho(i) + p(i, m) * dp_rho(i) / std::pow(val_rho(i) , 2);
      dh_e(i) = 1.0 + p(i, m) * dh_rho(i) / std::pow(val_rho(i) , 2);
    }

  for (int i = 0; i < Nb; i++) bval_e(i) = benth(i, n) -  bp(i, m) / bval_rho(i); /* bord */
}

/*
 * *********************
 * Pour l'incompressible
 * *********************
 */

/* Lois en T */
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

/* Lois en h */
void Fluide_reel_base::_compute_CPMLB_pb_multiphase_h_(MLoiSpanD_h prop) const
{
  assert((int )prop.size() == 4);
  SpanD CP = prop.at(Loi_en_h::CP), M = prop.at(Loi_en_h::MU), L = prop.at(Loi_en_h::LAMBDA), B = prop.at(Loi_en_h::BETA);
  _cp_h_(h_ref_, P_ref_, CP);
  _mu_h_(h_ref_, P_ref_, M);
  _lambda_h_(h_ref_, P_ref_, L);
  _beta_h_(h_ref_, P_ref_, B);
}

void Fluide_reel_base::_compute_all_pb_multiphase_h_(MLoiSpanD_h inter, MLoiSpanD_h bord) const
{
  assert((int )inter.size() == 2 && (int )bord.size() == 2);
  SpanD T = inter.at(Loi_en_h::T), CP = inter.at(Loi_en_h::CP),
        bT = bord.at(Loi_en_h::T), bCp = bord.at(Loi_en_h::CP);
  _T_(h_ref_, P_ref_, T);
  _T_(h_ref_, P_ref_, bT);
  _cp_h_(h_ref_, P_ref_, bCp);
  _cp_h_(h_ref_, P_ref_, CP);
}

/*
 * *****************
 * Pour compressible
 * *****************
 */

/* Lois en T */
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
  const SpanD dp_h = ref_cast(Champ_Inc_base, z_fld_->ch_h_ou_T_.valeur()).derivees()["pression"].get_span_tot();
  assert((int )dX_dP_h.size() == (int )dX_dh_P.size() && (int )dX_dP_h.size() == (int )dp_h.size()  && (int )dX_dP.size() == (int )dp_h.size());
  for (int i = 0; i < (int) dX_dP_h.size(); i++)
    dX_dP[i] = dX_dP_h[i] + dp_h[i] * dX_dh_P[i];
}

void Fluide_reel_base::H_to_T::dX_dT_P(const SpanD dX_dP_h, const SpanD dX_dh_P, SpanD dX_dT )
{
  const SpanD dT_h = ref_cast(Champ_Inc_base, z_fld_->ch_h_ou_T_.valeur()).derivees()["temperature"].get_span_tot();
  assert((int )dX_dP_h.size() == (int )dT_h.size() && (int )dX_dh_P.size() == (int )dT_h.size() && (int )dX_dT.size() == (int )dT_h.size() );
  for (int i = 0; i < (int) dX_dP_h.size(); i++)
    dX_dT[i] = dT_h[i] * dX_dh_P[i];
}
/* Lois en h */
void Fluide_reel_base::compute_CPMLB_pb_multiphase_h_(const MSpanD input, MLoiSpanD_h prop, int ncomp, int id) const
{
  assert((int )prop.size() == 4);
  const SpanD H = input.at("enthalpie"), P = input.at("pressure");

  // BEEM
  SpanD CP = prop.at(Loi_en_h::CP), M = prop.at(Loi_en_h::MU), L = prop.at(Loi_en_h::LAMBDA), B = prop.at(Loi_en_h::BETA);
  assert((int )H.size() == ncomp * (int )CP.size() && (int )H.size() == ncomp * (int )P.size());
  assert((int )H.size() == ncomp * (int )B.size() && (int )H.size() == ncomp * (int )P.size());
  assert((int )H.size() == ncomp * (int )M.size() && (int )H.size() == ncomp * (int )P.size());
  assert((int )H.size() == ncomp * (int )L.size() && (int )H.size() == ncomp * (int )P.size());

  // BOOM
  cp_h_(H, P, CP, ncomp, id);
  mu_h_(H, P, M, ncomp, id);
  lambda_h_(H, P, L, ncomp, id);
  beta_h_(H, P, B, ncomp, id);
}

void Fluide_reel_base::compute_all_pb_multiphase_h_(const MSpanD input, MLoiSpanD_h inter, MLoiSpanD_h bord, int ncomp, int id) const
{
  assert( (int )input.size() == 4 && (int )inter.size() == 6 && (int )bord.size() == 2);

  const SpanD H = input.at("enthalpie"), P = input.at("pressure"), bH = input.at("bord_enthalpie"), bP = input.at("bord_pressure");

  SpanD R = inter.at(Loi_en_h::RHO), dP = inter.at(Loi_en_h::RHO_DP), dH = inter.at(Loi_en_h::RHO_DH),
        T = inter.at(Loi_en_h::T), dPT = inter.at(Loi_en_h::T_DP), dHT = inter.at(Loi_en_h::T_DH);

  SpanD bR = bord.at(Loi_en_h::RHO), bT = bord.at(Loi_en_h::T);

  assert((int )bH.size() == ncomp * (int )bP.size() && (int )bH.size() == ncomp * (int )bR.size());
  assert((int )bH.size() == ncomp * (int )bP.size() && (int )bH.size() == ncomp * (int )bT.size());
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )R.size());
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )dP.size());
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )dH.size());
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )T.size());
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )dPT.size());
  assert((int )H.size() == ncomp * (int )P.size() && (int )H.size() == ncomp * (int )dHT.size());

  // bord
  rho_h_(bH, bP, bR, ncomp, id);
  T_(bH, bP, bT, ncomp, id);

  // interne
  rho_h_(H, P, R, ncomp, id);
  dP_rho_h_(H, P, dP, ncomp, id);
  dh_rho_h_(H, P, dH, ncomp, id);
  T_(H, P, T, ncomp, id);
  dP_T_(H, P, dPT, ncomp, id);
  dh_T_(H, P, dHT, ncomp, id);
}
