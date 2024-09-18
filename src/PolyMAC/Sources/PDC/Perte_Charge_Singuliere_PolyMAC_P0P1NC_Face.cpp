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

#include <Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face.h>
#include <Op_Grad_PolyMAC_P0P1NC_Face.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Domaine_PolyMAC_P0.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Matrice_Morse.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Domaine.h>

Implemente_instanciable(Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face, "Perte_Charge_Singuliere_Face_PolyMAC_P0P1NC|Perte_Charge_Singuliere_Face_PolyMAC_P0", Perte_Charge_Singuliere_PolyMAC_Face);

Sortie& Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face::readOn(Entree& s) { return Perte_Charge_Singuliere_PolyMAC_Face::readOn(s); }

void Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face::completer()
{
  Perte_Charge_PolyMAC_Face::completer();
  // eq_masse besoin de champ_conserve !
  if (sub_type(Pb_Multiphase, mon_equation->probleme())) ref_cast(Pb_Multiphase, mon_equation->probleme()).equation_masse().init_champ_conserve();
}

void Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = ref_cast(Domaine_Poly_base, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue()).fcl();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inco) || !sub_type(Domaine_PolyMAC_P0, domaine)) return;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;
  int i, j, e, f, n, N = equation().inconnue().valeurs().line_size(), d, D = dimension, nf_tot = domaine.nb_faces_tot();
  //DoubleTrav aar_f(N); //alpha * alpha * rho a chaque face
  IntTrav stencil(0, 2);


  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < domaine.nb_faces() && fcl(f, 0) < 2)
      for (j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++) //elem amont / aval si PolyMAC_P0P1NC V2
        if (e < domaine.nb_elem())
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++) stencil.append_line(N * (nf_tot + D * e + d) + n, N * f + n);
  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(equation().inconnue().valeurs().size_totale(), equation().inconnue().valeurs().size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Perte_Charge_Singuliere_PolyMAC_P0P1NC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = ref_cast(Domaine_Poly_base, equation().domaine_dis());
  //const DoubleVect& volumes_entrelaces = domaine_PolyMAC_P0P1NC.volumes_entrelaces();
  const DoubleVect& pf = equation().milieu().porosite_face(), &fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const DoubleTab& vit = la_vitesse->valeurs(), &nf = domaine.face_normales(), &vfd = domaine.volumes_entrelaces_dir(), &xv = domaine.xv(), &xp = domaine.xp(),
                   *alpha = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr, *a_r = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr;
  const IntTab& f_e = domaine.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC_P0P1NC, equation().inconnue()).fcl();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
  int i, j, e, f, n, N = equation().inconnue().valeurs().line_size();
  int poly_v2 = sub_type(Domaine_PolyMAC_P0, domaine), semi = (int)semi_impl.count(nom_inco), d, D = dimension, nf_tot = domaine.nb_faces_tot();
  DoubleTrav aar_f(N); //alpha * alpha * rho a chaque face
  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < domaine.nb_faces())
      {
        double fac = (direction_perte_charge() < 0 ? fs(f) : std::fabs(nf(f,direction_perte_charge()))) * pf(f) * K();
        if (pbm)
          for (aar_f = 0, j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++)
            for (n = 0; n < N; n++) aar_f(n) += vfd(f, j) / vf(f) * (*a_r)(e, n) * (*alpha)(e, n);
        else aar_f = 1;

        if (!poly_v2 || fcl(f, 0) < 2) //contrib a la face : sauf si face de Dirichlet/Neumann en V2
          {
            for (n = 0; n < N; n++)  secmem(f, n) -= 0.5 * fac * aar_f(n) * vit(f, n) * std::fabs(vit(f, n));
            if (mat)
              for (n = 0; n < N; n++) (*mat)(N * f + n, N * f + n) += fac * aar_f(n) * std::fabs(vit(f, n));
          }
        if (poly_v2)
          for (j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++)
            if (e < domaine.nb_elem()) //elem amont / aval si PolyMAC_P0P1NC V2
              {
                for (d = 0; d < D; d++)
                  for (n = 0; n < N; n++) /* second membre */
                    secmem(nf_tot + D * e + d, n) += (j ? -1 : 1) * fs(f) * (xv(f, d) - xp(e, d)) * 0.5 * fac * aar_f(n) * vit(f, n) * std::fabs(vit(f, n));
                if (mat && !semi && fcl(f, 0) < 2)
                  for (d = 0; d < D; d++)
                    for (n = 0; n < N; n++) /* derivee (pas possible en semi-implicite) */
                      (*mat)(N * (nf_tot + D * e + d) + n, N * f + n) -= (j ? -1 : 1) * fs(f) * (xv(f, d) - xp(e, d)) * fac * aar_f(n) * std::fabs(vit(f, n));
              }
      }
}
