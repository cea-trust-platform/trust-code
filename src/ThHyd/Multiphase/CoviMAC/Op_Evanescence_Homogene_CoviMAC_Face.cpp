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
// File:        Op_Evanescence_Homogene_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/CoviMAC
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Evanescence_Homogene_CoviMAC_Face.h>
#include <Zone_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>

#include <Zone_Cl_CoviMAC.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <SETS.h>
#include <Param.h>

Implemente_instanciable(Op_Evanescence_Homogene_CoviMAC_Face,"Op_Evanescence_Homogene_CoviMAC_Face",Operateur_Evanescence_base);

Sortie& Op_Evanescence_Homogene_CoviMAC_Face::printOn(Sortie& os) const
{
  return os;
}

Entree& Op_Evanescence_Homogene_CoviMAC_Face::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("alpha_res", &alpha_res_, Param::REQUIRED);
  param.ajouter("alpha_res_min", &alpha_res_min_);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Op_Evanescence_Homogene_CoviMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();
  const IntTab& fcl = ch.fcl();

  /* on doit pouvoir ajouter / soustraire les equations entre composantes */
  int i, j, l, e, f, n, N = inco.line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot();
  if (N == 1) return; //pas d'evanescence en simple phase!
  for (auto &&n_m : matrices) if (n_m.second->nb_colonnes())
      {
        IntTrav sten(0, 2);
        sten.set_smart_resize(1);

        std::set<int> idx;
        Matrice_Morse& mat = *n_m.second, mat2;
        /* equations aux faces : celles calculees seulement */
        for (f = 0; f < zone.nb_faces(); f++, idx.clear()) if (fcl(f, 0) < 2)
            {
              for (i = N * f, n = 0; n < N; n++, i++) for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
                  idx.insert(mat.get_tab2()(j) - 1);
              for (i = N * f, n = 0; n < N; n++, i++) for (auto &&c : idx) sten.append_line(i, c);
            }
          else if (n_m.first == ch.le_nom().getString()) /* CLs a valeur imposee : diagonale */
            for (n = 0, i = N * f; n < N; n++, i++) sten.append_line(i, i);

        /* equations aux elements */
        for (e = 0, l = nf_tot; e < zone.nb_elem_tot(); e++) for (d = 0; d < D; d++, l++, idx.clear())
            {
              for (i = N * l, n = 0; n < N; n++, i++) for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
                  idx.insert(mat.get_tab2()(j) - 1);
              for (i = N * l, n = 0; n < N; n++, i++) for (auto &&c : idx) sten.append_line(i, c);
            }
        Matrix_tools::allocate_morse_matrix(mat.nb_lignes(), mat.nb_colonnes(), sten, mat2);
        mat = mat2; //pour forcer l'ordre des coefficients dans la matrice (accelere les operations ligne a ligne)
      }
}

