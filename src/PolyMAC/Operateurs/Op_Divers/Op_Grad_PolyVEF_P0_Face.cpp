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

#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Masse_PolyVEF_Face.h>
#include <Champ_Elem_PolyVEF_P0.h>
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

Implemente_instanciable(Op_Grad_PolyVEF_P0_Face, "Op_Grad_PolyVEF_P0_Face", Op_Grad_PolyMAC_P0_Face);

Sortie& Op_Grad_PolyVEF_P0_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Grad_PolyVEF_P0_Face::readOn(Entree& s) { return s; }

void Op_Grad_PolyVEF_P0_Face::completer()
{
  Operateur_Grad_base::completer();
  const Domaine_PolyMAC& dom = ref_domaine.valeur();
  /* besoin d'un joint de 1 */
  if (dom.domaine().nb_joints() && dom.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Grad_PolyVEF_P0_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  ref_cast(Champ_Elem_PolyVEF_P0, ref_cast(Navier_Stokes_std, equation()).pression()).init_auxiliary_variables();
  last_gradp_ = -DBL_MAX;
}

void Op_Grad_PolyVEF_P0_Face::update_grad(int full_stencil) const
{
  const Domaine_PolyVEF& dom = ref_cast(Domaine_PolyVEF, ref_domaine.valeur());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const DoubleTab& press = le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs(), *alp =
                             sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const int M = press.line_size();
  double t_past = equation().inconnue().recuperer_temps_passe();
  if (!full_stencil && (alp ? (last_gradp_ >= t_past) : (last_gradp_ != -DBL_MAX)))
    return; //deja calcule a ce temps -> rien a faire

  /* gradient */
  dom.fgrad(M, 1, 1, ref_dcl->les_conditions_limites(), ch.fcl(), nullptr, nullptr, 1, full_stencil, fgrad_d, fgrad_e, fgrad_c);
  last_gradp_ = t_past;
}

void Op_Grad_PolyVEF_P0_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = ref_domaine.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const IntTab& fcl = ch.fcl();
  int i, e, f, fb = 0, ne_tot = dom.nb_elem_tot(),
               nf_tot = dom.nb_faces_tot(), nfb_tot = dom.nb_faces_bord_tot(), d,
               D = dimension, n, N = ch.valeurs().line_size() / D, m,
               M = ( le_champ_inco.non_nul() ? le_champ_inco->valeurs() :
                     ref_cast(Navier_Stokes_std, equation()).pression().valeurs()).line_size();
  update_grad(sub_type(Pb_Multiphase, equation().probleme())); //provoque le calcul du gradient

  IntTrav sten(0, 2); //stencil (NS, pression)

  Matrice_Morse *mat = matrices["pression"], mat2;

  /* aux faces : gradient aux faces + remplissage de dgp_pb */
  for (f = 0; f < dom.nb_faces(); f++)
    for (i = fgrad_d(f); i < fgrad_d(f + 1); i++)
      if ((e = fgrad_e(i)) < ne_tot || fcl(fb = e - ne_tot, 0) > 1)
        for (d = 0; d < D; d++)
          for (n = 0, m = 0; n < N; n++, m += (M > 1))
            sten.append_line(N * (D * f + d) + n, M * (e < ne_tot ? e : ne_tot + dom.fbord(fb)) + m);

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten);
  if (mat)
    Matrix_tools::allocate_morse_matrix(N * D * nf_tot, M * (ne_tot + nfb_tot), sten, mat2), mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
}

void Op_Grad_PolyVEF_P0_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC& dom = ref_domaine.valeur();
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const Conds_lim& cls = ref_dcl->les_conditions_limites();
  const IntTab& f_e = dom.face_voisins(), &fcl = ch.fcl();
  const DoubleTab& vfd = dom.volumes_entrelaces_dir(),
                   &press = semi_impl.count("pression") ? semi_impl.at("pression") :
                            (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()), *alp =
                              sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const DoubleVect& pf = equation().milieu().porosite_face();
  int i, e, f, fb, ne_tot = dom.nb_elem_tot(), d, D = dimension, n, N = secmem.line_size() / D, m, M = press.line_size();
  update_grad();
  Matrice_Morse *mat = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : nullptr;

  DoubleTrav a_v(N); //(grad p)_f, produit alpha * vol

  /* aux faces */
  for (f = 0; f < dom.nb_faces(); f++)
    {
      if (fgrad_d(f + 1) == fgrad_d(f))
        abort();
      for (a_v = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
        for (n = 0; n < N; n++)
          a_v(n) += vfd(f, i) * (alp ? (*alp)(e, n) : 1);

      /* |f| grad p */
      for (i = fgrad_d(f); i < fgrad_d(f + 1); i++)
        for (e = fgrad_e(i), fb = e - ne_tot, d = 0; d < D; d++)
          for (n = 0, m = 0; n < N; n++, m += (M > 1))
            {
              double fac = a_v(n) * pf(f) * fgrad_c(i, d, m);
              secmem(f, N * d + n) -= fac * (e < ne_tot || fcl(fb, 0) > 1 ? press(e < ne_tot ? e : ne_tot + dom.fbord(fb), m) : ref_cast(Neumann, cls[fcl(fb, 1)].valeur()).flux_impose(fcl(fb, 2), m));
              if (mat && (e < ne_tot || fcl(fb, 0) > 1))
                (*mat)(N * (D * f + d) + n, M * (e < ne_tot ? e : ne_tot + dom.fbord(fb)) + m) += fac;
            }
    }
}
