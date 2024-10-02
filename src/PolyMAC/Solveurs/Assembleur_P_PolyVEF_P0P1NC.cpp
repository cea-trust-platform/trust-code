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
#include <Assembleur_P_PolyVEF_P0P1NC.h>
#include <Champ_Face_PolyVEF.h>
#include <Domaine_PolyVEF.h>
#include <Neumann_sortie_libre.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Diagonale.h>
#include <Static_Int_Lists.h>
#include <Matrice_Bloc_Sym.h>
#include <Operateur_Grad.h>
#include <TRUSTTab_parts.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <Dirichlet.h>
#include <Debog.h>

extern Stat_Counter_Id assemblage_sys_counter_;

Implemente_instanciable(Assembleur_P_PolyVEF_P0P1NC, "Assembleur_P_PolyVEF_P0P1NC", Assembleur_P_PolyMAC);

Sortie& Assembleur_P_PolyVEF_P0P1NC::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Assembleur_P_PolyVEF_P0P1NC::readOn(Entree& s) { return Assembleur_base::readOn(s); }


int  Assembleur_P_PolyVEF_P0P1NC::assembler_mat(Matrice& la_matrice,const DoubleVect& diag,int incr_pression,int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  Cerr << "Assemblage de la matrice de pression ... " ;
  statistiques().begin_count(assemblage_sys_counter_);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur());

  const Domaine_PolyVEF_P0P1NC& domaine = ref_cast(Domaine_PolyVEF_P0P1NC, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, mon_equation->inconnue());
  const IntTab& e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  int i, j, e, f, fb, ne = domaine.nb_elem(), ne_tot = domaine.nb_elem_tot(), nf = domaine.nb_faces(), nf_tot = domaine.nb_faces_tot();

  DoubleTrav w2; //matrice W2 (de Domaine_PolyVEF_P0P1NC) par element

  /* 1. stencil de la matrice en pression : seulement au premier passage */
  if (!stencil_done) /* premier passage: calcul */
    {
      IntTrav stencil(0, 2);
      for (e = 0; e < ne; e++)
        for (stencil.append_line(e, e), i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) /* blocs "elem-elem" et "elem-face" */
          stencil.append_line(e, ne_tot + f); //toutes les faces (sauf bord de Neumann)
      for (e = 0; e < ne_tot; e++)
        for (domaine.W2(nullptr, e, w2), i = 0; i < w2.dimension(1); i++) /* blocs "face-elem" et "face-face" */
          if (fcl(f = e_f(e, i), 0) == 1 && f < nf) stencil.append_line(ne_tot + f, ne_tot + f); //Neumann : ligne "dpf = 0"
          else if (f < nf)
            for (stencil.append_line(ne_tot + f, e), j = 0; j < w2.dimension(1); j++) /* sinon : ligne sum w2_{ff'} (pf' - pe) */
              if (w2(i, j, 0)) stencil.append_line(ne_tot + f, ne_tot + e_f(e, j));

      tableau_trier_retirer_doublons(stencil);
      Matrix_tools::allocate_morse_matrix(ne_tot + nf_tot, ne_tot + nf_tot, stencil, mat);
      tab1.ref_array(mat.get_set_tab1()), tab2.ref_array(mat.get_set_tab2());
      stencil_done = 1;
    }
  else /* passages suivants : recyclage */
    {
      mat.get_set_tab1().ref_array(tab1);
      mat.get_set_tab2().ref_array(tab2);
      mat.get_set_coeff().resize(tab2.size()), mat.get_set_coeff() = 0;
      mat.set_nb_columns(ne_tot + nf_tot);
    }

  /* 2. coefficients */
  for (e = 0; e < ne_tot; e++)
    {
      domaine.W2(nullptr, e, w2); //calcul de W2
      double m_ee = 0, m_fe, m_ef, coeff; //coefficients (elem, elem), (elem, face) et (face, elem)
      for (i = 0; i < w2.dimension(0); i++, m_ee += m_ef)
        {
          f = e_f(e, i), coeff = diag.size_totale() ? pf(f) * vf(f) / diag(f) : 1;
          for (m_ef = 0, m_fe = 0, j = 0; j < w2.dimension(1); j++)
            if (w2(i, j, 0))
              {
                fb = e_f(e, j);
                if (f < domaine.nb_faces() && fcl(f, 0) != 1) mat(ne_tot + f, ne_tot + fb) += coeff * pe(e) * w2(i, j, 0); //interne ou Dirichlet
                else if (f < domaine.nb_faces() && i == j) mat(ne_tot + f, ne_tot + fb) = 1; //f Neumann : ligne dpf = 0
                m_ef += coeff * pe(e) * w2(i, j, 0),  m_fe += coeff * pe(e) * w2(i, j, 0); //accumulation dans m_ef, m_fe
              }
          if (e < domaine.nb_elem()) mat(e, ne_tot + f) -= m_ef;
          if (f < domaine.nb_faces() && fcl(f, 0) != 1) mat(ne_tot + f, e) -= m_fe; //si f non Neumann : coef (face, elem)
        }
      if (e < domaine.nb_elem()) mat(e, e) += m_ee; //coeff (elem, elem)
    }

  //en l'absence de CLs en pression, on ajoute P(0) = 0 sur le process 0
  has_P_ref=0;
  for (int n_bord=0; n_bord<le_dom_PolyMAC->nb_front_Cl(); n_bord++)
    if (sub_type(Neumann_sortie_libre, le_dom_Cl_PolyMAC->les_conditions_limites(n_bord).valeur()) )
      has_P_ref=1;
  if (!has_P_ref && !Process::me()) mat(0, 0) *= 2;

  statistiques().end_count(assemblage_sys_counter_);
  Cerr << statistiques().last_time(assemblage_sys_counter_) << " s" << finl;
  return 1;
}

