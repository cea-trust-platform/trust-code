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

#include <Correction_Antal_PolyVEF_P0.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <math.h>

Implemente_instanciable(Correction_Antal_PolyVEF_P0, "Correction_Antal_Face_PolyVEF_P0", Correction_Antal_PolyMAC_P0);
// XD Correction_Antal_PolyVEF_P0 Correction_Antal_PolyMAC_P0 Correction_Antal_PolyVEF_P0 1 Antal correction source term for multiphase problem

Sortie& Correction_Antal_PolyVEF_P0::printOn(Sortie& os) const
{
  return os;
}

Entree& Correction_Antal_PolyVEF_P0::readOn(Entree& is)
{
  return Correction_Antal_PolyMAC_P0::readOn(is);
}

void Correction_Antal_PolyVEF_P0::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleTab& inco = ch.valeurs();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);
  int f, k, l, d, D = dimension, N = inco.line_size() / D;
  /* faces */
  for (f = 0; f < domaine.nb_faces(); f++)
    for (d = 0; d < D; d++)
      for (int d2 = 0; d2 < D; d2++)
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++)
            stencil.append_line(N * (D * f + d) + k, N * (D * f + d2) + l);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Correction_Antal_PolyVEF_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue());
  const DoubleTab& vit = ch.valeurs(), &pvit = ch.passe(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe(),
                    &rho   = equation().milieu().masse_volumique().passe(),
                     &d_bulles = equation().probleme().get_champ("diametre_bulles").valeurs();
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& pf = equation().milieu().porosite_face(),
                    &vf = domaine.volumes_entrelaces();
  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir(),
                   &y_faces = domaine.y_faces(),
                    &n_y_faces = domaine.normale_paroi_faces();
  int D = dimension,
      N = vit.line_size()/D ,
      nf = domaine.nb_faces();

  DoubleTrav dv2(N, N), u_par(N,D), pu_par(N,D), scal_u(N), scal_pu(N) ;
  int e, f, k, d;

  double fac, a_l, rho_l, db_l, secmem_l;
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;

  for (f = 0; f < nf; f++)
    if (y_faces(f) > 1.e-10)
      {
        // Fill local velocity
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            u_par(k, d) = vit(f, N*d+k), pu_par(k,d)=pvit(f, N*d+k);

        // Retract component normal to the wall
        scal_u = 0, scal_pu=0;
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            scal_u(k) += u_par(k, d)*n_y_faces(f, d), scal_pu(k)+= pu_par(k, d)*n_y_faces(f, d);
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            u_par(k, d) -= scal_u(k)*n_y_faces(f, d), pu_par(k, d) -= scal_pu(k)*n_y_faces(f, d) ;

        // Calculation of liquid-gas velocity difference
        dv2 = 0.;
        for ( k = 0; k < N; k++)
          if (k != n_l)
            for (d = 0 ; d<D ; d++)  dv2(k, n_l) += ( 2* (u_par(k, d)-u_par(n_l, d)) - (pu_par(k, d)-pu_par(n_l, d))) * (pu_par(k, d)-pu_par(n_l, d));

        for (k = 0; k < N; k++)
          if (k != n_l)
            {
              fac = pf(f) * vf(f) ;
              a_l = (    alpha(f_e(f, 0), k)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? alpha(e, k)*vf_dir(f,1) : 0)  ) / vf(f);
              rho_l=(    rho(f_e(f, 0), n_l)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? rho(e, n_l)*vf_dir(f,1) : 0)  ) / vf(f);
              db_l= ( d_bulles(f_e(f, 0), k)*vf_dir(f,0) +    ((e = f_e(f, 1))>0 ? d_bulles(e, k)*vf_dir(f,1):0) ) / vf(f);

              for (d=0 ; d<D ; d++)
                {
                  double fac2 = fac * n_y_faces(f, d) * 2. * a_l * rho_l / db_l * std::max(0., Cw1_ + Cw2_*db_l/(2.*y_faces(f))) ;
                  secmem_l = fac2 * dv2(k, n_l);//std::min(dv2(k, n_l), 1.);
                  secmem(f, N*d+k)   += secmem_l;
                  secmem(f, N*d+n_l) -= secmem_l;
                  if (mat)
                    for (int d2=0 ; d2<D ; d2++)
                      {
                        (*mat)(N * (D * f + d) + k, N * (D * f + d2) + k) -= fac2 * 2* (pu_par(k, d2)-pu_par(n_l, d2));
                        (*mat)(N * (D * f + d) + k, N * (D * f + d2) +n_l)+= fac2 * 2* (pu_par(k, d2)-pu_par(n_l, d2));
                        (*mat)(N * (D * f + d) +n_l,N * (D * f + d2) + k) += fac2 * 2* (pu_par(k, d2)-pu_par(n_l, d2));
                        (*mat)(N * (D * f + d) +n_l,N * (D * f + d2) +n_l)-= fac2 * 2* (pu_par(k, d2)-pu_par(n_l, d2));
                        for (int d3=0 ; d3<D ; d3++)
                          {
                            (*mat)(N * (D * f + d) + k, N * (D * f + d3) + k) -= fac2 * 2 * (-n_y_faces(f, d3)*n_y_faces(f, d2) )*(pu_par(k, d2)-pu_par(n_l, d2));
                            (*mat)(N * (D * f + d) + k, N * (D * f + d3) +n_l)+= fac2 * 2 * (-n_y_faces(f, d3)*n_y_faces(f, d2) )*(pu_par(k, d2)-pu_par(n_l, d2));
                            (*mat)(N * (D * f + d) +n_l,N * (D * f + d3) + k) += fac2 * 2 * (-n_y_faces(f, d3)*n_y_faces(f, d2) )*(pu_par(k, d2)-pu_par(n_l, d2));
                            (*mat)(N * (D * f + d) +n_l,N * (D * f + d3) +n_l)-= fac2 * 2 * (-n_y_faces(f, d3)*n_y_faces(f, d2) )*(pu_par(k, d2)-pu_par(n_l, d2));
                          }
                      }
                }
            }
      }
}
