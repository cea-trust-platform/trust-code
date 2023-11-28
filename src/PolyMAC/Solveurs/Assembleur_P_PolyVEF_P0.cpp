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
#include <Assembleur_P_PolyVEF_P0.h>
#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Neumann_sortie_libre.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Masse_PolyVEF_P0_Face.h>
#include <Domaine_PolyVEF_P0.h>
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

Implemente_instanciable(Assembleur_P_PolyVEF_P0, "Assembleur_P_PolyVEF_P0", Assembleur_P_PolyMAC_P0P1NC);

Sortie& Assembleur_P_PolyVEF_P0::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Assembleur_P_PolyVEF_P0::readOn(Entree& s) { return Assembleur_P_PolyMAC_P0P1NC::readOn(s); }

int  Assembleur_P_PolyVEF_P0::assembler_mat(Matrice& la_matrice,const DoubleVect& diag,int incr_pression,int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur());

  const Domaine_PolyVEF_P0& dom = ref_cast(Domaine_PolyVEF_P0, le_dom_PolyMAC.valeur());
  const Op_Grad_PolyVEF_P0_Face& grad = ref_cast(Op_Grad_PolyVEF_P0_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur());
  grad.update_grad();
  const DoubleTab& fgrad_c = grad.fgrad_c, &nf = dom.face_normales();
  const IntTab& f_e = dom.face_voisins(), &fgrad_d = grad.fgrad_d, &fgrad_e = grad.fgrad_e, &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue()).fcl();
  const DoubleVect& pf = equation().milieu().porosite_face();
  int i, j, e, eb, f, fb, ne = dom.nb_elem(), ne_tot = dom.nb_elem_tot(), nfb_tot = dom.nb_faces_bord_tot(), d, D = dimension;

  //en l'absence de CLs en pression, on ajoute P(0) = 0 sur le process 0
  has_P_ref=0;
  for (int n_bord=0; n_bord<le_dom_PolyMAC->nb_front_Cl(); n_bord++)
    if (sub_type(Neumann_sortie_libre, le_dom_Cl_PolyMAC->les_conditions_limites(n_bord).valeur()) )
      has_P_ref=1;

  /* 1. stencil de la matrice en pression : seulement au premier passage */
  if (!stencil_done)
    {
      IntTrav stencil(0, 2);
      for (f = 0; f < dom.nb_faces(); f++)
        for (i = 0; i < 2; i++)
          if ((e = f_e(f, i)) >= 0  ? e < ne : fcl(f, 0) != 1)
            {
              for (j = fgrad_d(f); j < fgrad_d(f + 1); j++)
                if ((eb = fgrad_e(j)) < ne_tot || fcl(fb = eb - ne_tot, 0) != 1)
                  stencil.append_line(e >= 0 ? e : ne_tot + f, eb < ne_tot ? eb : ne_tot + dom.fbord(fb));
            }
          else if (fcl(f, 0) == 1) stencil.append_line(ne_tot + f, ne_tot + f);

      tableau_trier_retirer_doublons(stencil);
      Matrix_tools::allocate_morse_matrix(ne_tot + nfb_tot, ne_tot + nfb_tot, stencil, mat);
      mat.sort_stencil();
      tab1.ref_array(mat.get_set_tab1()), tab2.ref_array(mat.get_set_tab2());
      stencil_done = 1;
    }
  else //sinon, on recycle
    {
      mat.get_set_tab1().ref_array(tab1);
      mat.get_set_tab2().ref_array(tab2);
      mat.get_set_coeff().resize(tab2.size());
      mat.set_nb_columns(ne_tot);
    }

  /* 2. remplissage des coefficients : style Op_Diff_PolyMAC_Elem */
  for (f = 0; f < dom.nb_faces(); f++)
    for (i = 0; i < 2; i++)
      if ((e = f_e(f, i)) >= 0 ? e < ne : fcl(f, 0) != 1)
        {
          for (j = fgrad_d(f); j < fgrad_d(f + 1); j++)
            if ((eb = fgrad_e(j)) < ne_tot || fcl(fb = eb - ne_tot, 0) != 1)
              for (d = 0; d < D; d++)
                mat(e >= 0 ? e : ne_tot + f, eb < ne_tot ? eb : ne_tot + dom.fbord(fb)) += (i ? 1 : -1) * pf(f) * nf(f, d) * fgrad_c(j, d, 0);
        }
      else if (fcl(f, 0) == 1) mat(ne_tot + f, ne_tot + f) = 1;

  if (!has_P_ref && !Process::me()) mat(0, 0) *= 2;

  return 1;
}

