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
#include <Op_Grad_PolyVEF_P0P1_Face.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <SFichier.h>
#include <Option_PolyVEF.h>

Implemente_instanciable(Op_Div_PolyVEF_P0P1, "Op_Div_PolyVEF_P0P1", Op_Div_PolyMAC_P0);

Sortie& Op_Div_PolyVEF_P0P1::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Div_PolyVEF_P0P1::readOn(Entree& s) { return s; }

void Op_Div_PolyVEF_P0P1::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const DoubleTab& inco = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl(), &scl_d = ch.scl_d(1), &f_s = dom.face_sommets(),
                &ps_ref = ref_cast(Assembleur_P_PolyVEF_P0P1, ref_cast(Navier_Stokes_std, equation()).assembleur_pression().valeur()).ps_ref();
  int i, e, f, s, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = inco.line_size() / D;

  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;
  IntTrav sten_v(0,2), sten_p(0, 2); //stencil des deux matrices

  for (f = 0; f < dom.nb_faces_tot(); f++)
    {
      if (fcl(f, 0) < 2) /* elements : faces internes / Neumann seulement */
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          if (e < dom.nb_elem()) //elements
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                sten_v.append_line(N * e + n, N * (D * f + d) + n);
      if (fcl(f, 0) < (Option_PolyVEF::sym_as_diri ? 2 : 3)) /* sommets : Symetrie aussi (sauf si sym_as_diri) */
        for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
          for (d = 0; s < dom.domaine().nb_som() && scl_d(s) == scl_d(s + 1) && d < D; d++) //sommets a pression non imposee
            for (n = 0; n < N; n++)
              sten_v.append_line(N * (ne_tot + s) + n, N * (D * f + d) + n);
    }

  for (e = 0; e < dom.nb_elem(); e++) sten_p.append_line(e, e); /* sten_p : diagonale du vide + egalites p_s = p_e pour les pressions inutilisees */
  for (s = 0; s < dom.nb_som_tot() ; s++)
    {
      if (s < dom.nb_som()) sten_p.append_line(ne_tot + s, ne_tot + s);
      if ((e = ps_ref(s)) >= 0)
        {
          if (s < dom.nb_som())  sten_p.append_line(ne_tot + s, e);
          if (e < dom.nb_elem()) sten_p.append_line(e, ne_tot + s);
        }
    }

  if (matv) tableau_trier_retirer_doublons(sten_v), Matrix_tools::allocate_morse_matrix(press.size_totale(), inco.size_totale(), sten_v, matv2);
  if (matp) tableau_trier_retirer_doublons(sten_p), Matrix_tools::allocate_morse_matrix(press.size_totale(), press.size_totale(), sten_p, matp2);
  if (matv) matv->nb_colonnes() ? *matv += matv2 : *matv = matv2;
  if (matp) matp->nb_colonnes() ? *matp += matp2 : *matp = matp2;
}

