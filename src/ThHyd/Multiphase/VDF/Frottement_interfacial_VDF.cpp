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

#include <Frottement_interfacial_base.h>
#include <Frottement_interfacial_VDF.h>
#include <Milieu_composite.h>
#include <Champ_Face_VDF.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Frottement_interfacial_VDF, "Frottement_interfacial_VDF_Face", Source_Frottement_interfacial_base);

Sortie& Frottement_interfacial_VDF::printOn(Sortie& os) const { return os; }
Entree& Frottement_interfacial_VDF::readOn(Entree& is) { return Source_Frottement_interfacial_base::readOn(is); }

void Frottement_interfacial_VDF::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;
  const Domaine_VDF& domaine = ref_cast(Domaine_VDF, equation().domaine_dis().valeur());
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces(),
                    &dh_e = equation().milieu().diametre_hydraulique_elem();
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(), &vfd = domaine.volumes_entrelaces_dir(), &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                   &press = ref_cast(QDM_Multiphase, equation()).pression().passe(), &temp = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                    &rho = equation().milieu().masse_volumique().passe(), &mu = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();

  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  DoubleTab const *d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr;

  int e, f, c, i, j, k, l, n, N = inco.line_size(), Np = press.line_size(), cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), sigma_l(N, N), dv(N, N), ddv(N, N, 4), ddv_c(4), d_bulles_l(N), coeff(N, N, 2); //arguments pour coeff
  double dh;
  const Frottement_interfacial_base& correlation_fi = ref_cast(Frottement_interfacial_base, correlation_.valeur());

  DoubleTab pvit_elem(0, N * dimension);
  domaine.domaine().creer_tableau_elements(pvit_elem);
  ch.get_elem_vector_field(pvit_elem, true);

  /* faces */
  for (f = 0; f < domaine.nb_faces(); f++)
    if (fcl(f, 0) < 2)
      {
        a_l = 0, p_l = 0, T_l = 0, rho_l = 0, mu_l = 0, dh = 0, sigma_l = 0, dv = dv_min, ddv = 0, d_bulles_l = 0;
        for (c = 0; c < 2 ; c++)
          if( (e = f_e(f, c)) >= 0 )
            for (n = 0; n < N; n++)
              {
                a_l(n) += vfd(f, c) / vf(f) * alpha(e, n);
                p_l(n) += vfd(f, c) / vf(f) * press(e, n * (Np > 1));
                T_l(n) += vfd(f, c) / vf(f) * temp(e, n);
                rho_l(n) += vfd(f, c) / vf(f) * rho(!cR * e, n);
                mu_l(n) += vfd(f, c) / vf(f) * mu(!cM * e, n);
                for (k = 0; k < N; k++)
                  if (milc.has_interface(n, k))
                    {
                      Interface_base& sat = milc.get_interface(n, k);
                      sigma_l(n, k) += vfd(f, c) / vf(f) * sat.sigma(temp(e, n), press(e, n * (Np > 1)));
                    }
                dh += vfd(f, c) / vf(f) * alpha(e, n) * dh_e(e);

                for (k = 0; k < N; k++)
                  {
                    double dv_c = ch.v_norm(pvit_elem, pvit, e, f, k, n, nullptr, &ddv_c(0));
                    if (dv_c > dv(k, n))
                      for (dv(k, n) = dv_c, i = 0; i < 4; i++)
                        ddv(k, n, i) = ddv_c(i);

                  }


                d_bulles_l(n) += (d_bulles) ? vfd(f, c) / vf(f) * (*d_bulles)(e, n) : 0;
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
                  for (j = 0; j < 2; j++)
                    (*mat)(N * f + k, N * f + (j ? l : k)) += fac * (j ? -1 : 1) * (coeff(k, l, 0) + coeff(k, l, 1) * ddv(k, l, 3) * (pvit(f, k) - pvit(f, l)));
              }

      }
}
