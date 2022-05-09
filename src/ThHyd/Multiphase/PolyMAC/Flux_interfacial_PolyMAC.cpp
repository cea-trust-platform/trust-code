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
// File:        Flux_interfacial_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/PolyMAC
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Flux_interfacial_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Champ_P0_PolyMAC.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Flux_interfacial_base.h>
#include <Changement_phase_base.h>
#include <Milieu_composite.h>
#include <Op_Diff_PolyMAC_Elem.h>
#include <Op_Diff_PolyMAC_V2_Elem.h>
#include <Aire_interfaciale.h>

Implemente_instanciable(Flux_interfacial_PolyMAC,"Flux_interfacial_P0_PolyMAC|Flux_interfacial_P0_PolyMAC_V2", Source_base);

Sortie& Flux_interfacial_PolyMAC::printOn(Sortie& os) const
{
  return os;
}

Entree& Flux_interfacial_PolyMAC::readOn(Entree& is)
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  if (!pbm.has_correlation("flux_interfacial")) Process::exit(que_suis_je() + " : a flux_interfacial correlation must be defined in the global correlations { } block!");
  correlation_ = pbm.get_correlation("flux_interfacial");
  return is;
}

void Flux_interfacial_PolyMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();

  /* on doit pouvoir ajouter / soustraire les equations entre composantes */
  int i, j, e, n, N = inco.line_size();
  if (N == 1) return;
  std::set<int> idx;
  for (auto &&n_m : matrices) if (n_m.first.find("/") == std::string::npos) /* pour ignorer les inconnues venant d'autres problemes */
      {
        Matrice_Morse& mat = *n_m.second, mat2;
        const DoubleTab& dep = equation().probleme().get_champ(n_m.first).valeurs();
        int m, nc = dep.dimension_tot(0), M = dep.line_size();
        IntTrav sten(0, 2);
        sten.set_smart_resize(1);
        if (n_m.first == "temperature" || n_m.first == "pression" || n_m.first == "alpha" || n_m.first == "interfacial_area" ) /* temperature/pression: dependance locale */
          for (e = 0; e < zone.nb_elem(); e++) for (n = 0; n < N; n++) for (m = 0; m < M; m++) sten.append_line(N * e + n, M * e + m);
        else if (mat.nb_colonnes()) for (e = 0; e < zone.nb_elem(); e++) /* autres variables: on peut melanger les composantes*/
            {
              for (idx.clear(), n = 0, i = N * e; n < N; n++, i++) for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
                  idx.insert(mat.get_tab2()(j) - 1); //idx : ensemble des colonnes dont depend au moins une ligne des N composantes en e
              for (n = 0, i = N * e; n < N; n++, i++) for (auto &&x : idx) sten.append_line(i, x); //ajout de cette depedance a toutes les lignes
            }
        else continue;
        Matrix_tools::allocate_morse_matrix(N * zone.nb_elem_tot(), M * nc, sten, mat2);
        mat.nb_colonnes() ? mat += mat2 : mat = mat2;
      }

}

void Flux_interfacial_PolyMAC::completer()
{
  if (!sub_type(Flux_interfacial_PolyMAC, equation().sources()(equation().sources().size()-1).valeur())) Process::exit(que_suis_je() + " : Flux_interfacial_PolyMAC must be the last source term in the source term declaration list of the " + equation().que_suis_je() + " equation ! ");
}


