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

#include <Champ_front_instationnaire_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Assembleur_P_PolyVEF_P0P1.h>
#include <Op_Grad_PolyVEF_P0P1_Face.h>
#include <Op_Div_PolyVEF_P0P1.h>
#include <Neumann_sortie_libre.h>
#include <Champ_Face_PolyVEF.h>
#include <Masse_PolyVEF_Face.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Matrice_Diagonale.h>
#include <Navier_Stokes_std.h>
#include <Matrice_Morse_Sym.h>
#include <Schema_Temps_base.h>
#include <Matrice_Bloc_Sym.h>
#include <Static_Int_Lists.h>
#include <Operateur_Grad.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Dirichlet.h>
#include <Debog.h>
#include <Piso.h>

Implemente_instanciable(Assembleur_P_PolyVEF_P0P1, "Assembleur_P_PolyVEF_P0P1", Assembleur_P_PolyMAC_P0);

Sortie& Assembleur_P_PolyVEF_P0P1::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Assembleur_P_PolyVEF_P0P1::readOn(Entree& s) { return Assembleur_P_PolyMAC_P0::readOn(s); }

const IntTab& Assembleur_P_PolyVEF_P0P1::ps_ref() const
{
  if (ps_ref_.get_md_vector().non_nul()) return ps_ref_;
  const Domaine_PolyVEF& dom = ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue());
  const IntTab& f_s = dom.face_sommets(), &fcl = ch.fcl();
  const DoubleTab& xp = dom.xp();
  const Static_Int_Lists& s_e = dom.som_elem();

  dom.domaine().creer_tableau_sommets(ps_ref_);
  for (int f = 0; f < dom.nb_faces_tot(); f++)
    if (fcl(f, 0) < 2)
      for (int i = 0, s; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
        ps_ref_(s) = -1;
  ps_ref_.echange_espace_virtuel();
  /* pour les pressions non utilisees, de quel sommet doit-on prendre la pression ? */
  for (int s = 0; s < dom.nb_som_tot(); s++)
    if (ps_ref_(s) == 0)
      {
        std::array<double, 3> xpmin = { 1e8, 1e8, 1e8 };
        for (int i = 0; i < s_e.get_list_size(s); i++)
          {
            int e = s_e(s, i);
            std::array<double, 3> xpb = { xp(e, 0), xp(e, 1), dimension < 3 ? 0 : xp(e, 2) };
            if (xpb < xpmin) ps_ref_(s) = e, xpmin = xpb;
          }
      }
  return ps_ref_;
}