/* equations sum_k alpha_k = 1, [grad p]_{fe} = [grad p]_{fe'} en Pb_Multiphase */
void Assembleur_P_PolyVEF_P0P1NC::dimensionner_continuite(matrices_t matrices, int aux_only) const
{
  const Domaine_PolyVEF_P0P1NC& domaine = ref_cast(Domaine_PolyVEF_P0P1NC, le_dom_PolyMAC.valeur());
  int i, j, e, f, fb, n, N = equation().inconnue().valeurs().line_size(), m, M = equation().get_champ("pression").valeurs().line_size(),
                         ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot();
  const IntTab& fcl = ref_cast(Champ_Face_PolyVEF, mon_equation->inconnue()).fcl(), &e_f = domaine.elem_faces();
  IntTrav sten_a(0, 2), sten_p(0, 2), sten_v(0, 2);
  DoubleTrav w2;
  /* equations sum alpha_k = 1 */
  if (!aux_only)
    for (e = 0; e < domaine.nb_elem(); e++)
      for (n = 0; n < N; n++) sten_a.append_line(e, N * e + n);
  /* equations sur les p_f : continuite du gradient si interne, p = p_f si Neumann, sum_k alpha_k v_k = sum_k alpha_k v_k,imp si Dirichlet */
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (domaine.W2(nullptr, e, w2), i = 0; i < w2.dimension(0); i++)
      if ((f = e_f(e, i)) >= domaine.nb_faces()) continue; //faces virtuelles
      else if (!fcl(f, 0))
        for (sten_p.append_line(!aux_only * ne_tot + f, e), j = 0; j < w2.dimension(1); j++) //face interne
          {
            if (w2(i, j, 0) && fcl(fb = e_f(e, j), 0) != 1)
              for (m = 0; m < M; m++)
                sten_p.append_line(M * (!aux_only * ne_tot + f) + m, M * (ne_tot + fb) + m);
          }
      else if (fcl(f, 0) == 1)
        for (m = 0; m < M; m++) sten_p.append_line(M * (!aux_only * ne_tot + f) + m, M * (ne_tot + f) + m); //Neumann
      else for (n = 0, m = 0; n < N; n++, m += (M > 1)) sten_v.append_line(M * (!aux_only * ne_tot + f) + m, N * f + n); //Dirichlet

  tableau_trier_retirer_doublons(sten_v), tableau_trier_retirer_doublons(sten_p);
  if (!aux_only) Matrix_tools::allocate_morse_matrix(ne_tot + nf_tot, N * ne_tot, sten_a, *matrices.at("alpha"));
  Matrix_tools::allocate_morse_matrix(M * (!aux_only * ne_tot + nf_tot), M * (ne_tot + nf_tot), sten_p, *matrices.at("pression"));
  Matrix_tools::allocate_morse_matrix(M * (!aux_only * ne_tot + nf_tot), equation().inconnue().valeurs().size_totale(), sten_v, *matrices.at("vitesse"));
}

