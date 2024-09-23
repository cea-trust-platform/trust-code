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

#include <Source_Flux_interfacial_base.h>
#include <Viscosite_turbulente_base.h>
#include <Flux_interfacial_base.h>
#include <Changement_phase_base.h>
#include <Operateur_Diff_base.h>
#include <Champ_Inc_P0_base.h>
#include <Aire_interfaciale.h>
#include <Milieu_composite.h>
#include <Champ_Face_base.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Synonyme_info.h>
#include <Matrix_tools.h>
#include <EOS_to_TRUST.h>
#include <Array_tools.h>
#include <Domaine_VF.h>
#include <Domaine.h>

Implemente_base(Source_Flux_interfacial_base,"Source_Flux_interfacial_base", Sources_Multiphase_base);

Sortie& Source_Flux_interfacial_base::printOn(Sortie& os) const { return os; }

Entree& Source_Flux_interfacial_base::readOn(Entree& is)
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  if (!pbm.has_correlation("flux_interfacial"))
    Process::exit(que_suis_je() + " : a flux_interfacial correlation must be defined in the global correlations { } block!");

  const bool res_en_T = pbm.resolution_en_T();
  if (!res_en_T) Process::exit("Source_Flux_interfacial_base::readOn NOT YET PORTED TO ENTHALPY EQUATION ! TODO FIXME !!");

  correlation_ = pbm.get_correlation("flux_interfacial");

  dv_min = ref_cast(Flux_interfacial_base, correlation_.valeur()).dv_min();

  return is;
}

void Source_Flux_interfacial_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Inc_P0_base& ch = ref_cast(Champ_Inc_P0_base, equation().inconnue());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleTab& inco = ch.valeurs();

  /* on doit pouvoir ajouter / soustraire les equations entre composantes */
  int i, j, e, n, N = inco.line_size();
  if (N == 1) return;
  std::set<int> idx;
  for (auto &&n_m : matrices)
    if (n_m.first.find("/") == std::string::npos) /* pour ignorer les inconnues venant d'autres problemes */
      {
        Matrice_Morse& mat = *n_m.second, mat2;
        const DoubleTab& dep = equation().probleme().get_champ(n_m.first).valeurs();
        const int M = dep.line_size();
        IntTrav sten(0, 2);

        if (n_m.first == "temperature" || n_m.first == "pression" || n_m.first == "alpha" || n_m.first == "interfacial_area" ) /* temperature/pression: dependance locale */
          for (e = 0; e < domaine.nb_elem(); e++)
            for (n = 0; n < N; n++)
              for (int m = 0; m < M; m++) sten.append_line(N * e + n, M * e + m);
        if (mat.nb_colonnes())
          for (e = 0; e < domaine.nb_elem(); e++) /* autres variables: on peut melanger les composantes*/
            {
              for (idx.clear(), n = 0, i = N * e; n < N; n++, i++)
                for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
                  idx.insert(mat.get_tab2()(j) - 1); //idx : ensemble des colonnes dont depend au moins une ligne des N composantes en e
              for (n = 0, i = N * e; n < N; n++, i++)
                for (auto &&x : idx) sten.append_line(i, x); //ajout de cette depedance a toutes les lignes
            }
        tableau_trier_retirer_doublons(sten);
        Matrix_tools::allocate_morse_matrix(inco.size_totale(), dep.size_totale(), sten, mat2);
        mat.nb_colonnes() ? mat += mat2 : mat = mat2;
      }
}

void Source_Flux_interfacial_base::completer()
{
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  int N = equation().inconnue().valeurs().line_size();
  if (!sub_type(Source_Flux_interfacial_base, equation().sources().dernier().valeur()))
    Process::exit(que_suis_je() + " : Source_Flux_interfacial_base must be the last source term in the source term declaration list of the " + equation().que_suis_je() + " equation ! ");

  if (sub_type(Energie_Multiphase, equation()))
    {
      qpi_.resize(0, N, N), domaine.domaine().creer_tableau_elements(qpi_);
      dT_qpi_.resize(0, N, N, N), domaine.domaine().creer_tableau_elements(dT_qpi_);
      da_qpi_.resize(0, N, N, N), domaine.domaine().creer_tableau_elements(da_qpi_);
      dp_qpi_.resize(0, N, N), domaine.domaine().creer_tableau_elements(dp_qpi_);
    }

  if(ref_cast(Operateur_Diff_base, equation().probleme().equation(0).operateur(0).l_op_base()).is_turb()) is_turb_ = 1;
}