int  Assembleur_P_PolyVEF_P0P1::assembler_mat(Matrice& la_matrice,const DoubleVect& diag,int incr_pression,int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur()), mat_div_v, mat_div_p, mat_grad;
  const Navier_Stokes_std& eq = ref_cast(Navier_Stokes_std, equation());
  const Op_Div_PolyVEF_P0P1& div = ref_cast(Op_Div_PolyVEF_P0P1, eq.operateur_divergence().valeur());
  const Op_Grad_PolyVEF_P0P1_Face& grad = ref_cast(Op_Grad_PolyVEF_P0P1_Face, eq.operateur_gradient().valeur());

  const Domaine_PolyVEF_P0P1& dom = ref_cast(Domaine_PolyVEF_P0P1, le_dom_PolyMAC.valeur());
  int ne_tot = dom.nb_elem_tot(), nf_tot = dom.nb_faces_tot(), ns_tot = dom.nb_som_tot(), d, D = dimension, f, i;
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces();

  /* 1. stencil : seulement au premier passage */
  if (!stencil_done)
    {
      //dimensionner_blocs de div et grad, puis produit
      div.dimensionner_blocs({ { "vitesse", &mat_div_v }, { "pression", &mat_div_p } });
      grad.dimensionner_blocs_ext({ { "pression", &mat_grad } }, 1); //avec les faces virtuelles
      mat.affecte_prod(mat_div_v, mat_grad), mat.set_nb_columns(ne_tot + ns_tot);
      mat += mat_div_p;
      //stockage des stencils : pour mat, on ne peut stocker que la taille
      tab1.resize(1), tab1(0) = mat.get_set_tab1().size(), tab2.resize(1), tab2(0) = mat.get_set_tab2().size();
      div_v_tab1.ref_array(mat_div_v.get_set_tab1()), div_v_tab2.ref_array(mat_div_v.get_set_tab2());
      div_p_tab1.ref_array(mat_div_p.get_set_tab1()), div_p_tab2.ref_array(mat_div_p.get_set_tab2());
      grad_tab1.ref_array(mat_grad.get_set_tab1()), grad_tab2.ref_array(mat_grad.get_set_tab2());
      stencil_done = 1;
    }
  else //sinon, on recycle
    {
      mat.get_set_tab1().resize(tab1(0)), mat.get_set_tab2().resize(tab2(0)), mat.get_set_coeff().resize(tab2(0)), mat.set_nb_columns(ne_tot + ns_tot);
      mat_div_v.get_set_tab1().ref_array(div_v_tab1), mat_div_v.get_set_tab2().ref_array(div_v_tab2), mat_div_v.get_set_coeff().resize(div_v_tab2.size()), mat_div_v.set_nb_columns(D * nf_tot);
      mat_div_p.get_set_tab1().ref_array(div_p_tab1), mat_div_p.get_set_tab2().ref_array(div_p_tab2), mat_div_p.get_set_coeff().resize(div_p_tab2.size()), mat_div_p.set_nb_columns(ne_tot + ns_tot);
      mat_grad.get_set_tab1().ref_array(grad_tab1), mat_grad.get_set_tab2().ref_array(grad_tab2), mat_grad.get_set_coeff().resize(grad_tab2.size()), mat_grad.set_nb_columns(ne_tot + ns_tot);
    }

  /* gradient : par op_grad, puis produit par -1 / diag */
  DoubleTrav sec_grad(nf_tot, D), sec_div(ne_tot + ns_tot, 1), inv_diag(D * nf_tot);
  grad.ajouter_blocs_ext({ { "pression", &mat_grad } }, sec_grad, 1); //avec lignes virtuelles
  div.ajouter_blocs({ { "vitesse", &mat_div_v }, { "pression", &mat_div_p } }, sec_div); //avec lignes virtuelles
  for (f = 0, i = 0; f < nf_tot; f++)
    for (d = 0; d < D; d++, i++) inv_diag(i) = 1. / (diag.size() ? diag(i) : pf(f) * vf(f));
  mat_grad.diagmulmat(inv_diag);
  mat.affecte_prod(mat_div_v, mat_grad);
  mat_div_p *= -1;
  mat += mat_div_p; //partie (p, p) (pressions imposees) de la divergence

  //en l'absence de CLs en pression, on ajoute P(0) = 0 sur le process premier process dont le domaine est non vide
  has_P_ref=0;
  for (int n_bord=0; n_bord<le_dom_PolyMAC->nb_front_Cl(); n_bord++)
    if (sub_type(Neumann_sortie_libre, le_dom_Cl_PolyMAC->les_conditions_limites(n_bord).valeur()) )
      has_P_ref=1;

  return 1;
}

void Assembleur_P_PolyVEF_P0P1::dimensionner_continuite(matrices_t matrices, int aux_only) const
{
  const Domaine_PolyVEF_P0P1& dom = ref_cast(Domaine_PolyVEF_P0P1, le_dom_PolyMAC.valeur());
  int i, e, f, s, d, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D, m, M = equation().get_champ("pression").valeurs().line_size(),
                     ne_tot = dom.nb_elem_tot(), ns_tot = dom.domaine().nb_som_tot();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, mon_equation->inconnue());
  const IntTab& fcl = ch.fcl(), &scl_d = ch.scl_d(1), &f_s = dom.face_sommets(), &ps_r = ps_ref();
  IntTrav sten_a(0, 2), sten_p(0, 2), sten_v(0, 2);
  DoubleTrav w2;
  /* equations sum alpha_k = 1 */
  if (!aux_only)
    for (e = 0; e < dom.nb_elem(); e++)
      for (n = 0; n < N; n++) sten_a.append_line(e, N * e + n);
  /* equations aux sommets */
  for (f = 0; f < dom.nb_faces_tot(); f++)
    if (fcl(f, 0) < 2) /* sten_v : divergence "classique" */
      for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
        for (d = 0; s < dom.domaine().nb_som() && scl_d(s) == scl_d(s + 1) && d < D; d++) //sommets a pression non imposee
          for (n = 0; n < N; n++)
            sten_v.append_line(!aux_only * ne_tot + s, N * (D * f + d) + n);

  /* sten_p : diagonale du vide + egalites p_s = p_e pour les pressions inutilisees */
  for (e = 0; e < dom.nb_elem(); e++)
    for (m = 0; m < M; m++)
      sten_p.append_line(M * e, M * e);
  for (s = 0; s < dom.nb_som_tot() ; s++)
    for (n = 0, m = 0; n < N; n++, m += (M > 1))
      {
        if (s < dom.nb_som()) sten_p.append_line(M * (!aux_only * ne_tot + s) + m, M * (!aux_only * ne_tot + s) + m);
        if ((e = ps_r(s)) >= 0 && !aux_only)
          {
            if (s < dom.nb_som())  sten_p.append_line(M * (ne_tot + s) + m, M * e + m);
            if (e < dom.nb_elem()) sten_p.append_line(M * e + m, M * (ne_tot + s) + m);
          }
      }

  tableau_trier_retirer_doublons(sten_v), tableau_trier_retirer_doublons(sten_p);
  if (!aux_only) Matrix_tools::allocate_morse_matrix(ne_tot + ns_tot, N * ne_tot, sten_a, *matrices.at("alpha"));
  Matrix_tools::allocate_morse_matrix(M * (!aux_only * ne_tot + ns_tot), M * (ne_tot + ns_tot), sten_p, *matrices.at("pression"));
  Matrix_tools::allocate_morse_matrix(M * (!aux_only * ne_tot + ns_tot), equation().inconnue().valeurs().size_totale(), sten_v, *matrices.at("vitesse"));
}

