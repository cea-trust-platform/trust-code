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

#include <Frottement_interfacial_PolyMAC_P0.h>
#include <Frottement_interfacial_base.h>
#include <Op_Grad_PolyMAC_P0_Face.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Milieu_composite.h>
#include <Zone_PolyMAC_P0.h>
#include <Zone_Cl_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Interface_base.h>
#include <QDM_Multiphase.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

Implemente_instanciable(Frottement_interfacial_PolyMAC_P0, "Frottement_interfacial_Face_PolyMAC_P0", Source_Frottement_interfacial_base);

Sortie& Frottement_interfacial_PolyMAC_P0::printOn(Sortie& os) const { return os; }
Entree& Frottement_interfacial_PolyMAC_P0::readOn(Entree& is) { return Source_Frottement_interfacial_base::readOn(is); }

void Frottement_interfacial_PolyMAC_P0::dimensionner_blocs_aux(IntTrav& stencil) const
{
  const DoubleTab& inco = ref_cast(Champ_Face_base, equation().inconnue().valeur()).valeurs();
  const Zone_VF& zone = ref_cast(Zone_VF, equation().zone_dis().valeur());
  int i, j, e, k, l, N = inco.line_size(), d, db, D = dimension, nf_tot = zone.nb_faces_tot();

  /* elements */
  for (e = 0, i = nf_tot; e < zone.nb_elem_tot(); e++)
    for (d = 0; d < D; d++, i++)
      for (db = 0, j = nf_tot + D * e; db < D; db++, j++)
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++) stencil.append_line(N * i + k, N * j + l);
}