DoubleTab& Source_Flux_interfacial_base::qpi() const
{
  if (sub_type(Energie_Multiphase, equation())) return qpi_;
  return ref_cast(Source_Flux_interfacial_base, ref_cast(Energie_Multiphase, ref_cast(Pb_Multiphase, equation().probleme()).equation_energie()).sources().dernier().valeur()).qpi();
}

DoubleTab& Source_Flux_interfacial_base::dT_qpi() const
{
  if (sub_type(Energie_Multiphase, equation())) return dT_qpi_;
  return ref_cast(Source_Flux_interfacial_base, ref_cast(Energie_Multiphase, ref_cast(Pb_Multiphase, equation().probleme()).equation_energie()).sources().dernier().valeur()).dT_qpi();
}

DoubleTab& Source_Flux_interfacial_base::da_qpi() const
{
  if (sub_type(Energie_Multiphase, equation())) return da_qpi_;
  return ref_cast(Source_Flux_interfacial_base, ref_cast(Energie_Multiphase, ref_cast(Pb_Multiphase, equation().probleme()).equation_energie()).sources().dernier().valeur()).da_qpi();
}

DoubleTab& Source_Flux_interfacial_base::dp_qpi() const
{
  if (sub_type(Energie_Multiphase, equation())) return dp_qpi_;
  return ref_cast(Source_Flux_interfacial_base, ref_cast(Energie_Multiphase, ref_cast(Pb_Multiphase, equation().probleme()).equation_energie()).sources().dernier().valeur()).dp_qpi();
}

void Source_Flux_interfacial_base::mettre_a_jour(double temps)
{
  qpi_ = 0, dT_qpi_ = 0, da_qpi_ = 0, dp_qpi_ = 0;
}