/* equation sum_k alpha_k = 1 en Pb_Multiphase */
void Assembleur_P_PolyVEF_P0::dimensionner_continuite(matrices_t matrices, int aux_only) const
{
  const IntTab& fcl = ref_cast(Champ_Face_PolyMAC, mon_equation->inconnue()).fcl();
  int e, f, d, D = dimension, n, N = ref_cast(Pb_Multiphase, equation().probleme()).nb_phases(), m, M = equation().get_champ("pression").valeurs().line_size(),
               ne_tot = le_dom_PolyMAC->nb_elem_tot(), nfb_tot = le_dom_PolyMAC->nb_faces_bord_tot();
  IntTrav sten_a(0, 2), sten_p(0, 2), sten_v(0, 2);
  /* elements : sum alpha = 1 */
  if (!aux_only)
    for (e = 0; e < le_dom_PolyMAC->nb_elem(); e++)
      for (n = 0; n < N; n++) sten_a.append_line(e, N * e + n);

  /* faces de bord : continuite du gradient si interne, p = p_f si Neumann, sum_k alpha_k v_k = sum_k alpha_k v_k,imp si Dirichlet */
  for (f = 0; f < le_dom_PolyMAC->premiere_face_int(); f++)
    if (fcl(f, 0) == 1)
      for (m = 0; m < M; m++) sten_p.append_line(M * (!aux_only * ne_tot + f) + m, M * (ne_tot + f) + m); /* Neumann -> p = p_imp */
    else for (d = 0; d < D; d++)
        for (n = 0, m = 0; n < N; n++, m += (M > 1))
          sten_v.append_line(M * (!aux_only * ne_tot + f) + m, N * (D * f + d) + n); /* Dirichlet -> flux tot = flux tot imp */

  tableau_trier_retirer_doublons(sten_v), tableau_trier_retirer_doublons(sten_p);
  if (!aux_only) Matrix_tools::allocate_morse_matrix(ne_tot + nfb_tot, N * ne_tot, sten_a, *matrices.at("alpha"));
  Matrix_tools::allocate_morse_matrix(M * (!aux_only * ne_tot + nfb_tot), M * (ne_tot + nfb_tot), sten_p, *matrices.at("pression"));
  Matrix_tools::allocate_morse_matrix(M * (!aux_only * ne_tot + nfb_tot), equation().inconnue().valeurs().size_totale(), sten_v, *matrices.at("vitesse"));
}

void Assembleur_P_PolyVEF_P0::assembler_continuite(matrices_t matrices, DoubleTab& secmem, int aux_only) const
{
  const Domaine_PolyMAC& dom = le_dom_PolyMAC.valeur();
  const Pb_Multiphase* pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const DoubleTab *alpha = pbm ? &pbm->equation_masse().inconnue().valeurs() : nullptr, &press = equation().probleme().get_champ("pression").valeurs(),
                   &vit = equation().inconnue().valeurs(), *alpha_rho = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr, &nf = dom.face_normales();
  const IntTab& fcl = ref_cast(Champ_Face_PolyMAC, mon_equation->inconnue()).fcl(), &f_e = dom.face_voisins();
  const DoubleVect& ve = dom.volumes(), &pe = equation().milieu().porosite_elem();
  int e, f, d, D = dimension, n, N = vit.line_size() / D, m, M = press.line_size(), ne_tot = dom.nb_elem_tot();
  Matrice_Morse *mat_a = alpha ? matrices.at("alpha") : nullptr, &mat_p = *matrices.at("pression"), &mat_v = *matrices.at("vitesse");
  DoubleTrav fac(N);
  double ar_tot;
  secmem = 0, fac = 1;

  /* equations sum alpha_k = 1 */
  /* second membre : on multiplie par porosite * volume pour que le systeme en P soit symetrique en cartesien */
  if (!aux_only)
    for (e = 0; e < dom.nb_elem(); e++)
      for (secmem(e) = -pe(e) * ve(e), n = 0; n < N; n++) secmem(e) += pe(e) * ve(e) * (*alpha)(e, n);
  /* matrice */
  if (!aux_only)
    for (e = 0; e < dom.nb_elem(); e++)
      for (n = 0; n < N; n++) (*mat_a)(e, N * e + n) = -pe(e) * ve(e);

  /* equations sur les p_f : continuite du gradient si interne, p = p_f si Neumann, sum_k alpha_k v_k = sum_k alpha_k v_k,imp si Dirichlet */
  for (mat_p.get_set_coeff() = 0, mat_v.get_set_coeff() = 0, f = 0; f < dom.premiere_face_int(); f++)
    if (fcl(f, 0) == 1) //Neumann -> egalites p_f = p_imp
      {
        for (m = 0; m < M; m++) secmem(M * (!aux_only * ne_tot + f) + m) = ref_cast(Neumann, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), m) - press(ne_tot + f, m);
        for (m = 0; m < M; m++) mat_p(M * (!aux_only * ne_tot + f) + m, M * (ne_tot + f) + m) = 1;
      }
    else  //Dirichlet -> egalite flux_tot_imp - flux_tot = 0
      {
        if (M == 1) //une pression, plusieurs vitesses -> on ne peut imposer qu'une ponderation : on choisit celle ocrrespondant aux flux de masse total
          {
            for (e = f_e(f, 0), ar_tot = 0, n = 0; n < N; n++) ar_tot += (*alpha_rho)(e, n);
            for (n = 0; n < N; n++) fac(n) = (*alpha_rho)(e, n) / ar_tot;
          }
        else if (M != N) abort(); //sinon, il faut autant de pressions que de vitesses

        for (d = 0; d < D; d++)
          for (n = 0, m = 0; n < N; n++, m += (M > 1))
            secmem(!aux_only * ne_tot + f, m) += fac(n) * nf(f, d) * (vit(f, N * d + n) - (fcl(f, 0) == 3 ? ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n) : 0));
        for (d = 0; d < D; d++)
          for (n = 0, m = 0; n < N; n++, m += (M > 1))
            mat_v(M * (!aux_only * ne_tot + f) + m, N * (D * f + d) + n) -= fac(n) * nf(f, d);
      }
}
