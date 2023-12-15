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

#include <Perte_Charge_Singuliere_PolyVEF_P0_Face.h>
#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Domaine_PolyMAC_P0.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Matrice_Morse.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Domaine.h>

Implemente_instanciable(Perte_Charge_Singuliere_PolyVEF_P0_Face, "Perte_Charge_Singuliere_Face_PolyVEF_P0", Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face);

Sortie& Perte_Charge_Singuliere_PolyVEF_P0_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Perte_Charge_Singuliere_PolyVEF_P0_Face::readOn(Entree& s) { return Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face::readOn(s); }

void Perte_Charge_Singuliere_PolyVEF_P0_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = ref_cast(Domaine_Poly_base, equation().domaine_dis().valeur());
  const DoubleTab& vit = la_vitesse->valeurs();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  if (! matrices.count(nom_inco)) return;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;
  int i, f, d, db, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D;

  IntTrav sten(0, 2);
  sten.set_smart_resize(1);

  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < domaine.nb_faces())
      for (d = 0; d < D; d++)
        for (db = 0; db < D; db++)
          for (n = 0; n < N; n++)
            sten.append_line(N * (D * f + d) + n, N * (D * f + db) + n);
  tableau_trier_retirer_doublons(sten);
  Matrix_tools::allocate_morse_matrix(vit.size_totale(), vit.size_totale(), sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Perte_Charge_Singuliere_PolyVEF_P0_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = ref_cast(Domaine_Poly_base, equation().domaine_dis());
  //const DoubleVect& volumes_entrelaces = domaine_PolyVEF_P0.volumes_entrelaces();
  const DoubleVect& pf = equation().milieu().porosite_face(), &fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const DoubleTab& vit = la_vitesse->valeurs(), &nf = domaine.face_normales(), &vfd = domaine.volumes_entrelaces_dir(),
                   *alpha = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr, *a_r = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr;
  const IntTab& f_e = domaine.face_voisins();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
  int i, j, e, f, d, db, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D;
  DoubleTrav aar_f(N), vn(N); //alpha * alpha * rho a chaque face
  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < domaine.nb_faces())
      {
        double fac = (direction_perte_charge() < 0 ? fs(f) : std::fabs(nf(f,direction_perte_charge()))) * pf(f) * K() / D;
        if (pbm)
          for (aar_f = 0, j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++)
            for (n = 0; n < N; n++) aar_f(n) += vfd(f, j) / vf(f) * (*a_r)(e, n) * (*alpha)(e, n);
        else aar_f = 1;
        for (vn = 0, d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            vn(n) += vit(f, N * d + n) * nf(f, d) / fs(f);

        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            secmem(f, N * d + n) -= 0.5 * fac * aar_f(n) * std::abs(vn(n)) * vn(n) * nf(f, d) / fs(f);
        if (mat)
          for (d = 0; d < D; d++)
            for (db = 0; db < D; db++)
              for (n = 0; n < N; n++)
                (*mat)(N * (D * f + d) + n, N * (D * f + db) + n) += fac * aar_f(n) * std::abs(vn(n)) * nf(f, d) / fs(f) * nf(f, db) / fs(f);
      }
}
