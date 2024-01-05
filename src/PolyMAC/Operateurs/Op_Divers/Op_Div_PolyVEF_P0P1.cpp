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

#include <Assembleur_P_PolyVEF_P0P1.h>
#include <Champ_Face_PolyVEF.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Op_Div_PolyVEF_P0P1.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <SFichier.h>

Implemente_instanciable(Op_Div_PolyVEF_P0P1, "Op_Div_PolyVEF_P0P1", Op_Div_PolyMAC_P0);

Sortie& Op_Div_PolyVEF_P0P1::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Div_PolyVEF_P0P1::readOn(Entree& s) { return s; }

void Op_Div_PolyVEF_P0P1::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyVEF& dom = ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const DoubleTab& inco = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl(), &scl_d = ch.scl_d(1), &e_s = dom.domaine().les_elems();
  int i, j, e, f, s, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = inco.line_size() / D;

  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;
  IntTrav sten_v(0,2), sten_p(0, 2); //stencil des deux matrices

  for (f = 0; f < dom.nb_faces_tot(); f++)
    if (fcl(f, 0) < 2) /* sten_v : divergence "classique" */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          if (e < dom.nb_elem()) //elements
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                sten_v.append_line(N * e + n, N * (D * f + d) + n);
          for (j = 0; j < e_s.dimension(1) && (s = e_s(e, j)) >= 0; j++)
            for (d = 0; s < dom.domaine().nb_som() && scl_d(s) == scl_d(s + 1) && d < D; d++) //sommets a pression non imposee
              for (n = 0; n < N; n++)
                sten_v.append_line(N * (ne_tot + s) + n, N * (D * f + d) + n);
        }

  for (e = 0; e < dom.nb_elem(); e++) sten_p.append_line(e, e); /* sten_p : diagonale du vide + egalites p_s = p_e pour les pressions inutilisees */
  for (s = 0; s < dom.nb_som() ; s++)
    sten_p.append_line(ne_tot + s, ne_tot + s);

  if (matv) tableau_trier_retirer_doublons(sten_v), Matrix_tools::allocate_morse_matrix(press.size_totale(), inco.size_totale(), sten_v, matv2);
  if (matp) tableau_trier_retirer_doublons(sten_p), Matrix_tools::allocate_morse_matrix(press.size_totale(), press.size_totale(), sten_p, matp2);
  if (matv) matv->nb_colonnes() ? *matv += matv2 : *matv = matv2;
  if (matp) matp->nb_colonnes() ? *matp += matp2 : *matp = matp2;
}

void Op_Div_PolyVEF_P0P1::ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyVEF& dom = ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const Conds_lim& cls = le_dcl_PolyMAC->les_conditions_limites();
  const DoubleTab& press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs(), &nf = dom.face_normales(), &xs = dom.domaine().coord_sommets(), &vfd = dom.volumes_entrelaces_dir();
  const IntTab& f_e = dom.face_voisins(), &f_s = dom.face_sommets(), &e_s = dom.domaine().les_elems(), &e_f = dom.elem_faces(), &fcl = ch.fcl(), &scl_d = ch.scl_d(1), &scl_c = ch.scl_c(1), &es_d = dom.elem_som_d();
  const DoubleVect& pf = equation().milieu().porosite_face(), &ve = dom.volumes(), &v_es = dom.vol_elem_som();
  int i, j, k, e, f, s = 0, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = vit.line_size() / D, has_P_ref = 0, has_s = secmem.dimension_tot(0) > ne_tot, ok, sgn;
  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;
  double prefac, prefac2, fac;
  DoubleTrav v(2, D), S_se(e_s.dimension(1), D);
  flux_bords_.resize(dom.nb_faces_bord(), N);
  for (i = 0; i < cls.size(); i++)
    if (sub_type(Neumann_sortie_libre,cls[i].valeur())) has_P_ref = 1;
  if (matp && !has_P_ref)
    {
      if (Process::me() == Process::mp_min(dom.nb_elem() ? Process::me() : 1e8)) /* 1er proc possedant un element reel */
        (*matp)(0, 0) += 1;
      const ArrOfInt& items_blocs = xs.get_md_vector().valeur().get_items_to_compute(); /* 1er proc possedant un sommet reel : plus dur... */
      for (ok = 0, i = 0; i < items_blocs.size_array(); i += 2)
        if ((ok |= items_blocs[i] < items_blocs[i + 1])) break;
      if (Process::me() == Process::mp_min(ok ? Process::me() : 1e8))
        (*matp)(ne_tot + items_blocs[i], ne_tot + items_blocs[i]) += 1;
    }

  /* flux_bords_ */
  for (f = 0; f < dom.premiere_face_int(); f++)
    for (d = 0; d < D; d++)
      for (n = 0; n < N; n++)
        flux_bords_(f, n) += nf(f, d) * pf(f) * vit(f, N * d + n);

  for (e = 0; e < dom.nb_elem_tot(); e++)
    {
      for (S_se = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        {
          sgn = (e == f_e(f, 0) ? 1 : -1);
          /* contribs aux elements */
          if (e < dom.nb_elem())
            for (d = 0, prefac = sgn * pf(f); d < D; d++)
              for (fac = prefac * nf(f, d), n = 0; n < N; n++)
                {
                  secmem(e, n) += fac * vit(f, N * d + n);
                  if (matv && fcl(f, 0) < 2)
                    (*matv)(N * e + n, N * (D * f + d) + n) -= fac;
                }
          /* remplissage de Sse */
          for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
            for (k = (int)(std::find(&e_s(e, 0), &e_s(e, 0) + e_s.dimension(1), s) - &e_s(e, 0)), d = 0; d < D; d++)
              S_se(k, d) -= sgn * nf(f, d) / 2;
        }

      /* contribs aux sommets */
      if (has_s)
        for (i = 0, j = es_d(e); i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++, j++)
          if (s < dom.nb_som() && scl_d(s) == scl_d(s + 1))
            for (k = 0; k < e_f.dimension(1) && (f = e_f(e, k)) >= 0; k++)
              for (prefac = pf(f) * vfd(f, e != f_e(f, 0)) / ve(e), prefac2 = (e == f_e(f, 0) ? 1 : -1) * pf(f) * v_es(j) / ve(e), d = 0; d < D; d++)
                for (fac = prefac * S_se(i, d) - prefac2 * nf(f, d), n = 0; n < N; n++)
                  {
                    secmem(ne_tot + s, n) += fac * vit(f, N * d + n);
                    if (matv && fcl(f, 0) < 2)
                      (*matv)(N * (ne_tot + s) + n, N * (D * f + d) + n) -= fac;
                  }
    }

  //equations restantes aux sommets : pressions imposees et inutiles
  for (s = 0; has_s && s < dom.nb_som(); s++)
    if (scl_d(s) < scl_d(s + 1)) /* lignes "pression aux sommets imposee" */
      for (n = 0; n < N; n++)
        {
          secmem(ne_tot + s, n) -= press(ne_tot + s, n) - ref_cast(Neumann, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), n);
          if (matp) (*matp)(N * (ne_tot + s) + n, N * (ne_tot + s) + n)++;
        }
}
