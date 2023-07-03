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

#include <Op_Grad_PolyMAC_P0P1NC_Face.h>
#include <Champ_Elem_PolyMAC_P0P1NC.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Neumann_sortie_libre.h>

#include <Dirichlet.h>

#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Check_espace_virtuel.h>
#include <communications.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <TRUSTTrav.h>
#include <Masse_PolyMAC_P0P1NC_Face.h>
#include <Milieu_base.h>
#include <Pb_Multiphase.h>
#include <cfloat>

Implemente_instanciable(Op_Grad_PolyMAC_P0P1NC_Face,"Op_Grad_PolyMAC_P0P1NC_Face",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_PolyMAC_P0P1NC_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_PolyMAC_P0P1NC_Face::readOn(Entree& s)
{
  return s ;
}



/*! @brief
 *
 */
void Op_Grad_PolyMAC_P0P1NC_Face::associer(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis, const Champ_Inc& ch)
{
  ref_domaine = ref_cast(Domaine_PolyMAC_P0P1NC, domaine_dis.valeur());
  ref_zcl = ref_cast(Domaine_Cl_PolyMAC, domaine_cl_dis.valeur());
}

void Op_Grad_PolyMAC_P0P1NC_Face::completer()
{
  Operateur_Grad_base::completer();
  const Domaine_PolyMAC_P0P1NC& domaine = ref_domaine.valeur();
  /* initialisation des inconnues auxiliaires de la pression... */
  ref_cast(Champ_Elem_PolyMAC_P0P1NC, ref_cast(Navier_Stokes_std, equation()).pression().valeur()).init_auxiliary_variables();
  /* et de grad P si la vitesse en a */
  if (equation().inconnue().valeurs().get_md_vector() == domaine.mdv_faces_aretes)
    if (ref_cast(Navier_Stokes_std, equation()).grad_P().non_nul())
      ref_cast(Champ_Face_PolyMAC_P0P1NC, ref_cast(Navier_Stokes_std, equation()).grad_P().valeur()).init_auxiliary_variables();
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
  const Domaine_PolyMAC_P0P1NC& domaine = ref_domaine.valeur();
  const Champ_Face_PolyMAC_P0P1NC& ch = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue().valeur());
  const IntTab& e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& vit = ch.valeurs(), &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs();
  int i, j, e, f, fb, ne_tot = domaine.nb_elem_tot(), n, N = vit.line_size(), m, M = press.line_size();
  Matrice_Morse *mat = matrices["pression"], mat2;
  IntTrav sten(0, 2);
  DoubleTrav w2;
  sten.set_smart_resize(1), w2.set_smart_resize(1);
  for (e = 0; e < ne_tot; e++)
    for (domaine.W2(NULL, e, w2), i = 0; i < w2.dimension(0); i++)
      if ((f = e_f(e, i)) < domaine.nb_faces()) /* faces reelles seulement */
        {
          for (n = 0, m = 0; n < N; n++, m += (M > 1)) sten.append_line(N * f + n, M * e + m); /* bloc (face, elem )*/
          for (j = 0; j < w2.dimension(1); j++)
            if (fcl(fb = e_f(e, j), 0) != 1 && w2(i, j, 0)) /* bloc (face, face) */
              for (n = 0, m = 0; n < N; n++, m += (M > 1)) sten.append_line(N * f + n, M * (ne_tot + fb) + m);
        }

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten);
  Matrix_tools::allocate_morse_matrix(vit.size_totale(), press.size_totale(), sten, mat2);
  mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
}

void Op_Grad_PolyMAC_P0P1NC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC_P0P1NC& domaine = ref_domaine.valeur();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue().valeur()).fcl();
  const DoubleTab& vfd = domaine.volumes_entrelaces_dir(), &press = semi_impl.count("pression") ? semi_impl.at("pression") : ref_cast(Navier_Stokes_std, equation()).pression().valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  const DoubleVect& fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces(), &pe = equation().milieu().porosite_elem();
  int i, j, e, eb, f, fb, ne_tot = domaine.nb_elem_tot(), n, N = secmem.line_size(), m, M = press.line_size();

  Matrice_Morse *mat = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : NULL;
  DoubleTrav w2, alpha(N), coeff_e(N); //matrice W2 dans chaque element, taux de vide a la face
  w2.set_smart_resize(1);

  for (e = 0; e < ne_tot; e++)
    for (domaine.W2(NULL, e, w2), i = 0; i < w2.dimension(0); i++)
      if ((f = e_f(e, i)) < domaine.nb_faces())
        {
          /* taux de vide a la face (identique a celui de Masse_PolyMAC_P0P1NC_Face) */
          double prefac = (e == f_e(f, 0) ? 1 : -1) * pe(e) * vfd(f, e != f_e(f, 0)) / fs(f); /* ponderation pour elimner p_f si on est en TPFA */
          for (alpha = 0, j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
            for (n = 0; n < N; n++) alpha(n) += vfd(f, j) * (alp ? (*alp)(eb, n) : 1) / vf(f);
          for (coeff_e = 0, j = 0; j < w2.dimension(1); j++)
            if (w2(i, j, 0))
              for (fb = e_f(e, j), n = 0, m = 0; n < N; n++, m += (M > 1))
                {
                  double fac = alpha(n) * w2(i, j, 0) * prefac;
                  secmem(f, n) -= fac * (press(ne_tot + fb, m) - press(e, m));
                  if (mat && fcl(fb, 0) != 1) (*mat)(N * f + n, M * (ne_tot + fb) + m) += fac; /* bloc (face, face) */
                  coeff_e(n) += fac;
                }
          if (mat)
            for (n = 0, m = 0; n < N; n++, m += (M > 1)) (*mat)(N * f + n, M * e + m) -= coeff_e(n); /* bloc (face, elem) */
        }
}

int Op_Grad_PolyMAC_P0P1NC_Face::impr(Sortie& os) const
{
  return 0;
}
