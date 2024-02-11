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

#include <DP_Impose_PolyVEF_Face.h>
#include <Domaine_PolyVEF.h>
#include <Champ_Face_base.h>
#include <Champ_Don_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Synonyme_info.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Milieu_base.h>
#include <cfloat>

Implemente_instanciable(DP_Impose_PolyVEF_Face, "DP_Impose_Face_PolyVEF_P0", DP_Impose_PolyMAC_Face);
Add_synonym(DP_Impose_PolyVEF_Face, "DP_Impose_Face_PolyVEF_P0P1");
Add_synonym(DP_Impose_PolyVEF_Face, "DP_Impose_Face_PolyVEF_P0P1NC");

Sortie& DP_Impose_PolyVEF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& DP_Impose_PolyVEF_Face::readOn(Entree& s) { return DP_Impose_PolyMAC_Face::readOn(s); }

void DP_Impose_PolyVEF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyVEF& dom = ref_cast(Domaine_PolyVEF, equation().domaine_dis());

  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  IntTrav sten(0, 2);

  int i, f, d, db, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D, nf_tot = dom.nb_faces_tot();
  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < dom.nb_faces())
      for (d = 0; d < D; d++)
        for (db = 0; db < D; db++)
          for (n = 0; n < N; n++)
            sten.append_line(N * (D * f + d) + n, N * (D * f + db) + n);

  tableau_trier_retirer_doublons(sten);
  Matrix_tools::allocate_morse_matrix(N * D * nf_tot, N * D * nf_tot, sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void DP_Impose_PolyVEF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& dom = ref_cast(Domaine_Poly_base, equation().domaine_dis());
  const DoubleVect& pf = equation().milieu().porosite_face(), &fs = dom.face_surfaces(), &vf = dom.volumes_entrelaces();
  const DoubleTab& vit = equation().inconnue().valeurs(), *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr,
                   &vfd = dom.volumes_entrelaces_dir(), &nf = dom.face_normales();
  const IntTab& f_e = dom.face_voisins(), &fcl = ref_cast(Champ_Face_base, equation().inconnue()).fcl();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
  int i, j, e, f, d, db, D = dimension, n, N = vit.line_size() / D, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);

  double rho = equation().milieu().masse_volumique().valeurs()(0, 0),
         fac_rho = (equation().probleme().is_dilatable() || sub_type(Pb_Multiphase, equation().probleme())) ? 1.0 : 1.0 / rho;

  if (regul_)
    {
      for (i = 0; i < num_faces.size(); i++)
        if ((f = num_faces(i)) < dom.nb_faces())
          secmem(f) += fs(f) * pf(f) * sgn(i) * dp_regul_ * fac_rho;
    }
  else
    {
      DoubleTrav xvf(num_faces.size(), dimension), DP(num_faces.size(), 3), vn(N), a_f(N);
      for (i = 0; i < num_faces.size(); i++)
        for (j = 0; j < dimension; j++)
          xvf(i, j) = dom.xv()(num_faces(i), j);
      DP_.valeur().valeur_aux(xvf, DP);

      for (a_f = 1, i = 0; i < num_faces.size(); i++)
        if ((f = num_faces(i)) < dom.nb_faces() && (!p0p1 || fcl(f, 0) < 2))
          {
            if (alp)
              for (a_f = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
                for (n = 0; n < N; n++)
                  a_f(n) += vfd(f, i) / vf(f) * (*alp)(e, n);

            for (vn = 0, d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                vn(n) += nf(f, d) * vit(f, N * d + n) / fs(f);

            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                secmem(f, N * d + n) += a_f(n) * nf(f, d) * pf(f) * sgn(i) * (DP(i, 0) + DP(i, 1) * (surf * sgn(i) * vn(n) - DP(i, 2))) * fac_rho;
            if (mat)
              for (d = 0; d < D; d++)
                for (db = 0; db < D; db++)
                  for (n = 0; n < N; n++)
                    (*mat)(N * (D * f + d) + n, N * (D * f + db) + n) -= a_f(n) * nf(f, d) * nf(f, db) / fs(f) * pf(f) * DP(i, 1) * surf * fac_rho;
          }

      bilan()(0) = Process::mp_max(num_faces.size() ? DP(0, 0) : -DBL_MAX);
      bilan()(1) = Process::mp_max(num_faces.size() ? DP(0, 1) / rho : -DBL_MAX);
      bilan()(3) = Process::mp_max(num_faces.size() ? DP(0, 2) * rho : -DBL_MAX);
      if (Process::me())
        bilan() = 0; //pour eviter un sommage en sortie
    }
}