void Assembleur_P_PolyVEF_P0P1NC::assembler_continuite(matrices_t matrices, DoubleTab& secmem, int aux_only) const
{
  const Domaine_PolyVEF_P0P1NC& domaine = ref_cast(Domaine_PolyVEF_P0P1NC, le_dom_PolyMAC.valeur());
  const Pb_Multiphase* pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const DoubleTab *alpha = pbm ? &pbm->equation_masse().inconnue().valeurs() : nullptr, &press = equation().probleme().get_champ("pression").valeurs(),
                   &vit = equation().inconnue().valeurs(), *alpha_rho = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr, &nf = domaine.face_normales();
  const IntTab& fcl = ref_cast(Champ_Face_PolyVEF, mon_equation->inconnue()).fcl(), &e_f = domaine.elem_faces();
  const DoubleVect& ve = domaine.volumes(), &pe = equation().milieu().porosite_elem(), &fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces();
  int i, j, e, f, fb, n, N = vit.line_size(), m, M = press.line_size(), ne_tot = domaine.nb_elem_tot(), d, D = dimension;
  Matrice_Morse *mat_a = alpha ? matrices.at("alpha") : nullptr, &mat_p = *matrices.at("pression"), &mat_v = *matrices.at("vitesse");
  DoubleTrav w2, fac(N);
  double ar_tot, acc;
  secmem = 0, fac = 1;

  /* equations sum alpha_k = 1 */
  /* second membre : on multiplie par porosite * volume pour que le systeme en P soit symetrique en cartesien */
  if (!aux_only)
    for (e = 0; e < domaine.nb_elem(); e++)
      for (secmem(e) = -pe(e) * ve(e), n = 0; n < N; n++) secmem(e) += pe(e) * ve(e) * (*alpha)(e, n);
  /* matrice */
  if (!aux_only)
    for (e = 0; e < domaine.nb_elem(); e++)
      for (n = 0; n < N; n++) (*mat_a)(e, N * e + n) = -pe(e) * ve(e);

  /* equations sur les p_f : continuite du gradient si interne, p = p_f si Neumann, sum_k alpha_k v_k = sum_k alpha_k v_k,imp si Dirichlet */
  for (mat_p.get_set_coeff() = 0, mat_v.get_set_coeff() = 0, e = 0; e < ne_tot; e++)
    for (domaine.W2(nullptr, e, w2), i = 0; i < w2.dimension(0); i++)
      if ((f = e_f(e, i)) >= domaine.nb_faces()) continue; //faces virtuelles
      else if (!fcl(f, 0)) //face interne
        {
          for (acc = 0, j = 0; j < w2.dimension(1); acc+= pe(e) * vf(f) * w2(i, j, 0), j++)
            for (m = 0; m < M; m++) //second membre
              secmem(!aux_only * ne_tot + f, m) -= pe(e) * vf(f) * w2(i, j, 0) * (press(ne_tot + e_f(e, j), m) - press(e, m));
          for (m = 0; m < M; m++) mat_p(M * (!aux_only * ne_tot + f) + m, M * e + m) -= acc;
          for (j = 0; j < w2.dimension(1); j++) //matrice (sauf bords de Meumann)
            if (w2(i, j, 0) && fcl(fb = e_f(e, j), 0) != 1)
              for (m = 0; m <M; m++)
                mat_p(M * (!aux_only * ne_tot + f) + m, M * (ne_tot + fb) + m) += pe(e) * vf(f) * w2(i, j, 0);
        }
      else if (fcl(f, 0) == 1) //Neumann -> egalites p_f = p_imp
        {
          for (m = 0; m < M; m++) secmem(M * (!aux_only * ne_tot + f) + m) = fs(f) * (ref_cast(Neumann, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), m) - press(ne_tot + f, m));
          for (m = 0; m < M; m++) mat_p(M * (!aux_only * ne_tot + f) + m, M * (ne_tot + f) + m) = fs(f);
        }
      else  //Dirichlet -> egalite flux_tot_imp - flux_tot = 0
        {
          if (M == 1 && N > 1) //une pression, plusieurs vitesses -> on ne peut imposer qu'une ponderation : on choisit celle ocrrespondant aux flux de masse total
            {
              for (ar_tot = 0, n = 0; n < N; n++) ar_tot += (*alpha_rho)(e, n);
              for (n = 0; n < N; n++) fac(n) = (*alpha_rho)(e, n) / ar_tot;
            }
          else if (M != N) abort(); //sinon, il faut autant de pressions que de vitesses

          for (n = 0, m = 0; n < N; n++, m += (M > 1)) secmem(!aux_only * ne_tot + f, m) += vf(f) * fac(n) * vit(f, n);
          if (fcl(f, 0) == 3)
            for (d = 0; d < D; d++)
              for (n = 0, m = 0; n < N; n++, m += (M > 1)) //contrib de la valeur imposee: Dirichlet non homogene seulement
                secmem(!aux_only * ne_tot + f, m) -= vf(f) * fac(n) * nf(f, d) / fs(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n);
          for (n = 0, m = 0; n < N; n++, m += (M > 1)) mat_v(M * (!aux_only * ne_tot + f) + m, N * f + n) -= vf(f) * fac(n);
        }
}

