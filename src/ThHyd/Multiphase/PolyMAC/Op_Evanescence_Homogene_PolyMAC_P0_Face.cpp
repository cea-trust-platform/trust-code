/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Op_Evanescence_Homogene_PolyMAC_P0_Face.h>
#include <Champ_Face_base.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Op_Evanescence_Homogene_PolyMAC_P0_Face, "Op_Evanescence_HOMOGENE_PolyMAC_P0_Face", Op_Evanescence_Homogene_Face_base);

Sortie& Op_Evanescence_Homogene_PolyMAC_P0_Face::printOn(Sortie& os) const { return os; }
Entree& Op_Evanescence_Homogene_PolyMAC_P0_Face::readOn(Entree& is) { return Op_Evanescence_Homogene_Face_base::readOn(is); }

void Op_Evanescence_Homogene_PolyMAC_P0_Face::dimensionner_blocs_aux(std::set<int>& idx, IntTrav& sten,  Matrice_Morse& mat ) const
{
  const Zone_VF& zone = ref_cast(Zone_VF, equation().zone_dis().valeur());
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  const int nf_tot = zone.nb_faces_tot(), D = dimension, N = ch.valeurs().line_size() ;
  int i, j, l, e, d, n;

  for (e = 0, l = nf_tot; e < zone.nb_elem_tot(); e++)
    for (d = 0; d < D; d++, l++, idx.clear())
      {
        for (i = N * l, n = 0; n < N; n++, i++)
          for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
            idx.insert(mat.get_tab2()(j) - 1);
        for (i = N * l, n = 0; n < N; n++, i++)
          for (auto &&c : idx)
            sten.append_line(i, c);
      }
}

void Op_Evanescence_Homogene_PolyMAC_P0_Face::ajouter_blocs_aux(IntTrav& maj, DoubleTrav coeff, matrices_t matrices, DoubleTab& secmem) const
{
  const Zone_VF& zone = ref_cast(Zone_VF, equation().zone_dis().valeur());
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  const DoubleTab& inco = ch.valeurs(), &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe();
  double a_eps = alpha_res_, a_eps_min = alpha_res_min_, a_m, a_max; //seuil de declenchement du traitement de l'evanescence
  Matrice_Morse& mat_diag = *matrices.at(ch.le_nom().getString());

  int e, i, j, k, l, n, N = inco.line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot();
  for (e = 0; e < zone.nb_elem_tot(); e++) /* elements : a faire D fois par element */
    {
      /* phase majoritaire : directement dans l'element */
      for (a_max = 0, k = -1, n = 0; n < N; n++)
        if ((a_m = alpha(e, n)) > a_max) k = n, a_max = a_m;
      if (k >= 0)
        for (i = nf_tot + D * e, d = 0; d < D; d++, i++) maj(i) = k;
      else abort();

      /* coeff d'evanescence */
      for (i = nf_tot + D * e, d = 0; d < D; d++, i++)
        for (n = 0; n < N; n++)
          if (n != k && (a_m = alpha(e, n)) < a_eps)
            {
              coeff(i, n, 1) = mat_diag(N * i + k, N * i + k) * (coeff(i, n, 0) = std::min(std::max((a_eps - a_m) / (a_eps - a_eps_min), 0.), 1.));
              double flux = coeff(i, n, 0) * secmem(i, n) + coeff(i, n, 1) * (inco(i, n) - inco(i, k));
              secmem(i, k) += flux, secmem(i, n) -= flux;
            }
    }

  /* lignes de matrices */
  for (auto &&n_m : matrices)
    if (n_m.second->nb_colonnes())
      {
        int diag = (n_m.first == ch.le_nom().getString()); //est-on sur le bloc diagonal?
        Matrice_Morse& mat = *n_m.second;
        for (e = 0, l = nf_tot; e < zone.nb_elem_tot(); e++) /* elements : l est l'indice de ligne */
          for (d = 0; d < D; d++, l++)
            for (n = 0; n < N; n++)
              if (coeff(l, n, 0))
                for (k = maj(l), i = mat.get_tab1()(N * l + n) - 1, j = mat.get_tab1()(N * l + k) - 1; i < mat.get_tab1()(N * l + n + 1) - 1; i++, j++)
                  {
                    assert(mat.get_tab2()(i) == mat.get_tab2()(j));
                    int c = diag * mat.get_tab2()(i) - 1; //indice de colonne (commun aux deux lignes grace au dimensionner_blocs())
                    mat.get_set_coeff()(j) += coeff(l, n, 0) * mat.get_set_coeff()(i) - coeff(l, n, 1) * ((c == N * l + n) - (c == N * l + k));
                    mat.get_set_coeff()(i) += -coeff(l, n, 0) * mat.get_set_coeff()(i) + coeff(l, n, 1) * ((c == N * l + n) - (c == N * l + k));
                  }
      }
}
