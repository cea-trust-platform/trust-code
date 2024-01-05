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

#include <Op_Grad_PolyVEF_P0P1_Face.h>
#include <Masse_PolyVEF_Face.h>
#include <Champ_Elem_PolyVEF_P0P1.h>
#include <Champ_Face_PolyVEF.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_std.h>
#include <Domaine_Cl_PolyMAC.h>
#include <communications.h>
#include <Pb_Multiphase.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Matrix_tools.h>
#include <Milieu_base.h>
#include <Array_tools.h>
#include <Periodique.h>
#include <TRUSTTrav.h>
#include <SFichier.h>
#include <cfloat>

Implemente_instanciable(Op_Grad_PolyVEF_P0P1_Face, "Op_Grad_PolyVEF_P0P1_Face", Op_Grad_PolyMAC_P0_Face);

Sortie& Op_Grad_PolyVEF_P0P1_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Grad_PolyVEF_P0P1_Face::readOn(Entree& s) { return s; }

void Op_Grad_PolyVEF_P0P1_Face::completer()
{
  Operateur_Grad_base::completer();
  const Domaine_PolyMAC& dom = ref_domaine.valeur();
  /* besoin d'un joint de 1 */
  if (dom.domaine().nb_joints() && dom.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Grad_PolyVEF_P0P1_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  ref_cast(Champ_Elem_PolyVEF_P0P1, ref_cast(Navier_Stokes_std, equation()).pression()).init_auxiliary_variables();
  last_gradp_ = -DBL_MAX;
}

void Op_Grad_PolyVEF_P0P1_Face::dimensionner_blocs_ext(matrices_t matrices, int virt, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = ref_domaine.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const IntTab& f_e = dom.face_voisins(), &e_s = dom.domaine().les_elems(), &fcl = ch.fcl(), &scl_d = ch.scl_d(1);
  int i, j, e, f, s, ne_tot = dom.nb_elem_tot(), ns_tot = dom.nb_som_tot(), nf_tot = dom.nb_faces_tot(), d, D = dimension, n, N = ch.valeurs().line_size() / D,
                     m, M = (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()).line_size();

  IntTrav sten(0, 2); //stencil (NS, pression)
  Matrice_Morse *mat = matrices["pression"], mat2;

  for (f = 0; f < (virt ? dom.nb_faces_tot() : dom.nb_faces()); f++)
    if (fcl(f, 0) < 3 && f_e(f, 0) >= 0 && (fcl(f, 0) || f_e(f, 1) >= 0)) /* pour eviter les faces virtuelles auxquelles il manque un voisin */
      {
        /* elements amont/aval */
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (d = 0; d < D; d++)
              for (n = 0, m = 0; n < N; n++, m += (M > 1))
                sten.append_line(N * (D * f + d) + n, M * e + m);
            for (j = 0; j < e_s.dimension(1) && (s = e_s(e, j)) >= 0; j++)
              if (scl_d(s + 1) == scl_d(s))
                for (d = 0; d < D; d++)
                  for (n = 0, m = 0; n < N; n++, m += (M > 1))
                    sten.append_line(N * (D * f + d) + n, M * (ne_tot + s) + m);
          }
      }

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten);
  if (mat) Matrix_tools::allocate_morse_matrix(N * D * nf_tot, M * (ne_tot + ns_tot), sten, mat2), mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
}

void Op_Grad_PolyVEF_P0P1_Face::ajouter_blocs_ext(matrices_t matrices, DoubleTab& secmem, int virt, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = ref_domaine.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const Conds_lim& cls = ref_dcl->les_conditions_limites();
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl(), &f_s = dom.face_sommets(), &scl_d = ch.scl_d(1), &scl_c = ch.scl_c(1), &e_s = dom.domaine().les_elems(), &e_f = dom.elem_faces(), &es_d = dom.elem_som_d();
  const DoubleTab& vfd = dom.volumes_entrelaces_dir(), &nf = dom.face_normales(),
                   &press = semi_impl.count("pression") ? semi_impl.at("pression") : (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()),
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces(), &ve = dom.volumes(), &v_es = dom.vol_elem_som();
  int i, j, k, l, e, f, fb, s, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = secmem.line_size() / D, m, M = press.line_size();
  Matrice_Morse *mat = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : nullptr;

  DoubleTrav a_v(N), Gs(e_s.dimension(1), D); //produit alpha * vol, dependance du gradient P1 en chaque sommet

  /* aux faces */
  for (f = 0; f < (virt ? dom.nb_faces_tot() : dom.nb_faces()); f++)
    if (fcl(f, 0) < 3 && (f_e(f, 0) >= 0 && (fcl(f, 0) || f_e(f, 1) >= 0))) /* pour eviter les faces virtuelles auxquelles il manque un voisin */
      {
        /* produit porosite * alpha * volume */
        for (a_v = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          for (n = 0; n < N; n++) a_v(n) += vfd(f, i) / vf(f) * pf(f) * (alp ? (*alp)(e, n) : 1);

        /* second membre / matrice */
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            /* gradient P1 dans e*/
            for (Gs = 0, j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
              for (k = 0; k < f_s.dimension(1) && (s = f_s(fb, k)) >= 0; k++)
                for (l = (std::find(&e_s(e, 0), &e_s(e, 0) + e_s.dimension(1), s) - &e_s(e, 0)), d = 0; d < D; d++)
                  if (D < 3)
                    Gs(l, d) += 0.5 * nf(fb, d) * (e == f_e(fb, 0) ? 1 : -1) / ve(e);
                  else abort();

            /* contributions : gradient P1 + partie P0 selon la normale */
            for (d = 0; d < D; d++)
              for (n = 0, m = 0; n < N; n++, m += (M > 1))
                {
                  for (secmem(f, N * d + n) -= a_v(n) * (i ? 1 : -1) * nf(f, d) * press(e, m), j = 0, k = es_d(e); j < e_s.dimension(1) && (s = e_s(e, j)) >= 0; j++, k++)
                    secmem(f, N * d + n) -= a_v(n) * (vfd(f, i) * Gs(j, d) - (i ? 1 : -1) * nf(f, d) * v_es(k) / ve(e)) * (scl_d(s + 1) == scl_d(s) ? press(ne_tot + s, m) : ref_cast(Neumann, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), m));
                  if (mat)
                    for ((*mat)(N * (D * f + d) + n, M * e + m) += a_v(n) * (i ? 1 : -1) * nf(f, d), j = 0, k = es_d(e); j < e_s.dimension(1) && (s = e_s(e, j)) >= 0; j++, k++)
                      if (scl_d(s) == scl_d(s + 1))
                        (*mat)(N * (D * f + d) + n, M * (ne_tot + s) + m) += a_v(n) * (vfd(f, i) * Gs(j, d) - (i ? 1 : -1) * nf(f, d) * v_es(k) / ve(e));
                }
          }
      }
}
