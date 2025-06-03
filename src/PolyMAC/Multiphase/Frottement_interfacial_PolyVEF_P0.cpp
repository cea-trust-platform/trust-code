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

#include <Frottement_interfacial_PolyVEF_P0.h>
#include <Frottement_interfacial_base.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Milieu_composite.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

Implemente_instanciable(Frottement_interfacial_PolyVEF_P0, "Frottement_interfacial_Face_PolyVEF_P0", Source_Frottement_interfacial_base);

Sortie& Frottement_interfacial_PolyVEF_P0::printOn(Sortie& os) const { return os; }
Entree& Frottement_interfacial_PolyVEF_P0::readOn(Entree& is) { return Source_Frottement_interfacial_base::readOn(is); }

void Frottement_interfacial_PolyVEF_P0::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const DoubleTab& inco = ch.valeurs();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  int f, k, l, d, D = dimension, N = inco.line_size() / D;
  /* faces */
  for (f = 0; f < domaine.nb_faces(); f++)
    for (d = 0; d < D; d++)
      for (k = 0; k < N; k++)
        for (l = 0; l < N; l++)
          stencil.append_line(N * (D * f + d) + k, N * (D * f + d) + l);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Frottement_interfacial_PolyVEF_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;
  const Domaine_PolyVEF_P0& domaine = ref_cast(Domaine_PolyVEF_P0, equation().domaine_dis().valeur());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces(), &dh_e = equation().milieu().diametre_hydraulique_elem();
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(), &vfd = domaine.volumes_entrelaces_dir(),
                   &alpha = pbm.equation_masse().inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, equation()).pression().passe(),
                     &temp  = pbm.equation_energie().inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();
  ConstDoubleTab_parts ppart(press);
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  DoubleTab const * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr ;

  int e, f, c, j, k, l, n, d, D = dimension, N = inco.line_size() / D, Np = press.line_size(),
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), sigma_l(N*(N-1)/2), dv(N, N), ddv(N, N, 4), d_bulles_l(N), coeff(N, N, 2); //arguments pour coeff
  double dv_c, dh;
  const Frottement_interfacial_base& correlation_fi = ref_cast(Frottement_interfacial_base, correlation_.valeur());


  // Et pour les methodes span de la classe Saturation
  const int ne_tot = domaine.nb_elem_tot(), nb_max_sat =  N * (N-1) /2; // oui !! suite arithmetique !!
  DoubleTrav Sigma_tab(ne_tot,nb_max_sat);

  // remplir les tabs ...
  for (k = 0; k < N; k++)
    for (l = k + 1; l < N; l++)
      {
        if (milc.has_saturation(k, l))
          {
            const Saturation_base& z_sat = milc.get_saturation(k, l);
            const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
            // recuperer sigma ...
            const DoubleTab& sig = z_sat.get_sigma_tab();
            // fill in the good case
            for (int ii = 0; ii < ne_tot; ii++) Sigma_tab(ii, ind_trav) = sig(ii);
          }
        else if (milc.has_interface(k, l))
          {
            Interface_base& sat = milc.get_interface(k,l);
            const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
            const DoubleTab& sig = sat.get_sigma_tab();
            for (int ii = 0; ii < ne_tot; ii++) Sigma_tab(ii, ind_trav) = sig(ii);
          }
      }

  /* faces */
  for (f = 0; f < domaine.nb_faces(); f++)
    {
      for (a_l = 0, p_l = 0, T_l = 0, rho_l = 0, mu_l = 0, dh = 0, sigma_l = 0, d_bulles_l=0, c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
        for (n = 0; n < N; n++)
          {
            a_l(n)   += vfd(f, c) / vf(f) * alpha(e, n);
            p_l(n)   += vfd(f, c) / vf(f) * press(e, n * (Np > 1));
            T_l(n)   += vfd(f, c) / vf(f) * temp(e, n);
            rho_l(n) += vfd(f, c) / vf(f) * rho(!cR * e, n);
            mu_l(n)  += vfd(f, c) / vf(f) * mu(!cM * e, n);
            for (k = n+1; k < N; k++)
              {
                const int ind_trav = (n*(N-1)-(n-1)*(n)/2) + (k-n-1);
                sigma_l(ind_trav) += vfd(f, c) / vf(f) * Sigma_tab(e, ind_trav);
              }
            dh += vfd(f, c) / vf(f) * alpha(e, n) * dh_e(e);
            d_bulles_l(n) += (d_bulles) ? vfd(f, c) / vf(f) * (*d_bulles)(e,n) : 0;
          }
      for (dv = dv_min, ddv = 0, k = 0; k < N; k++)
        for (l = 0; l < N; l++)
          {
            for (dv_c = 0, d = 0; d < D; d++)
              dv_c += std::pow(pvit(f, N * d + k) - pvit(f, N * d + l), 2);
            dv_c = sqrt(dv_c);
            if (dv_c > dv(k, l))
              for (dv(k, l) = dv_c, d = 0; d < D; d++)
                ddv(k, l, d) = (pvit(f, N * d + k) - pvit(f, N * d + l)) / dv_c;
          }

      correlation_fi.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, dh, dv, d_bulles_l, coeff);
      for (k = 0; k < N; k++)
        for (l = 0; l < N; l++)
          for (j = 0; j < 2; j++)
            coeff(k, l, j) *= 1 + (a_l(k) > 1e-8 ? std::pow(a_l(k) / a_res_, -exp_res) : 0) + (a_l(l) > 1e-8 ? std::pow(a_l(l) / a_res_, -exp_res) : 0);

      /* contributions  */
      for (d = 0; d < D; d++)
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++)
            if (k != l)
              {
                double fac = beta_ * pf(f) * vf(f);
                /* on essaie d'impliciter coeff sans ralentir la convergence en en faisant un developpement limite autour de pvit (dans la direction d'interet seulement) */
                secmem(f, N * d + k) -= fac * (coeff(k, l, 0) * (inco(f, N * d + k) - inco(f, N * d + l)) + coeff(k, l, 1) * ddv(k, l, d) * (pvit(f, N * d + k) - pvit(f, N * d + l)) * ((inco(f, N * d + k) - inco(f, N * d + l)) - (pvit(f, N * d + k) - pvit(f, N * d + l))));
                if (mat)
                  for (j = 0; j < 2; j++)
                    (*mat)(N * (D * f + d) + k, N * (D * f + d) + (j ? l : k)) += fac * (j ? -1 : 1) * (coeff(k, l, 0) + coeff(k, l, 1) * ddv(k, l, d) * (pvit(f, N * d + k) - pvit(f, N * d + l)));

                // secmem(f, N * d + k) -= fac * coeff(k, l, 0) * (inco(f, N * d + k) - inco(f, N * d + l));
                // if (mat)
                //   for (j = 0; j < 2; j++) (*mat)(N * (D * f + d) + k, N * (D * f + d) + (j ? l : k)) += fac * (j ? -1 : 1) * coeff(k, l, 0);
              }
    }
}