void Frottement_interfacial_PolyMAC_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;
  const Zone_PolyMAC_P0& zone = ref_cast(Zone_PolyMAC_P0, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &dh_e = equation().milieu().diametre_hydraulique_elem();
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(), &vfd = zone.volumes_entrelaces_dir(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, equation()).pression().passe(),
                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  DoubleTab const * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr ;

  int e, f, c, i, j, k, l, n, N = inco.line_size(), Np = press.line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot(),
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), sigma_l(N,N), dv(N, N), ddv(N, N, 4), ddv_c(4), d_bulles_l(N), coeff(N, N, 2); //arguments pour coeff
  double dh;
  const Frottement_interfacial_base& correlation_fi = ref_cast(Frottement_interfacial_base, correlation_.valeur());

  /* faces */
  for (f = 0; f < zone.nb_faces(); f++)
    if (fcl(f, 0) < 2)
      {
        for (a_l = 0, p_l = 0, T_l = 0, rho_l = 0, mu_l = 0, dh = 0, sigma_l = 0, dv = dv_min, ddv = 0, d_bulles_l=0, c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
          for (n = 0; n < N; n++)
            {
              a_l(n)   += vfd(f, c) / vf(f) * alpha(e, n);
              p_l(n)   += vfd(f, c) / vf(f) * press(e, n * (Np > 1));
              T_l(n)   += vfd(f, c) / vf(f) * temp(e, n);
              rho_l(n) += vfd(f, c) / vf(f) * rho(!cR * e, n);
              mu_l(n)  += vfd(f, c) / vf(f) * mu(!cM * e, n);
              for (k = 0; k < N; k++)
                if (milc.has_interface(n,k))
                  {
                    Interface_base& sat = milc.get_interface(n, k);
                    sigma_l(n,k) += vfd(f, c) / vf(f) * sat.sigma(temp(e,n),press(e,n * (Np > 1)));
                  }
              dh += vfd(f, c) / vf(f) * alpha(e, n) * dh_e(e);
              for (k = 0; k < N; k++)
                {
                  double dv_c = ch.v_norm(pvit, pvit, e, f, k, n, nullptr, &ddv_c(0));
                  if (dv_c > dv(k, n))
                    for (dv(k, n) = dv_c, i = 0; i < 4; i++) ddv(k, n, i) = ddv_c(i);
                }
              d_bulles_l(n) += (d_bulles) ? vfd(f, c) / vf(f) * (*d_bulles)(e,n) : 0;
            }

        correlation_fi.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, dh, dv, d_bulles_l, coeff);
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++)
            for (j = 0; j < 2; j++)
              coeff(k, l, j) *= 1 + (a_l(k) > 1e-8 ? std::pow(a_l(k) / a_res_, -exp_res) : 0) + (a_l(l) > 1e-8 ? std::pow(a_l(l) / a_res_, -exp_res) : 0);

        /* contributions : on prend le max entre les deux cotes */
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++)
            if (k != l)
              {
                double fac = pf(f) * vf(f);
                /* on essaie d'impliciter coeff sans ralentir la convergence en en faisant un developpement limite autour de pvit (dans la direction d'interet seulement) */
                secmem(f, k) -= fac * (coeff(k, l, 0) * (inco(f, k) - inco(f, l)) + coeff(k, l, 1) * ddv(k, l, 3) * (pvit(f, k) - pvit(f, l)) * ((inco(f, k) - inco(f, l)) - (pvit(f, k) - pvit(f, l))));
                if (mat)
                  for (j = 0; j < 2; j++) (*mat)(N * f + k, N * f + (j ? l : k)) += fac * (j ? -1 : 1) * (coeff(k, l, 0) + coeff(k, l, 1) * ddv(k, l, 3) * (pvit(f, k) - pvit(f, l)));
              }
      }

  /* elements */
  for (e = 0; e < zone.nb_elem_tot(); e++)
    {
      /* arguments de coeff */
      for (n = 0; n < N; n++)
        {
          a_l(n)   = alpha(e, n);
          p_l(n)   = press(e, n * (Np > 1));
          T_l(n)   =  temp(e, n);
          rho_l(n) =   rho(!cR * e, n);
          mu_l(n)  =    mu(!cM * e, n);
          for (k = 0; k < N; k++)
            if(milc.has_interface(n, k))
              {
                Interface_base& sat = milc.get_interface(n, k);
                sigma_l(n,k) = sat.sigma(temp(e,n), press(e,n * (Np > 1)));
              }
          d_bulles_l(n) = (d_bulles) ? (*d_bulles)(e,n) : 0;

          for (k = 0; k < N; k++) dv(k, n) = std::max(ch.v_norm(pvit, pvit, e, -1, k, n, nullptr, &ddv(k, n, 0)), dv_min);

        }

      correlation_fi.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, dh_e(e), dv, d_bulles_l, coeff);

      for (k = 0; k < N; k++)
        for (l = 0; l < N; l++)
          {
            coeff(k, l, 1) *= (dv(k, l) > dv_min); //pas de derivee si dv < dv_min
            for (j = 0; j < 2; j++)
              coeff(k, l, j) *= 1 + (a_l(k) > 1e-8 ? std::pow(a_l(k) / a_res_, -exp_res) : 0) + (a_l(l) > 1e-8 ? std::pow(a_l(l) / a_res_, -exp_res) : 0);
          }

      for (d = 0, i = nf_tot + D * e; d < D; d++, i++)
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++)
            if (k != l)
              {
                double fac = pe(e) * ve(e);
                /* on essaie d'impliciter coeff sans ralentir la convergence en en faisant un developpement limite autour de pvit (dans la direction d'interet seulement) */
                secmem(i, k) -= fac * (coeff(k, l, 0) * (inco(i, k) - inco(i, l)) + coeff(k, l, 1) * ddv(k, l, d) * (pvit(i, k) - pvit(i, l)) * ((inco(i, k) - inco(i, l)) - (pvit(i, k) - pvit(i, l))));
                if (mat)
                  for (j = 0; j < 2; j++) (*mat)(N * i + k, N * i + (j ? l : k)) += fac * (j ? -1 : 1) * (coeff(k, l, 0) + coeff(k, l, 1) * ddv(k, l, d) * (pvit(i, k) - pvit(i, l)));
              }
    }
}