void Assembleur_P_PolyVEF_P0P1::assembler_continuite(matrices_t matrices, DoubleTab& secmem, int aux_only) const
{
  const Domaine_PolyVEF_P0P1& dom = ref_cast(Domaine_PolyVEF_P0P1, le_dom_PolyMAC.valeur());
  const Pb_Multiphase* pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, mon_equation->inconnue());
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const DoubleTab *alpha = pbm ? &pbm->equation_masse().inconnue().valeurs() : nullptr, &press = equation().probleme().get_champ("pression").valeurs(),
                   &vit = equation().inconnue().valeurs(), *alpha_rho = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr,
                    ar_bord = pbm ? pbm->equation_masse().champ_conserve().valeur_aux_bords() : DoubleTab(), &xp = dom.xp(), &xv = dom.xv(), &xs = dom.domaine().coord_sommets(),
                    &nf = dom.face_normales();
  const IntTab& fcl = ch.fcl(), &scl_d = ch.scl_d(1), &scl_c = ch.scl_c(1), &f_e = dom.face_voisins(), &f_s = dom.face_sommets(), &ps_r = ps_ref();
  const DoubleVect& ve = dom.volumes(), &pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face();
  int i, j, e, f, s, sb, sc, d, D = dimension, n, N = vit.line_size() / D, m, M = press.line_size(), ne_tot = dom.nb_elem_tot();
  Matrice_Morse *mat_a = alpha ? matrices.at("alpha") : nullptr, &mat_p = *matrices.at("pression"), &mat_v = *matrices.at("vitesse");
  DoubleTrav w2, a_r(N), v(2, D);
  double ar_tot = 1, acc, dt = equation().schema_temps().pas_de_temps(), fac, xa[3], vz[3] = { 0, 0, 1 };
  secmem = 0, a_r = 1;

  /* equations sum alpha_k = 1 : on multiplie par porosite * volume * rho_m / dt pour que l'equation ressemble a delta p = 0 */
  if (!aux_only)
    for (e = 0; e < dom.nb_elem(); e++)
      {
        if (alpha_rho)
          for (ar_tot = 0, n = 0; n < N; n++)
            ar_tot += (*alpha_rho)(e, n);
        for (fac = ve(e) * pe(e) * ar_tot / dt, secmem(e) = -fac, n = 0; n < N; n++) secmem(e) += fac * (*alpha)(e, n);
        for (n = 0; n < N; n++) (*mat_a)(e, N * e + n) = -fac;
      }

  /* equations aux sommets : comme dans Op_Div_PolyVEF_P0P1, mais avec produit par alpha * rho */
  for (f = 0; f < dom.nb_faces_tot(); f++)
    {
      //alpha * rho amont
      if (alpha_rho)
        for (n = 0; n < N; n++)
          {
            for (acc = 0, d = 0; d < D; d++)
              acc += nf(f, d) * vit(f, N * d + n);
            for (a_r(n) = 0, i = 0; i < 2; i++)
              a_r(n) += (acc * (i ? -1 : 1) > 0 ? 1 : acc ? 0 : 0.5) * (f_e(f, i) >= 0 ? (*alpha_rho)(f_e(f, i), n) : ar_bord(f, n));
          }
      //divergence aux sommets
      for (i = 0; i < (D < 3 ? 1 : f_s.dimension(1)) && (s = f_s(f, i)) >= 0; i++)
        {
          sb = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0);
          for (d = 0; d < D; d++) xa[d] = (xs(s, d) + xs(sb, d)) / 2;
          for (v = 0, j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++) // surface * normale de s vers sb
            {
              auto vb = dom.cross(D, 3, &xv(f, 0), D < 3 ? vz : xa, &xp(e, 0), D < 3 ? nullptr : &xp(e, 0));
              for (fac = (dom.dot(&xs(sb, 0), &vb[0], &xs(s, 0)) > 0 ? 1. : -1.) / (D - 1), d = 0; d < D; d++)
                v(0, d) += fac * vb[d], v(1, d) -= fac * vb[d];
            }
          //bord
          if (f_e(f, 1) < 0)
            {
              auto vb = dom.cross(D, 3, &xv(f, 0), D < 3 ? vz : xa, &xs(s, 0), D < 3 ? nullptr : &xs(s, 0));
              for (fac = (dom.dot(&nf(f, 0), &vb[0]) > 0 ? 1. : -1.) / (D - 1), d = 0; d < D; d++)
                v(0, d) += fac * vb[d], v(1, d) += fac * vb[d];
            }
          //contributions
          for (j = 0; j < 2; j++)
            if ((sc = j ? sb : s) < dom.nb_som() && ps_r(sc) < 0 && scl_d(sc) == scl_d(sc + 1))
              for (d = 0; d < D; d++)
                for (n = 0, m = 0; n < N; n++, m += (M > 1))
                  {
                    secmem(!aux_only * ne_tot + sc, m) -= (D - 1) * v(j, d) * pf(f) * vit(f, N * d + n) * a_r(n) / D;
                    if (fcl(f, 0) < 2) mat_v(M * (!aux_only * ne_tot + sc) + m, N * (D * f + d) + n) += (D - 1) * v(j, d) * pf(f) * a_r(n) / D;
                  }
        }
    }

  //equations restantes aux sommets : pressions imposees et inutiles
  for (s = 0; s < dom.nb_som_tot(); s++)
    if (s < dom.nb_som() && scl_d(s) < scl_d(s + 1)) /* lignes "pression aux sommets imposee" */
      for (m = 0; m < M; m++)
        {
          secmem(!aux_only * ne_tot + s, m) -= press(!aux_only * ne_tot + s, m) - ref_cast(Neumann, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), m);
          mat_p(M * (!aux_only * ne_tot + s) + m, M * (!aux_only * ne_tot + s) + m)++;
        }
    else if ((e = ps_r(s)) >= 0 && !aux_only) /* force de rappel avec les sommets voisins pour les pressions inutilisees */
      for (m = 0; m < M; m++)
        {
          if (s < dom.nb_som())  secmem(ne_tot + s, m) += press(e, m) - press(ne_tot + s , m);
          if (e < dom.nb_elem()) secmem(e         , m) += press(ne_tot + s , m) - press(e, m);
          if (s < dom.nb_som())  mat_p(M * (ne_tot + s) + m, M * (ne_tot + s) + m)++;
          if (s < dom.nb_som())  mat_p(M * (ne_tot + s) + m, M * e + m)--;
          if (e < dom.nb_elem()) mat_p(M * e + m, M * e + m)++;
          if (e < dom.nb_elem()) mat_p(M * e + m, M * (ne_tot + s) + m)--;
        }
}