void Flux_interfacial_PolyMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  // Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC, equation().zone_dis().valeur());
  const DoubleVect& pe = zone.porosite_elem(), &ve = zone.volumes();
  const tabs_t& der_h = ref_cast(Champ_Inc_base, milc.enthalpie()).derivees();
  const Champ_base& ch_rho = milc.masse_volumique();
  const Champ_Inc_base& ch_alpha = pbm.eq_masse.inconnue().valeur(), &ch_a_r = pbm.eq_masse.champ_conserve(), &ch_vit = pbm.eq_qdm.inconnue().valeur(),
                        &ch_temp = pbm.eq_energie.inconnue().valeur(), &ch_p = pbm.eq_qdm.pression().valeur(),
                         *pch_rho = sub_type(Champ_Inc_base, ch_rho) ? &ref_cast(Champ_Inc_base, ch_rho) : NULL;
  const DoubleTab& inco = ch.valeurs(), &alpha = ch_alpha.valeurs(), &press = ch_p.valeurs(), &temp  = ch_temp.valeurs(), &pvit = ch_vit.passe(),
                   &h = milc.enthalpie().valeurs(), *dP_h = der_h.count("pression") ? &der_h.at("pression") : NULL, *dT_h = der_h.count("temperature") ? &der_h.at("temperature") : NULL,
                    &lambda = milc.conductivite().passe(), &mu = milc.viscosite_dynamique().passe(), &rho = milc.masse_volumique().passe(), &Cp = milc.capacite_calorifique().passe(),
                     &p_ar = ch_a_r.passe(), &a_r = ch_a_r.valeurs(),
                      *q_pi = sub_type(Op_Diff_PolyMAC_Elem, pbm.eq_energie.operateur(0).l_op_base()) ? &ref_cast(Op_Diff_PolyMAC_Elem, pbm.eq_energie.operateur(0).l_op_base()).q_pi() :
                              (sub_type(Op_Diff_PolyMAC_V2_Elem, pbm.eq_energie.operateur(0).l_op_base()) ? &ref_cast(Op_Diff_PolyMAC_V2_Elem, pbm.eq_energie.operateur(0).l_op_base()).q_pi() : NULL) ;
  Matrice_Morse *Mp = matrices.count("pression")    ? matrices.at("pression")    : NULL,
                 *Mt = matrices.count("temperature") ? matrices.at("temperature") : NULL,
                  *Ma = matrices.count("alpha") ? matrices.at("alpha") : NULL,
                   *Mai = matrices.count("interfacial_area") ? matrices.at("interfacial_area") : NULL;

  int i, j, col, e, d, D = dimension, k, l, n, N = inco.line_size(), nf_tot = zone.nb_faces_tot(),
                       cL = lambda.dimension_tot(0) == 1, cM = mu.dimension_tot(0) == 1, cR = rho.dimension_tot(0) == 1, cCp = Cp.dimension_tot(0) == 1, is_therm;
  const Flux_interfacial_base& correlation_fi = ref_cast(Flux_interfacial_base, correlation_.valeur().valeur());
  const Changement_phase_base *correlation_G = pbm.has_correlation("changement_phase") ? &ref_cast(Changement_phase_base, pbm.get_correlation("changement_phase").valeur()) : NULL;
  double dt = equation().schema_temps().pas_de_temps(), alpha_min = 1.e-6;

  /* limiteur de changement de phase : on limite gamma pour eviter d'avoir alpha_k < 0 dans une phase */
  /* pour cela, on assemble l'equation de masse sans changement de phase */
  DoubleTrav sec_m(alpha); //residus
  std::map<std::string, Matrice_Morse> mat_m_stockees;
  matrices_t mat_m; //derivees
  for (auto &&n_m : matrices) if (n_m.first.find("/") == std::string::npos) /* pour ignorer les inconnues venant d'autres problemes */
      {
        Matrice_Morse& dst = mat_m_stockees[n_m.first], &src = *n_m.second;
        dst.get_set_tab1().ref_array(src.get_set_tab1()); // memes stencils que celui de l'equation courante
        dst.get_set_tab2().ref_array(src.get_set_tab2());
        dst.set_nb_columns(src.nb_colonnes());
        dst.get_set_coeff().resize(src.get_set_coeff().size()); //coeffs nuls
        mat_m[n_m.first] = &dst;
      }
  const Masse_Multiphase& eq_m = pbm.eq_masse;
  for (i = 0; i < eq_m.nombre_d_operateurs(); i++) /* tous les operateurs */
    eq_m.operateur(i).l_op_base().ajouter_blocs(mat_m, sec_m, semi_impl);
  for (i = 0; i < eq_m.sources().size(); i++) if (!sub_type(Flux_interfacial_PolyMAC, eq_m.sources()(i).valeur())) /* toutes les sources sauf le flux interfacial */
      eq_m.sources()(i).valeur().ajouter_blocs(mat_m, sec_m, semi_impl);
  std::vector<std::array<Matrice_Morse *, 2>> vec_m; //vecteur "matrice source, matrice de destination"
  for (auto &&n_m : matrices) if (n_m.first.find("/") == std::string::npos && mat_m[n_m.first]->get_tab1().size() > 1) vec_m.push_back({{ mat_m[n_m.first], n_m.second }});

  /* elements */
  //coefficients et plein de derivees...
  DoubleTrav hi(N, N), dT_hi(N, N, N), da_hi(N, N, N), dP_hi(N, N), dT_phi(N), da_phi(N), dT_G(N), da_G(N), nv(N, N);
  for (e = 0; e < zone.nb_elem(); e++)
    {
      //  double vol = pe(e) * ve(e), x, G = 0, dv_min = 0.1, dh = zone.diametre_hydraulique_elem()(e, 0), dP_G = 0.; // E. Saikali : initialise dP_G ici sinon fuite memoire ...
      // PL ToDo: Uniformiser diametre_hydraulique_elem (vecteur) pour C3D et F5
      double vol = pe(e) * ve(e), x, G = 0, dv_min = 0.1, dh = zone.diametre_hydraulique_elem()(e), dP_G = 0.; // E. Saikali : initialise dP_G ici sinon fuite memoire ...
      for (nv = 0, d = 0; d < D; d++) for (n = 0; n < N; n++) for (k = 0 ; k<N ; k++) nv(n, k) += std::pow( pvit(nf_tot + D * e + d, n) - ((n!=k) ? pvit(nf_tot + D * e + d, k) : 0) , 2); // nv(n,n) = ||v(n)||, nv(n, k!=n) = ||v(n)-v(k)||
      for (n = 0; n < N; n++) for (k = 0 ; k<N ; k++) nv(n, k) = std::max(sqrt(nv(n, k)), dv_min);
      //coeffs d'echange vers l'interface (explicites)
      correlation_fi.coeffs(dh, &alpha(e, 0), &temp(e, 0), press(e, 0), &nv(0, 0),
                            &lambda(!cL * e, 0), &mu(!cM * e, 0), &rho(!cR * e, 0), &Cp(!cCp * e, 0), e, hi, dT_hi, da_hi, dP_hi);

      for (k = 0; k < N; k++) for (l = k + 1; l < N; l++)
          if (milc.has_saturation(k, l)) //flux phase k <-> phase l si saturation
            {
              Saturation_base& sat = milc.get_saturation(k, l);
              if (correlation_G) /* taux de changement de phase par une correlation */
                G = correlation_G->calculer(k, l, dh, &alpha(e, 0), &temp(e, 0), press(e, 0), &nv(0), &lambda(!cL * e, 0), &mu(!cM * e, 0), &rho(!cM * e, 0), &Cp(!cCp * e, 0),
                                            sat, dT_G, da_G, dP_G);
              /* limite thermique */
              double Tk = temp(e, k), Tl = temp(e, l), p = press(e, 0), Ts = sat.Tsat(p), dP_Ts = sat.dP_Tsat(p), //temperature de chaque cote + Tsat + derivees
                     phi = hi(k, l) * (Tk - Ts) + hi(l, k) * (Tl - Ts) + (q_pi ? (*q_pi)(e, k, l) : 0), L = (phi < 0 ? h(e, l) : sat.Hvs(p)) - (phi > 0 ? h(e, k) : sat.Hls(p));
              if ((is_therm = !correlation_G || std::fabs(G) > std::fabs(phi / L))) G = phi / L;
              /* enthalpies des phases (dans le sens correspondant au mode choisi pour G) */

              /* G est-il limite par l'evanescence cote k ou l ? */
              int n_lim = G > 0 ? k : l, sgn = G > 0 ? 1 : -1; //phase sortante
              double Glim = sec_m(e, n_lim) / vol + p_ar(e, n_lim) / dt; //changement de phase max acceptable par cette phase
              if (std::fabs(G) < Glim) n_lim = -1;       //G ne rend pas la phase evanescente -> pas de limitation (n_lim = -2)
              else G = (G > 0 ? 1 : -1) * Glim;//la phase serait evanescente a cause de G -> on le bloque a G_lim (n_lim = k / l)

              double hk = G > 0 ? h(e, k) : sat.Hls(p), dTk_hk = G > 0 && dT_h ? (*dT_h)(e, k) : 0, dP_hk = G > 0 ? (dP_h ? (*dP_h)(e, k) : 0) : sat.dP_Hls(p),
                       hl = G < 0 ? h(e, l) : sat.Hvs(p), dTl_hl = G < 0 && dT_h ? (*dT_h)(e, l) : 0, dP_hl = G < 0 ? (dP_h ? (*dP_h)(e, l) : 0) : sat.dP_Hvs(p);
              if (n_lim < 0 && is_therm) /* derivees de G en limite thermique */
              {
                  double dP_phi = dP_hi(k, l) * (Tk - Ts) + dP_hi(l, k) * (Tl - Ts) - (hi(k, l) + hi(l, k)) * dP_Ts, dTk_L = -dTk_hk, dTl_L = dTl_hl, dP_L = dP_hl - dP_hk;
                  dP_G = (dP_phi - G * dP_L) / L;
                  for (n = 0; n < N; n++) dT_G(n) = ((n == k) * (hi(k, l) - G * dTk_L) + (n == l) * (hi(l, k) - G * dTl_L) + dT_hi(k, l, n) * (Tk - Ts) + dT_hi(l, k, n) * (Tl - Ts)) / L;
                  for (n = 0; n < N; n++) da_G(n) = (da_hi(k, l, n) * (Tk - Ts) + da_hi(l, k, n) * (Tl - Ts)) / L;
                }


              if (sub_type(Masse_Multiphase, equation())) //eq de masse -> changement de phase
                {
                  for (i = 0; i < 2; i++) secmem(e, i ? l : k) -= vol * (i ? -1 : 1) * G;
                  if (n_lim < 0) /* G par limite thermique */
                    {
                      if (Ma) for (i = 0; i < 2; i++) for (n = 0; n < N; n++)  //derivees en alpha
                            (*Ma)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * da_G(n);
                      if (Mt) for (i = 0; i < 2; i++) for (n = 0; n < N; n++)  //derivees en T
                            (*Mt)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * dT_G(n);
                      if (Mp) for (i = 0; i < 2; i++) //derivees en p
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
                  for (i = 0; i < 2; i++) secmem(e, i ? l : k) -= vol * (i ? -1 : 1) * (s_c * hi(n_c, n_d) * (Tc - Ts) + (c || !q_pi ? 0 : (*q_pi)(e, k, l)) + G * hc);
                  /* derivees (y compris celles en G, sauf dans le cas limite)*/
                  if (Ma) for (i = 0; i < 2; i++) for (n = 0; n < N; n++) //derivees en alpha
                        (*Ma)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * (s_c * da_hi(n_c, n_d, n) * (Tc - Ts) + (n_lim < 0) * da_G(n) * hc);
                  if (Mt) for (i = 0; i < 2; i++) for (n = 0; n < N; n++) //derivees en T
                        (*Mt)(N * e + (i ? l : k), N * e + n) += vol * (i ? -1 : 1) * (s_c * (dT_hi(n_c, n_d, n) * (Tc - Ts) + (n == n_c) * hi(n_c, n_d)) + (n_lim < 0) * dT_G(n) * hc + G * (n == n_c) * dT_hc);
                  if (Mp) for (i = 0; i < 2; i++) //derivees en p
                      (*Mp)(N * e + (i ? l : k), e) += vol * (i ? -1 : 1) * (s_c * (dP_hi(n_c, n_d) * (Tc - Ts) - hi(n_c, n_d) * dP_Ts) + (n_lim < 0) * dP_G * hc + G * dP_hc);
                  if (n_lim >= 0) for (auto &s_d : vec_m) /* derivees de G dans le cas evanescent */
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
              if (Mt) for (i = 0; i < 2; i++) //juste des derivees en T
                  {
                    (*Mt)(N * e + (i ? l : k), N * e + k) += vol * (i ? -1 : 1) * hm;
                    (*Mt)(N * e + (i ? l : k), N * e + l) += vol * (i ? 1 : -1) * hm;
                  }
            }
    }
}
