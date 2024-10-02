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

#include <Assembleur_P_PolyVEF_P0.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Op_Div_PolyVEF_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <SFichier.h>

Implemente_instanciable(Op_Div_PolyVEF_P0, "Op_Div_PolyVEF_P0", Op_Div_PolyMAC_P0);

Sortie& Op_Div_PolyVEF_P0::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Div_PolyVEF_P0::readOn(Entree& s) { return s; }

void Op_Div_PolyVEF_P0::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue());
  const DoubleTab& inco = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  const IntTab& f_e = dom.face_voisins();
  int i, e, f, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = inco.line_size() / D;

  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;
  IntTrav sten_v(0, 2), sten_p(0, 2); //stencil des deux matrices

  for (f = 0; f < dom.nb_faces(); f++) /* sten_v : divergence "classique" */
    {
      for (i = 0; i < 2; i++)
        if ((e = f_e(f, i)) < dom.nb_elem()) //elements
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              sten_v.append_line(N * (e >= 0 ? e : ne_tot + f) + n, N * (D * f + d) + n);
    }

  for (e = 0; e < dom.nb_elem(); e++)
    sten_p.append_line(e, e); /* sten_p : diagonale du vide */
  for (f = 0; f < dom.nb_faces_bord(); f++)
    sten_p.append_line(ne_tot + f, ne_tot + f);

  if (matv)
    tableau_trier_retirer_doublons(sten_v), Matrix_tools::allocate_morse_matrix(press.size_totale(), inco.size_totale(), sten_v, matv2);
  if (matp)
    tableau_trier_retirer_doublons(sten_p), Matrix_tools::allocate_morse_matrix(press.size_totale(), press.size_totale(), sten_p, matp2);
  if (matv)
    matv->nb_colonnes() ? *matv += matv2 : *matv = matv2;
  if (matp)
    matp->nb_colonnes() ? *matp += matp2 : *matp = matp2;
}

void Op_Div_PolyVEF_P0::ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue());
  const Conds_lim& cls = le_dcl_PolyMAC->les_conditions_limites();
  const DoubleTab& nf = dom.face_normales();
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pf = equation().milieu().porosite_face();
  int i, e, f, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = vit.line_size() / D, has_P_ref = 0, has_fb = secmem.dimension_tot(0) > ne_tot;
  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr, matv2, matp2;

  for (i = 0; i < cls.size(); i++)
    if (sub_type(Neumann_sortie_libre, cls[i].valeur()))
      has_P_ref = 1;
  if (matp && !has_P_ref && Process::me() == Process::mp_min(dom.nb_elem() ? Process::me() : 1e8)) /* 1er proc possedant un element reel */
    (*matp)(0, 0) += 1;

  flux_bords_.resize(dom.nb_faces_bord(), N), flux_bords_ = 0;
  for (f = 0; f < dom.nb_faces(); f++)
    {
      /* contributions amont / aval */
      for (i = 0; i < 2; i++)
        if ((e = f_e(f, i)) < 0 && !has_fb) //faces de bord non demandees
          continue;
        else if (e < 0 && fcl(f, 0) == 1) //CL de Neumann -> p = p_imp
          for (n = 0; n < N; n++)
            {
              secmem(ne_tot + f, n) += ref_cast(Neumann, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), n);
              if (matv)
                (*matv)(N * (ne_tot + f) + n, N * (ne_tot + f) + n)--;
            }
        else if (e < dom.nb_elem()) //elem reel ou bord non Neumann
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              {
                secmem(e >= 0 ? e : ne_tot + f, n) += (i ? -1 : 1) * pf(f) * nf(f, d) * vit(f, N * d + n);
                if (matv)
                  (*matv)(N * (e >= 0 ? e : ne_tot + f) + n, N * (D * f + d) + n) -= (i ? -1 : 1) * pf(f) * nf(f, d);
              }
      /* aux bords : flux_bords_ + contrib des CLs de Dirichlet a la divergence */
      if (f < dom.premiere_face_int())
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            flux_bords_(f, n) += nf(f, d) * pf(f) * vit(f, N * d + n);
      if (has_fb && fcl(f, 0) == 3)
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            secmem(ne_tot + f, n) += nf(f, d) * pf(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n);
    }
}
