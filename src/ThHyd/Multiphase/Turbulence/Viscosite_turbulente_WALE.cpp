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

#include <Viscosite_turbulente_WALE.h>
#include <Discretisation_base.h>
#include <Champ_Face_base.h>
#include <Pb_Multiphase.h>
#include <Domaine_VF.h>
#include <Param.h>

Implemente_instanciable(Viscosite_turbulente_WALE, "Viscosite_turbulente_WALE", Viscosite_turbulente_base);

Sortie& Viscosite_turbulente_WALE::printOn(Sortie& os) const { return os; }

Entree& Viscosite_turbulente_WALE::readOn(Entree& is)
{
  mod_const_ = 0.5; // par default
  Param param(que_suis_je());
  param.ajouter("cw", &mod_const_);
  param.lire_avec_accolades_depuis(is);

  if (mod_const_ < 0.) Process::exit("The WALE's constant must be positive !");
  else Cerr << "LES WALE model used with constant CW = " << mod_const_ << finl;

  return Viscosite_turbulente_LES_base::readOn(is);
}

/*
 *                                               (Sij_d * Sij_d)^1.5
 *    visc_SGS = (Cw * delta)^2 * ---------------------------------------------------,
 *                                  (Sij_bar * Sij_bar)^2.5 + (Sij_d * Sij_d)^1.25
 *
 *    avec
 *
 *        Sij_d = 0.5 * (gij_bar^2 + gji_bar^2) -1/3 gkk_bar^2,
 *        gij_bar = du_i / dx_j,
 *        gij_bar^2 = gik_bar * gkj_bar,
 *        Sij_bar = 0.5 * (du_i / dx_j + du_j / dx_i)
 */
void Viscosite_turbulente_WALE::eddy_viscosity(DoubleTab& nu_t) const
{
  if (est_egal(mod_const_, 0., 1.e-15)) nu_t = 0.;
  else
    {
      const Domaine_VF& domaine_VF = ref_cast(Domaine_VF, pb_->domaine_dis().valeur());
      const Champ_Face_base& vit = ref_cast(Champ_Face_base, pb_->equation(0).inconnue().valeur());

      // Nota bene : en PolyMAC_P0, grad_u__ contient (nf.grad)u_i aux faces, puis (d_j u_i) aux elements
      const DoubleTab& grad_u__ = pb_->get_champ("gradient_vitesse").valeurs();

      const IntTab& face_voisins = domaine_VF.face_voisins(), &elem_faces = domaine_VF.elem_faces();
      const int nb_elem_tot = domaine_VF.nb_elem_tot(), dim = Objet_U::dimension, N = vit.valeurs().line_size();
      const bool is_poly = pb_->discretisation().is_polymac_family();
      const int nb_faces_tot = is_poly ? domaine_VF.nb_faces_tot() : 0;
      assert (N == nu_t.dimension(1));

      DoubleTrav num_sur_denom(nb_elem_tot, nu_t.dimension(1));
      num_sur_denom = 0.;

      DoubleTrav grad_u(nb_elem_tot, dim, dim, N), gij_bar2(dim, dim, N), Sij_d(dim, dim, N);
      grad_u = 0., gij_bar2 = 0., Sij_d = 0.;

      // remplir grad_u en 4 dimension
      // XXX : attention faut remplir grad_u ici pas plus tard !!!
      for (int elem = 0; elem < nb_elem_tot; elem++)
        for (int i = 0; i < dim; i++) // variable : du, dv, dw
          for (int j = 0; j < dim; j++) // par rappor a : dx, dy, dz
            for (int n = 0; n < N; n++) // phase
              grad_u(elem, i, j, n) = is_poly ? grad_u__(nb_faces_tot + elem * dim + j, n * dim + i) : grad_u__(nb_faces_tot + elem, N * (dim * i + j) + n);

      // Arrays utiles
      ArrOfDouble gkk_bar2(N), Sij_d2(N), Sij_bar(N), Sij_bar2(N);;

      for (int elem = 0; elem < nb_elem_tot; elem++)
        {
          //Calcul du terme Sij_d
          for (int i = 0; i < dim; i++)
            for (int j = 0; j < dim; j++)
              for (int n = 0; n < N; n++)
                {
                  gij_bar2(i, j, n) = 0.;
                  for (int k = 0; k < dim; k++)
                    gij_bar2(i, j, n) += grad_u(elem, i, k, n) * grad_u(elem, k, j, n);
                }

          gkk_bar2 = 0.;
          for (int k = 0; k < dim; k++)
            for (int n = 0; n < N; n++)
              gkk_bar2(n) += gij_bar2(k, k, n);

          for (int i = 0; i < dim; i++)
            for (int j = 0; j < dim; j++)
              for (int n = 0; n < N; n++)
                {
                  Sij_d(i, j, n) = 0.5 * (gij_bar2(i, j, n) + gij_bar2(j, i, n));
                  if (i == j)
                    Sij_d(i, j, n) -= gkk_bar2(n) / 3.; // Terme derriere le tenseur de Kronecker
                }

          // Calcul de Sij_d2 et Sij_bar2
          Sij_d2 = 0., Sij_bar = 0., Sij_bar2 = 0.;

          for (int i = 0; i < dim; i++)
            for (int j = 0; j < dim; j++)
              for (int n = 0; n < N; n++)
                {
                  Sij_d2(n) += Sij_d(i, j, n) * Sij_d(i, j, n);

                  // Sij_bar et Sij_bar2
                  Sij_bar(n) = 0.5 * (grad_u(elem, i, j, n) + grad_u(elem, j, i, n));
                  if (i == j)
                    {
                      const int face1 = elem_faces(elem, i), face2 = elem_faces(elem, i + dim), elem1 = face_voisins(face1, 0), elem2 = face_voisins(face2, 1);

                      if (elem1 >= 0 && elem2 >= 0)
                        Sij_bar(n) = ((grad_u(elem1, i, i, 0) + grad_u(elem, i, i, 0) + grad_u(elem2, i, i, 0))) / 3.;
                    }
                  Sij_bar2(n) += Sij_bar(n) * Sij_bar(n);
                }

          for (int n = 0; n < N; n++)
            {
              const double num = pow(Sij_d2(n), 1.5), denom = pow(Sij_bar2(n), 2.5) + pow(Sij_d2(n), 1.25);
              num_sur_denom(elem, n) = num != 0. ? (num / denom) : 0.;
            }
        }

      for (int i = 0; i < nu_t.dimension_tot(0); i++)
        for (int n = 0; n < nu_t.dimension(1); n++)
          nu_t(i, n) = mod_const_ * mod_const_ * l_(i) * l_(i) * num_sur_denom(i, n);
    }
}
