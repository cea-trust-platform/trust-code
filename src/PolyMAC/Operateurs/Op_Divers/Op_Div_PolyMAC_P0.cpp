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

#include <Champ_Face_PolyMAC_P0.h>
#include <Op_Div_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

Implemente_instanciable(Op_Div_PolyMAC_P0, "Op_Div_PolyMAC_P0", Op_Div_PolyMAC_P0P1NC);

Sortie& Op_Div_PolyMAC_P0::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Div_PolyMAC_P0::readOn(Entree& s) { return s; }

void Op_Div_PolyMAC_P0::dimensionner(Matrice_Morse& matrice) const
{
  if (has_interface_blocs())
    {
      Operateur_base::dimensionner(matrice);
      return;
    }

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_PolyMAC.valeur());
  const IntTab& f_e = domaine.face_voisins();
  int i, e, f, n, ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot(),
                  N = equation().inconnue().valeurs().line_size(), D = dimension;

  IntTab stencil(0, 2);

  for (f = 0; f < domaine.nb_faces(); f++)
    for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
      for (n = 0; n < N; n++)
        stencil.append_line(N * e + n, N * f + n);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * ne_tot, N * (nf_tot + D * ne_tot), stencil, matrice);
}

DoubleTab& Op_Div_PolyMAC_P0::ajouter(const DoubleTab& vit, DoubleTab& div) const
{
  if (has_interface_blocs()) return Operateur_base::ajouter(vit, div);

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_PolyMAC.valeur());
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face();
  const IntTab& f_e = domaine.face_voisins();
  int i, e, f, n, N = vit.line_size();
  assert(div.line_size() == N);

  DoubleTab& tab_flux_bords = flux_bords_;
  tab_flux_bords.resize(domaine.nb_faces_bord(), 1);
  tab_flux_bords = 0;

  for (f = 0; f < domaine.nb_faces(); f++)
    {
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          div(e, n) += (i ? -1 : 1) * fs(f) * pf(f) * vit(f, n);
      if (f < domaine.premiere_face_int())
        for (n = 0; n < N; n++)
          tab_flux_bords(f, n) = fs(f) * pf(f) * vit(f, n);
    }

  div.echange_espace_virtuel();

  return div;
}
void Op_Div_PolyMAC_P0::contribuer_a_avec(const DoubleTab& incoo, Matrice_Morse& mat) const
{
  if (has_interface_blocs())
    {
      Operateur_base::contribuer_a_avec(incoo, mat);
      return;
    }

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue());
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face();
  const IntTab& f_e = domaine.face_voisins();
  int i, e, f, n, N = ch.valeurs().line_size();

  for (f = 0; f < domaine.nb_faces(); f++)
    for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
      for (n = 0; n < N; n++)
        mat(N * e + n, N * f + n) += (i ? 1 : -1) * fs(f) * pf(f);
}