void Assembleur_P_PolyVEF_P0P1NC::modifier_secmem_pour_incr_p(const DoubleTab& press, const double fac, DoubleTab& secmem) const
{
  const Domaine_PolyVEF_P0P1NC& domaine = ref_cast(Domaine_PolyVEF_P0P1NC, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, mon_equation->inconnue());
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const IntTab& fcl = ch.fcl();
  int f, ne_tot = domaine.nb_elem_tot(), m, M = equation().probleme().get_champ("pression").valeurs().line_size();
  for (f = 0; f < domaine.premiere_face_int(); f++)
    if (fcl(f, 0) == 1)
      for (m = 0; m < M; m++)
        secmem(ne_tot + f, m) = (ref_cast(Neumann_sortie_libre, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), m) - press(ne_tot + f, m)) / fac;
}

/* norme pour assembler_continuite */
DoubleTab Assembleur_P_PolyVEF_P0P1NC::norme_continuite() const
{
  const DoubleVect& pe= equation().milieu().porosite_elem(), &ve = le_dom_PolyMAC->volumes();
  DoubleTab norm(le_dom_PolyMAC->nb_elem());
  for (int e = 0; e < le_dom_PolyMAC->nb_elem(); e++) norm(e) = pe(e) * ve(e);
  return norm;
}

int Assembleur_P_PolyVEF_P0P1NC::modifier_solution(DoubleTab& pression)
{
  Debog::verifier("pression dans modifier solution in",pression);
  if(!has_P_ref) pression -= mp_min_vect(pression);
  return 1;
}