void Op_Div_PolyVEF_P0P1::ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const Conds_lim& cls = le_dcl_PolyMAC->les_conditions_limites();
  const DoubleTab& press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs(), &nf = dom.face_normales(), &xs = dom.domaine().coord_sommets(), &xp = dom.xp(), &xv = dom.xv();
  const IntTab& f_e = dom.face_voisins(), &f_s = dom.face_sommets(), &fcl = ch.fcl(), &scl_d = ch.scl_d(1), &scl_c = ch.scl_c(1),
                &ps_ref = ref_cast(Assembleur_P_PolyVEF_P0P1, ref_cast(Navier_Stokes_std, equation()).assembleur_pression().valeur()).ps_ref();
  const DoubleVect& pf = equation().milieu().porosite_face();
  int i, j, e, f, s = 0, sb, sc, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = vit.line_size() / D, has_P_ref = 0, has_s = secmem.dimension_tot(0) > ne_tot, ok;
  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;
  double vz[3] = { 0, 0, 1 }, xa[3], fac;
  DoubleTrav v(2, D);
  flux_bords_.resize(dom.nb_faces_bord(), N), flux_bords_ = 0;
  for (i = 0; i < cls.size(); i++)
    if (sub_type(Neumann_sortie_libre,cls[i].valeur())) has_P_ref = 1;
  if (matp && !has_P_ref)
    {
      if (Process::me() == Process::mp_min(dom.nb_elem() ? Process::me() : 1e8)) /* 1er proc possedant un element reel */
        (*matp)(0, 0) -= 1;
      const ArrOfInt& items_blocs = xs.get_md_vector().valeur().get_items_to_compute(); /* 1er proc possedant un sommet reel : plus dur... */
      for (ok = 0, i = 0; !ok && i < items_blocs.size_array(); i += 2)
        for (s = items_blocs[i]; s < items_blocs[i + 1]; s++)
          if ((ok |= ps_ref(s) < 0)) break;
      if (Process::me() == Process::mp_min(ok ? Process::me() : 1e8))
        (*matp)(ne_tot + s, ne_tot + s) -= 1;
    }

  for (f = 0; f < dom.nb_faces_tot(); f++)
    {
      if (f < dom.premiere_face_int())
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            flux_bords_(f, n) += nf(f, d) * pf(f) * vit(f, N * d + n);
      //div elem amont/aval
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        {
          if (e < dom.nb_elem())
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                {
                  secmem(e, n) += (i ? -1 : 1) * nf(f, d) / D * pf(f) * vit(f, N * d + n);
                  if (matv && fcl(f, 0) < 2)
                    (*matv)(N * e + n, N * (D * f + d) + n) -= (i ? -1 : 1) * nf(f, d) / D * pf(f);
                }
        }
      //divergence directe aux sommets
      for (i = 0; has_s && i < (D < 3 ? 1 : f_s.dimension(1)) && (s = f_s(f, i)) >= 0; i++)
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
            if ((sc = j ? sb : s) < dom.nb_som() && ps_ref(sc) < 0 && scl_d(sc) == scl_d(sc + 1))
              for (d = 0; d < D; d++)
                for (n = 0; n < N; n++)
                  {
                    secmem(ne_tot + sc, n) += (D - 1) * v(j, d) * pf(f) * vit(f, N * d + n) / D;
                    if (matv && fcl(f, 0) < (Option_PolyVEF::sym_as_diri ? 2 : 3))
                      (*matv)(N * (ne_tot + sc) + n, N * (D * f + d) + n) -= (D - 1) * v(j, d) * pf(f) / D;
                  }
        }
    }

  //equations restantes aux sommets : pressions imposees et inutiles
  for (s = 0; has_s && s < dom.nb_som_tot(); s++)
    if (s < dom.nb_som() && scl_d(s) < scl_d(s + 1)) /* lignes "pression aux sommets imposee" */
      for (n = 0; n < N; n++)
        {
          secmem(ne_tot + s, n) -= press(ne_tot + s, n) - ref_cast(Neumann, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), n);
          if (matp) (*matp)(N * (ne_tot + s) + n, N * (ne_tot + s) + n)++;
        }
    else if ((e = ps_ref(s)) >= 0) /* forces "p_s = p_sb" pour les pressions inutilisees */
      for (n = 0; n < N; n++)
        {
          if (s < dom.nb_som())
            {
              secmem(ne_tot + s, n)  -= press(e, n) - press(ne_tot + s, n);
              if (matp) (*matp)(N * (ne_tot + s) + n, N * (ne_tot + s) + n)--, (*matp)(N * (ne_tot + s) + n, N * e + n)++;
            }
          if (e < dom.nb_elem())
            {
              secmem(e, n) -= press(ne_tot + s , n) - press(e, n);
              if (matp) (*matp)(N * e + n, N * e + n)--, (*matp)(N * e + n, N * (ne_tot + s) + n)++;
            }
        }
}
