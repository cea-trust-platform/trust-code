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

#include <Op_Grad_PolyMAC_P0P1NC_Face.h>
#include <Champ_Elem_PolyMAC_P0P1NC.h>
#include <Masse_PolyMAC_P0P1NC_Face.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
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

extern Stat_Counter_Id gradient_counter_;

Implemente_instanciable(Op_Grad_PolyMAC_P0P1NC_Face, "Op_Grad_PolyMAC_P0P1NC_Face", Op_Grad_PolyMAC_Face);

Sortie& Op_Grad_PolyMAC_P0P1NC_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Grad_PolyMAC_P0P1NC_Face::readOn(Entree& s) { return s; }

void Op_Grad_PolyMAC_P0P1NC_Face::completer()
{
  Operateur_Grad_base::completer();
  const Domaine_PolyMAC_P0P1NC& domaine = ref_cast(Domaine_PolyMAC_P0P1NC, ref_domaine.valeur());

  /* initialisation des inconnues auxiliaires de la pression... */
  ref_cast(Champ_Elem_PolyMAC_P0P1NC, ref_cast(Navier_Stokes_std, equation()).pression()).init_auxiliary_variables();

  /* et de grad P si la vitesse en a */
  if (equation().inconnue().valeurs().get_md_vector() == domaine.mdv_faces_aretes)
    if (ref_cast(Navier_Stokes_std, equation()).has_grad_P())
      ref_cast(Champ_Face_PolyMAC_P0P1NC, ref_cast(Navier_Stokes_std, equation()).grad_P()).init_auxiliary_variables();

  /* besoin d'un joint de 1 */
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Grad_PolyMAC_P0P1NC_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
}

void Op_Grad_PolyMAC_P0P1NC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  if (!matrices.count("pression")) return; //rien a faire

  const Domaine_PolyMAC_P0P1NC& domaine = ref_cast(Domaine_PolyMAC_P0P1NC, ref_domaine.valeur());
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue());
  const IntTab& e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& vit = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  const int ne_tot = domaine.nb_elem_tot(), N = vit.line_size(), M = press.line_size();
  Matrice_Morse *mat = matrices["pression"], mat2;
  IntTab sten(0, 2);
  DoubleTrav w2;

  for (int e = 0; e < ne_tot; e++)
    {
      domaine.W2(nullptr, e, w2);

      for (int i = 0; i < w2.dimension(0); i++)
        {
          const int f = e_f(e, i);

          if (f < domaine.nb_faces()) /* faces reelles seulement */
            {
              int m = 0;
              for (int n = 0; n < N; n++, m += (M > 1))
                sten.append_line(N * f + n, M * e + m); /* bloc (face, elem )*/

              for (int j = 0; j < w2.dimension(1); j++)
                {
                  const int fb = e_f(e, j);
                  if (fcl(fb, 0) != 1 && w2(i, j, 0)) /* bloc (face, face) */
                    {
                      m = 0;
                      for (int n = 0; n < N; n++, m += (M > 1))
                        sten.append_line(N * f + n, M * (ne_tot + fb) + m);
                    }
                }
            }
        }
    }

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten);
  Matrix_tools::allocate_morse_matrix(vit.size_totale(), press.size_totale(), sten, mat2);

  if (mat->nb_colonnes())
    *mat += mat2;
  else
    *mat = mat2;
}

void Op_Grad_PolyMAC_P0P1NC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(gradient_counter_);
  const Domaine_PolyMAC_P0P1NC& domaine = ref_cast(Domaine_PolyMAC_P0P1NC, ref_domaine.valeur());
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(),
                &fcl = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue()).fcl();

  const DoubleTab& vfd = domaine.volumes_entrelaces_dir(),
                   &press = semi_impl.count("pression") ? semi_impl.at("pression") : ref_cast(Navier_Stokes_std, equation()).pression().valeurs(),
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;

  const DoubleVect& fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces(), &pe = equation().milieu().porosite_elem();
  const int ne_tot = domaine.nb_elem_tot(), N = secmem.line_size(), M = press.line_size();

  Matrice_Morse *mat = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : nullptr;

  DoubleTrav w2, alpha(N), coeff_e(N); //matrice W2 dans chaque element, taux de vide a la face

  for (int e = 0; e < ne_tot; e++)
    {
      domaine.W2(nullptr, e, w2);
      for (int i = 0; i < w2.dimension(0); i++)
        {
          const int f = e_f(e, i);

          if (f < domaine.nb_faces())
            {
              /* taux de vide a la face (identique a celui de Masse_PolyMAC_P0P1NC_Face) */
              double prefac = (e == f_e(f, 0) ? 1 : -1) * pe(e) * vfd(f, e != f_e(f, 0)) / fs(f); /* ponderation pour elimner p_f si on est en TPFA */

              alpha = 0.;

              for (int j = 0; j < 2; j++)
                {
                  const int eb = f_e(f, j);
                  if (eb < 0) continue;

                  for (int n = 0; n < N; n++)
                    alpha(n) += vfd(f, j) * (alp ? (*alp)(eb, n) : 1) / vf(f);
                }

              coeff_e = 0.;

              for (int j = 0; j < w2.dimension(1); j++)
                if (w2(i, j, 0))
                  {
                    const int fb = e_f(e, j);
                    int m = 0;

                    for (int n = 0; n < N; n++, m += (M > 1))
                      {
                        const double fac = alpha(n) * w2(i, j, 0) * prefac;
                        secmem(f, n) -= fac * (press(ne_tot + fb, m) - press(e, m));

                        if (mat && fcl(fb, 0) != 1)
                          (*mat)(N * f + n, M * (ne_tot + fb) + m) += fac; /* bloc (face, face) */

                        coeff_e(n) += fac;
                      }
                  }

              if (mat)
                {
                  int m = 0;
                  for (int n = 0; n < N; n++, m += (M > 1))
                    (*mat)(N * f + n, M * e + m) -= coeff_e(n); /* bloc (face, elem) */
                }
            }
        }
    }
  statistiques().end_count(gradient_counter_);
}