void Source_Flux_interfacial_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Champ_Inc_P0_base& ch = ref_cast(Champ_Inc_P0_base, equation().inconnue());
  // Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleVect& pe = milc.porosite_elem(), &ve = domaine.volumes();
  const tabs_t& der_h = ref_cast(Champ_Inc_base, milc.enthalpie()).derivees();
  const Champ_base& ch_rho = milc.masse_volumique();
  const Champ_Inc_base& ch_alpha = pbm.equation_masse().inconnue(), &ch_a_r = pbm.equation_masse().champ_conserve(),
                        &ch_temp = pbm.equation_energie().inconnue(), &ch_p = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression(),
                         *pch_rho = sub_type(Champ_Inc_base, ch_rho) ? &ref_cast(Champ_Inc_base, ch_rho) : nullptr;

  const DoubleTab& inco = ch.valeurs(), &alpha = ch_alpha.valeurs(), &press = ch_p.valeurs(), &temp  = ch_temp.valeurs(), &temp_p  = ch_temp.passe(),
                   &h = milc.enthalpie().valeurs(), *dP_h = der_h.count("pression") ? &der_h.at("pression") : nullptr, *dT_h = der_h.count("temperature") ? &der_h.at("temperature") : nullptr,
                    &lambda = milc.conductivite()->passe(), &mu = milc.viscosite_dynamique()->passe(), &rho = milc.masse_volumique()->passe(), &Cp = milc.capacite_calorifique()->passe(),
                     &p_ar = ch_a_r.passe(), &a_r = ch_a_r.valeurs(), &qi = qpi(), &dTqi = dT_qpi(), &daqi = da_qpi(), &dpqi = dp_qpi(),
                      *d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr,
                       *k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : nullptr;

  Matrice_Morse *Mp = matrices.count("pression")    ? matrices.at("pression")    : nullptr,
                 *Mt = matrices.count("temperature") ? matrices.at("temperature") : nullptr,
                  *Ma = matrices.count("alpha") ? matrices.at("alpha") : nullptr,
                   *Mai = matrices.count("interfacial_area") ? matrices.at("interfacial_area") : nullptr;

  int i, j, col, e, d, D = dimension, k, l, n, N = inco.line_size(), is_therm;
  const int cL = (lambda.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), cR = (rho.dimension_tot(0) == 1), cCp = (Cp.dimension_tot(0) == 1);

  const Flux_interfacial_base& correlation_fi = ref_cast(Flux_interfacial_base, correlation_.valeur());
  const Changement_phase_base *correlation_G = pbm.has_correlation("changement_phase") ? &ref_cast(Changement_phase_base, pbm.get_correlation("changement_phase")) : nullptr;
  double dt = equation().schema_temps().pas_de_temps(), alpha_min = 1.e-6;

  // Viscosite turbulente pour les correlations qui en ont besoin
  DoubleTrav nut;
  if (is_turb_)
    {
      nut.resize(0, N);
      MD_Vector_tools::creer_tableau_distribue(equation().inconnue().valeurs().get_md_vector(), nut); //Necessary to compare size in eddy_viscosity()
      const Viscosite_turbulente_base& corr_visc_turb = ref_cast(Viscosite_turbulente_base, *ref_cast(Operateur_Diff_base, equation().probleme().equation(0).operateur(0).l_op_base()).correlation_viscosite_turbulente());
      corr_visc_turb.eddy_viscosity(nut);
    }

  /* limiteur de changement de phase : on limite gamma pour eviter d'avoir alpha_k < 0 dans une phase */
  /* pour cela, on assemble l'equation de masse sans changement de phase */
  DoubleTrav sec_m(alpha); //residus
  std::map<std::string, Matrice_Morse> mat_m_stockees;
  matrices_t mat_m; //derivees
  for (auto &&n_m : matrices)
    if (n_m.first.find("/") == std::string::npos) /* pour ignorer les inconnues venant d'autres problemes */
      {
        Matrice_Morse& dst = mat_m_stockees[n_m.first], &src = *n_m.second;
        dst.get_set_tab1().ref_array(src.get_set_tab1()); // memes stencils que celui de l'equation courante
        dst.get_set_tab2().ref_array(src.get_set_tab2());
        dst.set_nb_columns(src.nb_colonnes());
        dst.get_set_coeff().resize(src.get_set_coeff().size()); //coeffs nuls
        mat_m[n_m.first] = &dst;
      }
  const Masse_Multiphase& eq_m = ref_cast(Masse_Multiphase, pbm.equation_masse());
  for (i = 0; i < eq_m.nombre_d_operateurs(); i++) /* tous les operateurs */
    eq_m.operateur(i).l_op_base().ajouter_blocs(mat_m, sec_m, semi_impl);
  for (i = 0; i < eq_m.sources().size(); i++)
    if (!sub_type(Source_Flux_interfacial_base, eq_m.sources()(i).valeur())) /* toutes les sources sauf le flux interfacial */
      eq_m.sources()(i)->ajouter_blocs(mat_m, sec_m, semi_impl);
  std::vector<std::array<Matrice_Morse *, 2>> vec_m; //vecteur "matrice source, matrice de destination"
  for (auto &&n_m : matrices)
    if (n_m.first.find("/") == std::string::npos && mat_m[n_m.first]->get_tab1().size() > 1) vec_m.push_back({{ mat_m[n_m.first], n_m.second }});

  /* elements */
  //coefficients et plein de derivees...
  DoubleTrav dT_G(N), da_G(N), nv(N, N);
  Flux_interfacial_base::input_t in;
  Flux_interfacial_base::output_t out;
  DoubleTab& hi = out.hi, &dT_hi = out.dT_hi, &da_hi = out.da_hi, &dP_hi = out.dp_hi;
  hi.resize(N, N), dT_hi.resize(N, N, N), da_hi.resize(N, N, N), dP_hi.resize(N, N), in.v.resize(N, D);

  // Et pour les methodes span de la classe Saturation
  const int nbelem = domaine.nb_elem(), nb_max_sat =  N * (N-1) /2; // oui !! suite arithmetique !!
  DoubleTrav Ts_tab(nbelem,nb_max_sat), dPTs_tab(nbelem,nb_max_sat), Hvs_tab(nbelem,nb_max_sat), Hls_tab(nbelem,nb_max_sat), dPHvs_tab(nbelem,nb_max_sat), dPHls_tab(nbelem,nb_max_sat), Lvap_tab(nbelem,nb_max_sat), dP_Lvap_tab(nbelem,nb_max_sat), Sigma_tab(nbelem,nb_max_sat);

  // fill velocity at elem tab
  DoubleTab pvit_elem(0, N * D);
  domaine.domaine().creer_tableau_elements(pvit_elem);
  const Champ_Face_base& ch_vit = ref_cast(Champ_Face_base,ref_cast(Pb_Multiphase, equation().probleme()).equation_qdm().inconnue());
  ch_vit.get_elem_vector_field(pvit_elem);

  // remplir les tabs ...
  for (k = 0; k < N; k++)
    for (l = k + 1; l < N; l++)
      if (milc.has_saturation(k, l))
        {
          Saturation_base& z_sat = milc.get_saturation(k, l);
          const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
          // XXX XXX XXX
          // Attention c'est dangereux ! on suppose pour le moment que le champ de pression a 1 comp. Par contre la taille de res est nb_max_sat*nbelem !!
          // Aussi, on passe le Span le nbelem pour le champ de pression et pas nbelem_tot ....
          assert(press.line_size() == 1);

          // recuperer Tsat et sigma ...
          const DoubleTab& sig = z_sat.get_sigma_tab(), &tsat = z_sat.get_Tsat_tab();

          // fill in the good case
          for (int ii = 0; ii < nbelem; ii++)
            {
              Ts_tab(ii, ind_trav) = tsat(ii);
              Sigma_tab(ii, ind_trav) = sig(ii);
            }

          // remplir les autres
          MSatSpanD sats_all = { };
          sats_all.insert( { SAT::T_SAT_DP, dPTs_tab.get_span() });
          sats_all.insert( { SAT::HV_SAT, Hvs_tab.get_span() });
          sats_all.insert( { SAT::HL_SAT, Hls_tab.get_span() });
          sats_all.insert( { SAT::HV_SAT_DP, dPHvs_tab.get_span() });
          sats_all.insert( { SAT::HL_SAT_DP, dPHls_tab.get_span() });
          sats_all.insert( { SAT::LV_SAT, Lvap_tab.get_span() });
          sats_all.insert( { SAT::LV_SAT_DP, dP_Lvap_tab.get_span() });

          z_sat.compute_all_flux_interfacial_pb_multiphase(press.get_span() /* elem reel */, sats_all, nb_max_sat, ind_trav);
        }

  for (e = 0; e < domaine.nb_elem(); e++)
    {
      double vol = pe(e) * ve(e), x, G = 0, dh = milc.diametre_hydraulique_elem()(e), dP_G = 0.; // E. Saikali : initialise dP_G ici sinon fuite memoire ...
      for (in.v = 0, d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          in.v(n, d) = pvit_elem(e, N * d + n);
      for (nv = 0, d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          for (k = 0 ; k<N ; k++) nv(n, k) += std::pow(pvit_elem(e, N * d + n) - ((n!=k) ? pvit_elem(e, N * d + k) : 0) , 2); // nv(n,n) = ||v(n)||, nv(n, k!=n) = ||v(n)-v(k)||
      for (n = 0; n < N; n++)
        for (k = 0 ; k<N ; k++) nv(n, k) = std::max(sqrt(nv(n, k)), dv_min);
      //coeffs d'echange vers l'interface (explicites)
      in.dh = dh, in.alpha = &alpha(e, 0), in.T = &temp(e, 0),  in.T_passe = &temp_p(e, 0), in.p = press(e, 0), in.nv = &nv(0, 0), in.h = &h(e, 0), in.dT_h = dT_h ? &(*dT_h)(e, 0) : nullptr, in.dP_h = dP_h ? &(*dP_h)(e, 0) : nullptr;
      in.lambda = &lambda(!cL * e, 0), in.mu = &mu(!cM * e, 0), in.rho = &rho(!cR * e, 0), in.Cp = &Cp(!cCp * e, 0), in.e = e, in.Lvap = &Lvap_tab(e, 0), in.dP_Lvap = &dP_Lvap_tab(e, 0), in.sigma = &Sigma_tab(e,0);
      in.d_bulles = (d_bulles) ? &(*d_bulles)(e,0) : nullptr, in.k_turb = (k_turb) ? &(*k_turb)(e,0) : nullptr, in.nut = (is_turb_) ? &nut(e,0) : nullptr;
      correlation_fi.coeffs(in, out);

      for (k = 0; k < N; k++)
        for (l = k + 1; l < N; l++)
          if (milc.has_saturation(k, l)) //flux phase k <-> phase l si saturation
            {
              Saturation_base& sat = milc.get_saturation(k, l);
              const int i_sat = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !

              if (correlation_G) /* taux de changement de phase par une correlation */
                G = correlation_G->calculer(k, l, dh, &alpha(e, 0), &temp(e, 0), press(e, 0), &nv(0), &lambda(!cL * e, 0), &mu(!cM * e, 0), &rho(!cM * e, 0), &Cp(!cCp * e, 0),
                                            sat, dT_G, da_G, dP_G);

              /* limite thermique */
              double Tk = temp(e, k), Tl = temp(e, l), Ts = Ts_tab(e,i_sat), dP_Ts = dPTs_tab(e,i_sat), //temperature de chaque cote + Tsat + derivees
                     phi = hi(k, l) * (Tk - Ts) + hi(l, k) * (Tl - Ts) + qi(e, k, l) / vol, L = (phi < 0 ? h(e, l) : Hvs_tab(e,i_sat)) - (phi > 0 ? h(e, k) : Hls_tab(e,i_sat));
              if ((is_therm = !correlation_G || std::fabs(G) > std::fabs(phi / L))) G = phi / L;
              /* enthalpies des phases (dans le sens correspondant au mode choisi pour G) */

              /* G est-il limite par l'evanescence cote k ou l ? */
              int n_lim = G > 0 ? k : l, sgn = G > 0 ? 1 : -1; //phase sortante
              double Glim = sec_m(e, n_lim) / vol + p_ar(e, n_lim) / dt; //changement de phase max acceptable par cette phase
              if (std::fabs(G) < Glim) n_lim = -1;       //G ne rend pas la phase evanescente -> pas de limitation (n_lim = -2)
              else G = (G > 0 ? 1 : -1) * Glim;//la phase serait evanescente a cause de G -> on le bloque a G_lim (n_lim = k / l)

              double hk = G > 0 ? h(e, k) : Hls_tab(e,i_sat), dTk_hk = G > 0 && dT_h ? (*dT_h)(e, k) : 0, dP_hk = G > 0 ? (dP_h ? (*dP_h)(e, k) : 0) : dPHls_tab(e,i_sat),
                       hl = G < 0 ? h(e, l) : Hvs_tab(e,i_sat), dTl_hl = G < 0 && dT_h ? (*dT_h)(e, l) : 0, dP_hl = G < 0 ? (dP_h ? (*dP_h)(e, l) : 0) : dPHvs_tab(e,i_sat);
              if (n_lim < 0 && is_therm) /* derivees de G en limite thermique */
              {
                  double dP_phi = dP_hi(k, l) * (Tk - Ts) + dP_hi(l, k) * (Tl - Ts) - (hi(k, l) + hi(l, k)) * dP_Ts + dpqi(e, k, l) / vol,
                         dTk_L = -dTk_hk, dTl_L = dTl_hl, dP_L = dP_hl - dP_hk;
                  dP_G = (dP_phi - G * dP_L) / L;
                  for (n = 0; n < N; n++) dT_G(n) = ((n == k) * (hi(k, l) - G * dTk_L) + (n == l) * (hi(l, k) - G * dTl_L) + dT_hi(k, l, n) * (Tk - Ts) + dT_hi(l, k, n) * (Tl - Ts) + dTqi(e, k, l, n)) / L;
                  for (n = 0; n < N; n++) da_G(n) = (da_hi(k, l, n) * (Tk - Ts) + da_hi(l, k, n) * (Tl - Ts) + daqi(e, k, l, n)) / L;
                }


              if (sub_type(Masse_Multiphase, equation())) //eq de masse -> changement de phase
                {
                  for (i = 0; i < 2; i++) secmem(e, i ? l : k) -= vol * (i ? -1 : 1) * G;
                  if (n_lim < 0) /* G par limite thermique */
                    {
                      if (Ma)
                        for (i = 0; i < 2; i++)
                          for (n = 0; n < N; n++)  //derivees en alpha
                            (*Ma)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * da_G(n);
                      if (Mt)
                        for (i = 0; i < 2; i++)
                          for (n = 0; n < N; n++)  //derivees en T
                            (*Mt)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * dT_G(n);
                      if (Mp)
                        for (i = 0; i < 2; i++) //derivees en p
                          (*Mp)(N * e + (i ? l : k), e) += vol * (i ? -1 : 1) * dP_G;
                    }
                  else for (auto &s_d : vec_m) /* G par evanescence */
                      for (j = s_d[0]->get_tab1()(N * e + n_lim) - 1; j < s_d[0]->get_tab1()(N * e + n_lim + 1) - 1; j++)
                        for (col = s_d[0]->get_tab2()(j) - 1, x = -s_d[0]->get_coeff()(j), i = 0; i < 2; i++)
                          (*s_d[1])(N * e + (i ? l : k), col) += (i ? -1 : 1) * sgn * x;
                }
              else if (sub_type(Energie_Multiphase, equation())) //eq d'energie -> transfert de chaleur
                {
                  //on suppose que la limite thermique s'applique d'un cote : c (=0,1) / n_c (=k,l) / signe du flux sortant de la phase k : s_c (=1,-1)
                  int c = (a_r(e, k) > a_r(e, l)), n_c = c ? l : k, n_d = c ? k : l, s_c = c ? -1 : 1;
                  double Tc = c ? Tl : Tk, hc = c ? hl : hk, dT_hc = c ? dTl_hl : dTk_hk, dP_hc = c ? dP_hl : dP_hk;
                  for (i = 0; i < 2; i++) secmem(e, i ? l : k)-= vol * (i ? -1 : 1) * (s_c *        hi(n_c, n_d) * (Tc - Ts)                              + G * hc)                                              - (i != c) * qi(e, k, l);
                  /* derivees (y compris celles en G, sauf dans le cas limite)*/
                  if (Ma)
                    for (i = 0; i < 2; i++)
                      for (n = 0; n < N; n++) //derivees en alpha
                        (*Ma)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * (s_c *  da_hi(n_c, n_d, n) * (Tc - Ts)                              + (n_lim < 0) * da_G(n) * hc)                          - (i != c) * daqi(e, k, l, n);
                  if (Mt)
                    for (i = 0; i < 2; i++)
                      for (n = 0; n < N; n++) //derivees en T
                        (*Mt)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * (s_c * (dT_hi(n_c, n_d, n) * (Tc - Ts) + (n == n_c) * hi(n_c, n_d)) + (n_lim < 0) * dT_G(n) * hc + G * (n == n_c) * dT_hc) - (i != c) * dTqi(e, k, l, n);
                  if (Mp)
                    for (i = 0; i < 2; i++) //derivees en p
                      (*Mp)(N * e + (i ? l : k), e)           += vol * (i ? -1 : 1) * (s_c * (dP_hi(n_c, n_d)    * (Tc - Ts) - hi(n_c, n_d) * dP_Ts)      + (n_lim < 0) * dP_G * hc + G * dP_hc)                 - (i != c) * dpqi(e, k, l);
                  if (n_lim >= 0)
                    for (auto &s_d : vec_m) /* derivees de G dans le cas evanescent */
                      for (j = s_d[0]->get_tab1()(N * e + n_lim) - 1; j < s_d[0]->get_tab1()(N * e + n_lim + 1) - 1; j++)
                        for (col = s_d[0]->get_tab2()(j) - 1, x = -s_d[0]->get_coeff()(j), i = 0; i < 2; i++)
                          (*s_d[1])(N * e + (i ? l : k), col) += (i ? -1 : 1) * hc * sgn * x;
                }
              else if (sub_type(Aire_interfaciale, equation())) //eq d'aire interfaciale ; looks like the mass equation ; not a conservation equation !
                if (k==0) // k est la phase porteuse
                  if (alpha(e, l) > alpha_min) // if the phase l is present
                    {
                      secmem(e, l) += vol * 2./3. * inco(e, l) / (alpha(e, l) * (pch_rho ? (*pch_rho).valeurs()(e, l) : rho(e, l))) * G ;
                      if (n_lim < 0) /* G par limite thermique */
                        {
                          if (Ma)   //derivees en alpha
                            {
                              (*Ma)(N * e + l , N * e + l) -= vol * 2./3. * inco(e, l) / (-alpha(e, l)*alpha(e, l) * (pch_rho ? (*pch_rho).valeurs()(e, l) : rho(e, l))) * G;
                              for (n = 0; n < N; n++) (*Ma)(N * e + l , N * e + n) -=
                                  vol * 2./3. * inco(e, l) / (alpha(e, l)              * (pch_rho ? (*pch_rho).valeurs()(e, l) : rho(e, l))) * da_G(n);
                            }
                          if (Mt)   //derivees en T
                            {
                              if (pch_rho) (*Mt)(N * e + l , N * e + l) -=
                                  vol * 2./3. * inco(e, l) / alpha(e, l)*-pch_rho->derivees().at("temperature")(e, l)/((*pch_rho).valeurs()(e, l)*(*pch_rho).valeurs()(e, l)) * G;
                              for (n = 0; n < N; n++) (*Mt)(N * e + l , N * e + n) -=
                                  vol * 2./3. * inco(e, l) / (alpha(e, l) * (pch_rho ? (*pch_rho).valeurs()(e, l) : rho(e, l))) * dT_G(n);
                            }
                          if (Mp)  //derivees en p
                            {
                              if (pch_rho) (*Mp)(N * e + l , e) -=
                                  vol * 2./3. * inco(e, l) / alpha(e, l)*-pch_rho->derivees().at("pression")(e, l)/((*pch_rho).valeurs()(e, l)*(*pch_rho).valeurs()(e, l)) * G;
                              for (n = 0; n < N; n++) (*Mp)(N * e + l , N * e + n) -=
                                  vol * 2./3. * inco(e, l) / (alpha(e, l) * (pch_rho ? (*pch_rho).valeurs()(e, l) : rho(e, l))) * dP_G;
                            }
                          if (Mai) //derivees en ai
                            {
                              (*Mai)(N * e + l , N * e + l) -= vol * 2./3. / (alpha(e, l)* (pch_rho ? (*pch_rho).valeurs()(e, l) : rho(e, l))) * G;
                            } // dAi_G a ajouter
                        }
                      else for (auto &s_d : vec_m) /* G par evanescence */
                          for (j = s_d[0]->get_tab1()(N * e + n_lim) - 1; j < s_d[0]->get_tab1()(N * e + n_lim + 1) - 1; j++)
                            for (col = s_d[0]->get_tab2()(j) - 1, x = -s_d[0]->get_coeff()(j), i = 0; i < 2; i++)
                              (*s_d[1])(N * e + l , col) -= 2./3. * inco(e, l) / (alpha(e, l) * (pch_rho ? (*pch_rho).valeurs()(e, l) : rho(e, l))) * sgn * x;
                    }
            }
          else if (sub_type(Energie_Multiphase, equation())) /* pas de saturation : echanges d'energie seulement */
            {
              //flux sortant de la phase k : hm * (Tk - Tl)
              double hm = 1. / (1. / hi(k, l) + 1. / hi(l, k)), Tk = temp(e, k), Tl = temp(e, l);
              for (i = 0; i < 2; i++) secmem(e, i ? l : k) -= vol * (i ? -1 : 1) * hm * (Tk - Tl);
              if (Mt)
                for (i = 0; i < 2; i++) //juste des derivees en T
                  {
                    (*Mt)(N * e + (i ? l : k), N * e + k) += vol * (i ? -1 : 1) * hm;
                    (*Mt)(N * e + (i ? l : k), N * e + l) += vol * (i ? 1 : -1) * hm;
                  }
            }
    }
}