int Assembleur_P_PolyVEF_P0P1::modifier_solution(DoubleTab& pression)
{
  if(has_P_ref) return 1;
  double pe_min = DBL_MAX, ps_min = DBL_MAX;
  const Domaine_PolyVEF& dom = ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const IntVect& ps_r = ps_ref();
  int e, s, ne_tot = dom.nb_elem_tot();
  for (e = 0; e < dom.nb_elem(); e++) pe_min = std::min(pe_min, pression(e));
  for (s = 0; s < dom.nb_som(); s++)
    if (ps_r(s) < 0)
      ps_min = std::min(ps_min, pression(ne_tot + s));
  pe_min = Process::mp_min(pe_min), ps_min = Process::mp_min(ps_min);
  for (e = 0; e < dom.nb_elem_tot(); e++) pression(e) -= pe_min;
  for (s = 0; s < dom.nb_som_tot(); s++)
    pression(ne_tot + s) -= (ps_r(s) >= 0 ? pe_min : ps_min);
  return 1;
}

void Assembleur_P_PolyVEF_P0P1::modifier_secmem_pour_incr_p(const DoubleTab& press, const double fac, DoubleTab& secmem) const
{
  const Domaine_PolyMAC& zone = ref_cast(Domaine_PolyMAC, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, mon_equation->inconnue());
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const IntTab& scl_d = ch.scl_d(1), &scl_c = ch.scl_c(1);
  int s, ne_tot = zone.nb_elem_tot(), m, M = equation().probleme().get_champ("pression").valeurs().line_size();
  for (s = 0; s < zone.nb_som(); s++)
    if (scl_d(s) < scl_d(s + 1)) /* pression imposee */
      for (m = 0; m < M; m++)
        secmem(ne_tot + s, m) = (ref_cast(Neumann_sortie_libre, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), m) - press(ne_tot + s, m)) / fac;
}
