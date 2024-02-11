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
#include <Assembleur_P_PolyVEF_P0P1.h>
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

const DoubleTab& Op_Grad_PolyVEF_P0P1_Face::alpha_es() const
{
  if (alpha_es_.nb_dim() > 1) return alpha_es_;
  const Domaine_PolyMAC& dom = ref_domaine.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const IntTab& e_s = dom.domaine().les_elems(), &scl_d = ch.scl_d(1), &ps_ref = ref_cast(Assembleur_P_PolyVEF_P0P1, ref_cast(Navier_Stokes_std, equation()).assembleur_pression().valeur()).ps_ref();;
  const DoubleTab& xp = dom.xp(), &xs = dom.domaine().coord_sommets();
  int e, s, i, j, D = dimension, nl = D + 1, nc, nm, un = 1, infoo = 0, rank, nw;
  double eps = 1e-8;
  std::vector<int> v_s;//sommets qu'on peut utiliser
  DoubleTrav A, B, W(1);
  IntTrav pvt;
  alpha_es_.resize(e_s.dimension_tot(0), e_s.dimension_tot(1)), alpha_es_ = 0;

  for (e = 0; e < dom.nb_elem_tot(); e++)
    {
      for (v_s.clear(), i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
        if (ps_ref(s) < 0 || scl_d(s) < scl_d(s + 1))
          v_s.push_back(s);
      nc = (int) v_s.size(), nm = std::max(nc, nl), A.resize(nc, nl), B.resize(nm), pvt.resize(nm);
      if (nc < D + 1) //pas assez de points pour une interpolation lineaire -> on fait comme on peut...
        {
          for (i = 0, j = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
            if (ps_ref(s) < 0 || scl_d(s) < scl_d(s + 1))
              j++;
          for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
            if (ps_ref(s) < 0 || scl_d(s) < scl_d(s + 1))
              alpha_es_(e, i) = 1. / j;
          continue;
        }

      /* systeme lineaire : interpolation exacte sur les fonctions affines */
      for (A = 0, B = 0, B(D) = 1, i = 0; i < nc; i++)
        for (s = v_s[i], j = 0; j < nl; j++)
          A(i, j) = j < D ? xs(s, j) - xp(e, j) : 1;
      /* resolution */
      nw = -1, pvt = 0,                     F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
      W.resize(nw = (int)std::lrint(W(0))), F77NAME(dgelsy)(&nl, &nc, &un, &A(0, 0), &nl, &B(0), &nc, &pvt(0), &eps, &rank, &W(0), &nw, &infoo);
      /* stockage */
      for (i = 0, j = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
        if (ps_ref(s) < 0 || scl_d(s) < scl_d(s + 1))
          alpha_es_(e, j) = B(j), j++;
    }
  return alpha_es_;
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

  /* aux faces : gradient aux faces + remplissage de dgp_pb */
  for (f = 0; f < (virt ? dom.nb_faces_tot() : dom.nb_faces()); f++)
    if (fcl(f, 0) < 2 && f_e(f, 0) >= 0 && (fcl(f, 0) || f_e(f, 1) >= 0)) /* pour eviter les faces virtuelles auxquelles il manque un voisin */
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
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl(), &f_s = dom.face_sommets(), &scl_d = ch.scl_d(1), &scl_c = ch.scl_c(1), &e_s = dom.domaine().les_elems();
  const DoubleTab& xp = dom.xp(), &xv = dom.xv(), &vfd = dom.volumes_entrelaces_dir(), &xs = dom.domaine().coord_sommets(), &nf = dom.face_normales(),
                   &press = semi_impl.count("pression") ? semi_impl.at("pression") : (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()),
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr, &a_es = alpha_es();
  const DoubleVect& fs = dom.face_surfaces(), &pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces();
  int i, j, e, f, s, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = secmem.line_size() / D, m, M = press.line_size();
  Matrice_Morse *mat = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : nullptr;

  DoubleTrav p_a_v(N), G(2 + f_s.dimension(1), D); //produit alpha * vol, dependance de grad p en p_e (2 premieres lignes) / p_s (lignes suivantes)
  double vz[3] = {0, 0, 1 }, fac;

  /* aux faces */
  for (f = 0; f < (virt ? dom.nb_faces_tot() : dom.nb_faces()); f++)
    if (fcl(f, 0) < 2 && (f_e(f, 0) >= 0 && (fcl(f, 0) || f_e(f, 1) >= 0))) /* pour eviter les faces virtuelles auxquelles il manque un voisin */
      {
        for (p_a_v = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          for (n = 0; n < N; n++) p_a_v(n) += vfd(f, i) * pf(f) * (alp ? (*alp)(e, n) : 1);

        /* gradient : diamant */
        for (G = 0, i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
          {
            //3D : sommet suivant dans la connectivite face -> sommet
            int ib = D < 3 ? -1 : i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0, sb = D < 3 ? -1 : f_s(f, ib);
            for (j = 0; j < 2; j++) //chaque cote
              if ((e = f_e(f, j)) >= 0) //un element
                {
                  auto v = dom.cross(D, 3, &xp(e, 0), D < 3 ? vz : &xs(sb, 0), &xs(s, 0), D < 3 ? nullptr : &xs(s, 0)); //produit normale * surface a la facette
                  for (fac = (dom.dot(&xp(e, 0), &v[0], &xv(f, 0)) > 0 ? 1. : -1.) / (vf(f) * D * (D - 1)), d = 0; d < D; d++) v[d] *= fac; //orientation vers l'exterieur, correction surface (3D), division par volume
                  for (d = 0; d < D; d++)
                    {
                      G(j, d) += v[d], G(2 + i, d) += v[d]; //element, sommet s
                      if (ib >= 0) G(2 + ib, d) += v[d]; //sommet sb (3D)
                    }
                }
              else if (D < 3) //face de bord en 2D : contribution du bord -> facile...
                for (d = 0; d < D; d++) G(2 + i, d) += nf(f, d) / (2 * vf(f));
              else //face de bord en 3D -> plus penible...
                {
                  auto v = dom.cross(D, D, &xs(s, 0), &xs(sb, 0), &xv(f, 0), &xv(f, 0));
                  fac = std::abs(dom.dot(&nf(f, 0), &v[0])) / (2 * fs(f) * vf(f) * 2 * fs(f)); //contribution de s ou sb : surface du triangle (f, s, sb) / (2 * volume)
                  for (d = 0; d < D; d++) G(2 + i, d) += fac * nf(f, d), G(2 + ib, d) += fac * nf(f, d);
                }
          }

        /* second membre / matrice : elements */
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (d = 0; d < D; d++)
              for (n = 0, m = 0; n < N; n++, m += (M > 1))
                {
                  for (secmem(f, N * d + n) -= p_a_v(n) * G(i, d) * D * press(e, m), j = 0; j < e_s.dimension(1) && (s = e_s(e, j)) >= 0; j++)
                    secmem(f, N * d + n) += (D - 1) * p_a_v(n) * G(i, d) * (scl_d(s + 1) == scl_d(s) ? press(ne_tot + s, m) : ref_cast(Neumann, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), m)) * a_es(e, j);
                  if (mat)
                    for ((*mat)(N * (D * f + d) + n, M * e + m) += D * p_a_v(n) * G(i, d), j = 0; j < e_s.dimension(1) && (s = e_s(e, j)) >= 0; j++)
                      if (scl_d(s) == scl_d(s + 1))
                        (*mat)(N * (D * f + d) + n, M * (ne_tot + s) + m) -= (D - 1) * p_a_v(n) * G(i, d) * a_es(e, j);
                }
          }
        /* second membre / matrice : sommets -> avec la pression au sommet si celle-ci n'est pas imposee, avec celle de la premiere CL qui passe sinon*/
        for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
          for (d = 0; d < D; d++)
            for (n = 0, m = 0; n < N; n++, m += (M > 1))
              {
                secmem(f, N * d + n) -= p_a_v(n) * G(2 + i, d) * (scl_d(s + 1) == scl_d(s) ? press(ne_tot + s, m) : ref_cast(Neumann, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), m));
                if (mat && scl_d(s + 1) == scl_d(s)) (*mat)(N * (D * f + d) + n, M * (ne_tot + s) + m) += p_a_v(n) * G(2 + i, d);
              }
      }
}
