/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Op_Div_PolyMAC_P0P1NC.h>
#include <Check_espace_virtuel.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Debog.h>

Implemente_instanciable(Op_Div_PolyMAC_P0P1NC, "Op_Div_PolyMAC_P0P1NC", Op_Div_PolyMAC);

Sortie& Op_Div_PolyMAC_P0P1NC::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Div_PolyMAC_P0P1NC::readOn(Entree& s) { return s; }

void Op_Div_PolyMAC_P0P1NC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC_P0P1NC& domaine = ref_cast(Domaine_PolyMAC_P0P1NC, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue());
  const DoubleTab& inco = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const int ne_tot = domaine.nb_elem_tot();

  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr,
                 *matp = matrices.count("pression") ? matrices["pression"] : nullptr,
                  matv2, matp2;

  IntTab sten_v(0, 2), sten_p(0, 2);
  DoubleTab w2; //matrice w2 aux elements (la meme que dans Op_Grad et Assembleur_P)

  // Dependance en v : divergence par element et v = v_imp aux faces de Dirichlet
  if (matv)
    for (int f = 0; f < domaine.nb_faces(); f++)
      {
        if (fcl(f, 0) > 1) // v impose par les conditions limites
          sten_v.append_line(ne_tot + f, f);
        else // v calcule : contribution a la divergence aux elements
          for (int i = 0; i < 2; i++)
            {
              const int e = f_e(f, i);
              if (e < 0) continue;

              if (e < domaine.nb_elem())
                sten_v.append_line(e, f);
            }
      }

  // Dependance en p : equation sur p_f
  if (matp)
    {
      for (int e = 0; e < domaine.nb_elem_tot(); e++)
        {
          domaine.W2(nullptr, e, w2); // Calcul de la matrice W2

          for (int i = 0; i < w2.dimension(0); i++)
            {
              int f = e_f(e, i);
              if (fcl(f, 0) == 1) // Aux faces de Neumann : p_f = p_imp
                sten_p.append_line(ne_tot + f, ne_tot + f);
              else if (!fcl(f, 0))
                {
                  // Aux faces internes : egalite des deux gradients
                  sten_p.append_line(ne_tot + f, e);
                  for (int j = 0; j < w2.dimension(1); j++)
                    if (w2(i, j, 0))
                      sten_p.append_line(ne_tot + f, ne_tot + e_f(e, j));
                }
            }
        }

      // Diagonale du vide pour matp
      for (int e = 0; e < domaine.nb_elem(); e++)
        sten_p.append_line(e, e);
    }

  // Allocation des matrices pour matv
  if (matv)
    {
      tableau_trier_retirer_doublons(sten_v);
      Matrix_tools::allocate_morse_matrix(press.size_totale(), inco.size_totale(), sten_v, matv2);

      if (matv->nb_colonnes())
        *matv += matv2;
      else
        *matv = matv2;
    }

  // Allocation des matrices pour matp
  if (matp)
    {
      tableau_trier_retirer_doublons(sten_p);
      Matrix_tools::allocate_morse_matrix(press.size_totale(), press.size_totale(), sten_p, matp2);

      if (matp->nb_colonnes())
        *matp += matp2;
      else
        *matp = matp2;
    }
}

void Op_Div_PolyMAC_P0P1NC::ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC_P0P1NC& domaine = ref_cast(Domaine_PolyMAC_P0P1NC, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue());
  const Conds_lim& cls = le_dcl_PolyMAC->les_conditions_limites();
  const DoubleTab& press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs(), &nf = domaine.face_normales();
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face();
  const int ne_tot = domaine.nb_elem_tot(), D = dimension, has_f = secmem.dimension_tot(0) > ne_tot;
  Matrice_Morse *matv = matrices.count("vitesse") ? matrices["vitesse"] : nullptr, *matp = matrices.count("pression") ? matrices["pression"] : nullptr; //, matv2, matp2;

  DoubleTrav w2; //matrice w2 aux elements (la meme que dans Op_Grad et Assembleur_P)


  DoubleTab& tab_flux_bords = flux_bords_;
  tab_flux_bords.resize(domaine.nb_faces_bord(), 1);

  for (int f = 0; f < domaine.nb_faces(); f++) /* divergence aux elements + equations aux bords */
    {
      for (int i = 0; i < 2; i++)
        {
          const int e = f_e(f, i);
          if (e < 0) continue;

          if (e < domaine.nb_elem()) /* divergence aux elems */
            {
              secmem(e) -= (i ? 1 : -1) * fs(f) * pf(f) * vit(f);
              if (fcl(f, 0) < 2 && matv)
                (*matv)(e, f) += (i ? 1 : -1) * fs(f) * pf(f);
            }
        }

      if (f < domaine.premiere_face_int())
        tab_flux_bords(f) = fs(f) * pf(f) * vit(f);

      /* equations v = v_imp ou p = p_imp aux faces de bord */
      if (has_f && fcl(f, 0) > 1)
        {
          if (fcl(f, 0) == 3)
            for (int d = 0; d < D; d++)
              secmem(ne_tot + f) += nf(f, d) * pf(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), d);
          secmem(ne_tot + f) -= fs(f) * pf(f) * vit(f);
          if (matv)
            (*matv)(ne_tot + f, f) += fs(f) * pf(f);
        }
      else if (has_f && matp && fcl(f, 0) == 1) // si appel depuis ajouter(vpoint, div) alors on ne fait rien ici
        {
          secmem(ne_tot + f) += ref_cast(Neumann, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), 0) - press(ne_tot + f);
          (*matp)(ne_tot + f, ne_tot + f) += 1;
        }
    }

  /* equations aux faces internes : egalite des gradients */
  if (!has_f || matrices.size() == 0) return; // already done in assembleur_pression
  for (int e = 0; e < domaine.nb_elem_tot(); e++)
    {
      domaine.W2(nullptr, e, w2);
      for (int i = 0; i < w2.dimension(0); i++)
        {
          const int f = e_f(e, i);

          if (f < domaine.nb_faces() && !fcl(f, 0))
            {
              double coeff_e = 0.;

              for (int j = 0; j < w2.dimension(1); j++)
                if (w2(i, j, 0))
                  {
                    const int fb = e_f(e, j);

                    secmem(ne_tot + f) -= pf(f) * w2(i, j, 0) * (press(ne_tot + fb) - press(e));

                    if (matp)
                      (*matp)(ne_tot + f, ne_tot + fb) += pf(f) * w2(i, j, 0), coeff_e += pf(f) * w2(i, j, 0);
                  }

              if (matp)
                (*matp)(ne_tot + f, e) -= coeff_e;
            }
        }
    }
}
