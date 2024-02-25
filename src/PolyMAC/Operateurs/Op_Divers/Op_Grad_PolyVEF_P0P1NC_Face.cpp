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

#include <Op_Grad_PolyVEF_P0P1NC_Face.h>
#include <Champ_Elem_PolyVEF_P0P1NC.h>
#include <Masse_PolyVEF_Face.h>
#include <Champ_Face_PolyVEF.h>
#include <Check_espace_virtuel.h>
#include <Neumann_sortie_libre.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <Milieu_base.h>
#include <Dirichlet.h>
#include <TRUSTTrav.h>
#include <cfloat>
#include <Option_PolyVEF.h>

extern Stat_Counter_Id gradient_counter_;

Implemente_instanciable(Op_Grad_PolyVEF_P0P1NC_Face, "Op_Grad_PolyVEF_P0P1NC_Face", Op_Grad_PolyMAC_Face);

Sortie& Op_Grad_PolyVEF_P0P1NC_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Grad_PolyVEF_P0P1NC_Face::readOn(Entree& s) { return s; }

void Op_Grad_PolyVEF_P0P1NC_Face::completer()
{
  Operateur_Grad_base::completer();
  const Domaine_PolyVEF_P0P1NC& domaine = ref_cast(Domaine_PolyVEF_P0P1NC, ref_domaine.valeur());
  /* initialisation des inconnues auxiliaires de la pression */
  ref_cast(Champ_Elem_PolyVEF_P0P1NC, ref_cast(Navier_Stokes_std, equation()).pression()).init_auxiliary_variables();
  /* besoin d'un joint de 1 */
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Grad_PolyVEF_P0P1NC_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
}

void Op_Grad_PolyVEF_P0P1NC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  if (!matrices.count("pression")) return; //rien a faire

  const Domaine_PolyVEF_P0P1NC& domaine = ref_cast(Domaine_PolyVEF_P0P1NC, ref_domaine.valeur());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleTab& vit = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  int i, j, e, f, fb, ne_tot = domaine.nb_elem_tot(), d, D = dimension, n, N = vit.line_size() / D, m, M = press.line_size();
  Matrice_Morse *mat = matrices["pression"], mat2;
  IntTrav sten(0, 2);
  DoubleTrav w2;
  for (f = 0; f < domaine.nb_faces(); f++)
    for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
      {
        for (d = 0; d < D; d++)
          for (n = 0, m = 0; n < N; n++, m += (M > 1))
            sten.append_line(N * (D * f + d) + n, M * e + m); /* bloc (face, elem )*/
        for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
          if (fcl(fb, 0) != 1)
            for (d = 0; d < D; d++)
              for (n = 0, m = 0; n < N; n++, m += (M > 1))
                sten.append_line(N * (D * f + d) + n, M * (ne_tot + fb) + m);
      }

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten);
  Matrix_tools::allocate_morse_matrix(vit.size_totale(), press.size_totale(), sten, mat2);
  mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
}

void Op_Grad_PolyVEF_P0P1NC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(gradient_counter_);
  const Domaine_PolyVEF_P0P1NC& dom = ref_cast(Domaine_PolyVEF_P0P1NC, ref_domaine.valeur());
  const IntTab& f_e = dom.face_voisins(), &e_f = dom.elem_faces(), &fcl = ref_cast(Champ_Face_PolyVEF, equation().inconnue()).fcl();
  const DoubleTab& vfd = dom.volumes_entrelaces_dir(), &nf = dom.face_normales(), &press = semi_impl.count("pression") ? semi_impl.at("pression") : ref_cast(Navier_Stokes_std, equation()).pression().valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const DoubleVect& fs = dom.face_surfaces(), &vf = dom.volumes_entrelaces(), &pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &ve = dom.volumes();
  int i, j, e, eb, f, fb, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = secmem.line_size() / D, m, M = press.line_size();

  Matrice_Morse *mat = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : nullptr;
  DoubleTrav w2, alpha(N); //matrice W2 dans chaque element, taux de vide a la face

  for (e = 0; e < ne_tot; e++)
    for (dom.W2(nullptr, e, w2), i = 0; i < w2.dimension(0); i++)
      if ((f = e_f(e, i)) < dom.nb_faces())
        {
          /* taux de vide a la face (identique a celui de Masse_PolyVEF_P0P1NC_Face) */
          double prefac = pe(e) * vfd(f, e != f_e(f, 0)) * (e == f_e(f, 0) ? 1 : -1) / (fs(f) * fs(f)),
                 prefac2 = fcl(f, 0) < (Option_PolyVEF::sym_as_diri ? 3 : 2) ? pf(f) * vfd(f, e != f_e(f, 0)) / ve(e) : 0, scal; /* ponderation pour elimner p_f si on est en TPFA */
          for (alpha = 0, j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
            for (n = 0; n < N; n++)
              alpha(n) += vfd(f, j) * (alp ? (*alp)(eb, n) : 1) / vf(f);
          for (j = 0; j < w2.dimension(1); j++)
            for (fb = e_f(e, j), scal = dom.dot(&nf(f, 0), &nf(fb, 0)) / (fs(f) * fs(f)), n = 0, m = 0; n < N; n++, m += (M > 1))
              {
                double fac = alpha(n) * w2(i, j, 0) * prefac, fac2 = alpha(n) * prefac2 * (e == f_e(fb, 0) ? 1 : -1);
                for (d = 0; d < D; d++)
                  {
                    secmem(f, N * d + n) -= fac * nf(f, d) * (press(ne_tot + fb, m) - press(e, m)) + fac2 * (nf(fb, d) - scal * nf(f, d)) * press(ne_tot + fb, m);
                    if (mat)
                      (*mat)(N * (D * f + d) + n, M * e + m) -= fac * nf(f, d);
                    if (mat && fcl(fb, 0) != 1)
                      (*mat)(N * (D * f + d) + n, M * (ne_tot + fb) + m) += fac * nf(f, d) + fac2 * (nf(fb, d) - scal * nf(f, d)); /* bloc (face, face) */
                  }
              }
        }
  statistiques().end_count(gradient_counter_);
}