void Op_Evanescence_Homogene_CoviMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl();
  const DoubleTab& inco = ch.valeurs(), &vfd = zone.volumes_entrelaces_dir(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe();
  const DoubleVect& vf = zone.volumes_entrelaces();
  int e, f, i, j, k, l, n, N = inco.line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot(),
                           iter = sub_type(SETS, equation().schema_temps()) ? 0 * ref_cast(SETS, equation().schema_temps()).iteration : 0;
  if (N == 1) return; //pas d'evanescence en simple phase!

  double a_eps = alpha_res_, a_eps_min = alpha_res_min_, a_m, a_max; //seuil de declenchement du traitement de l'evanescence

  /* recherche de phases evanescentes et traitement des seconds membres */
  IntTrav maj(inco.dimension_tot(0)); //maj(i) : phase majoritaire de la ligne i
  DoubleTrav coeff(inco.dimension_tot(0), inco.line_size(), 2); //coeff(i, n, 0/1) : coeff a appliquer a l'equation existante / a l'eq. "inco = v_maj"
  Matrice_Morse& mat_diag = *matrices.at(ch.le_nom().getString());
  for (f = 0; f < zone.nb_faces(); f++) if (fcl(f, 0) < 2)
      {
        /* phase majoritaire : avec alpha interpole par defaut, avec alpha amont pour les ierations de SETS / ICE */
        for (a_max = 0, k = -1, n = 0; n < N; n++)
          {
            if (iter) a_m = alpha(f_e(f, f_e(f, 1) >= 0 && inco(f, n) < 0), n);
            else for (a_m = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) a_m += vfd(f, i) / vf(f) * alpha(e, n);
            if (a_m > a_max) k = n, a_max = a_m;
          }
        if (k >= 0) maj(f) = k;
        else abort();

        /* phases evanescentes : avec alpha amont. La phase majoritaire ne peut pas etre evanescente! */
        for (n = 0; n < N; n++)
          {
            if (iter) a_m = alpha(f_e(f, f_e(f, 1) >= 0 && inco(f, n) < 0), n);
            else for (a_m = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) a_m += vfd(f, i) / vf(f) * alpha(e, n);
            if (n != k && a_m < a_eps)
              {
                coeff(f, n, 1) = mat_diag(N * f + k, N * f + k) * (coeff(f, n, 0) = std::min(std::max((a_eps - a_m) / (a_eps - a_eps_min), 0.), 1.));
                double flux = coeff(f, n, 0) * secmem(f, n) + coeff(f, n, 1) * (inco(f, n) - inco(f, k));
                secmem(f, k) += flux, secmem(f, n) -= flux;
              }
          }

      }

  for (e = 0; e < zone.nb_elem_tot(); e++) /* elements : a faire D fois par element */
    {
      /* phase majoritaire : directement dans l'element */
      for (a_max = 0, k = -1, n = 0; n < N; n++) if ((a_m = alpha(e, n)) > a_max) k = n, a_max = a_m;
      if (k >= 0) for (i = nf_tot + D * e, d = 0; d < D; d++, i++) maj(i) = k;
      else abort();

      /* coeff d'evanescence */
      for (i = nf_tot + D * e, d = 0; d < D; d++, i++) for (n = 0; n < N; n++) if (n != k && (a_m = alpha(e, n)) < a_eps)
            {
              coeff(i, n, 1) = mat_diag(N * i + k, N * i + k) * (coeff(i, n, 0) = std::min(std::max((a_eps - a_m) / (a_eps - a_eps_min), 0.), 1.));
              double flux = coeff(i, n, 0) * secmem(i, n) + coeff(i, n, 1) * (inco(i, n) - inco(i, k));
              secmem(i, k) += flux, secmem(i, n) -= flux;
            }
    }

  /* lignes de matrices */
  for (auto &&n_m : matrices) if (n_m.second->nb_colonnes())
      {
        int diag = (n_m.first == ch.le_nom().getString()); //est-on sur le bloc diagonal?
        Matrice_Morse& mat = *n_m.second;
        /* faces */
        for (f = 0; f < zone.nb_faces(); f++) if (fcl(f, 0) < 2) for (n = 0; n < N; n++) if (coeff(f, n, 0))
                for (k = maj(f), i = mat.get_tab1()(N * f + n) - 1, j = mat.get_tab1()(N * f + k) - 1; i < mat.get_tab1()(N * f + n + 1) - 1; i++, j++)
                  {
                    assert(mat.get_tab2()(i) == mat.get_tab2()(j));
                    int c = diag * mat.get_tab2()(i) - 1; //indice de colonne (commun aux deux lignes grace au dimensionner_blocs())
                    mat.get_set_coeff()(j) +=  coeff(f, n, 0) * mat.get_set_coeff()(i) - coeff(f, n, 1) * ((c == N * f + n) - (c == N * f + k));
                    mat.get_set_coeff()(i) += -coeff(f, n, 0) * mat.get_set_coeff()(i) + coeff(f, n, 1) * ((c == N * f + n) - (c == N * f + k));
                  }
        /* elements : l est l'indice de ligne */
        for (e = 0, l = nf_tot; e < zone.nb_elem_tot(); e++) for (d = 0; d < D; d++, l++) for (n = 0; n < N; n++) if (coeff(l, n, 0))
                for (k = maj(l), i = mat.get_tab1()(N * l + n) - 1, j = mat.get_tab1()(N * l + k) - 1; i < mat.get_tab1()(N * l + n + 1) - 1; i++, j++)
                  {
                    assert(mat.get_tab2()(i) == mat.get_tab2()(j));
                    int c = diag * mat.get_tab2()(i) - 1; //indice de colonne (commun aux deux lignes grace au dimensionner_blocs())
                    mat.get_set_coeff()(j) +=  coeff(l, n, 0) * mat.get_set_coeff()(i) - coeff(l, n, 1) * ((c == N * l + n) - (c == N * l + k));
                    mat.get_set_coeff()(i) += -coeff(l, n, 0) * mat.get_set_coeff()(i) + coeff(l, n, 1) * ((c == N * l + n) - (c == N * l + k));
                  }
      }
}
