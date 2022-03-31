/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Frottement_interfacial_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/CoviMAC
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Frottement_interfacial_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Frottement_interfacial_base.h>
#include <Milieu_composite.h>
#include <Interface_base.h>

Implemente_instanciable(Frottement_interfacial_CoviMAC,"Frottement_interfacial_Face_CoviMAC", Source_base);

Sortie& Frottement_interfacial_CoviMAC::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_CoviMAC::readOn(Entree& is)
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  if (pbm.has_correlation("frottement_interfacial")) correlation_ = pbm.get_correlation("frottement_interfacial"); //correlation fournie par le bloc correlation
  else correlation_.typer_lire(pbm, "frottement_interfacial", is); //sinon -> on la lit
  return is;
}

void Frottement_interfacial_CoviMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();
  const IntTab& fcl = ch.fcl();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  int i, j, e, f, k, l, N = inco.line_size(), d, db, D = dimension, nf_tot = zone.nb_faces_tot();
  /* faces */
  for (f = 0; f < zone.nb_faces(); f++) if (fcl(f, 0) < 2)
      {
        for (k = 0; k < N; k++) for (l = 0; l < N; l++) stencil.append_line(N * f + k, N * f + l);
        // for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (d = 0, j = nf_tot + D * e; d < D; d++, j++)
        //     for (k = 0; k < N; k++) for (l = 0; l < N; l++) stencil.append_line(N * f + k, N * j + l);
      }
  /* elements */
  for (e = 0, i = nf_tot; e < zone.nb_elem_tot(); e++) for (d = 0; d < D; d++, i++) for (db = 0, j = nf_tot + D * e; db < D; db++, j++)
        for (k = 0; k < N; k++) for (l = 0; l < N; l++) stencil.append_line(N * i + k, N * j + l);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Frottement_interfacial_CoviMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &dh_e = zone.diametre_hydraulique_elem();
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(), &vfd = zone.volumes_entrelaces_dir(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, equation()).pression().passe(),
                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  int e, f, c, i, j, k, l, n, N = inco.line_size(), Np = press.line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot(),
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), exp_res = 2;
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), sigma_l(N,N), dv(N, N), ddv(N, N, 4), ddv_c(4), coeff(N, N, 2); //arguments pour coeff
  double dv_min = 0.1, dh, a_res = 1e-2;
  const Frottement_interfacial_base& correlation_fi = ref_cast(Frottement_interfacial_base, correlation_.valeur());

  /* faces */
  for (f = 0; f < zone.nb_faces(); f++) if (fcl(f, 0) < 2)
      {
        for (a_l = 0, p_l = 0, T_l = 0, rho_l = 0, mu_l = 0, dh = 0, sigma_l = 0, dv = dv_min, ddv = 0, c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
          {
            for (n = 0; n < N; n++) a_l(n)   += vfd(f, c) / vf(f) * alpha(e, n);
            for (n = 0; n < N; n++) p_l(n)   += vfd(f, c) / vf(f) * press(e, n * (Np > 1));
            for (n = 0; n < N; n++) T_l(n)   += vfd(f, c) / vf(f) * temp(e, n);
            for (n = 0; n < N; n++) rho_l(n) += vfd(f, c) / vf(f) * rho(!cR * e, n);
            for (n = 0; n < N; n++) mu_l(n)  += vfd(f, c) / vf(f) * mu(!cM * e, n);
            for (n = 0; n < N; n++) for (k = 0; k < N; k++) if (milc.has_saturation(n,k))
                  {
                    Interface_base& sat = milc.get_interface(n, k);
                    sigma_l(n,k) += vfd(f, c) / vf(f) * sat.sigma_(temp(e,n),press(e,n * (Np > 1)));
                  }

            for (n = 0; n < N; n++) dh += vfd(f, c) / vf(f) * alpha(e, n) * dh_e(e);
            for (k = 0; k < N; k++) for (l = 0; l < N; l++)
                {
                  double dv_c = ch.v_norm(pvit, pvit, e, f, k, l, NULL, &ddv_c(0));
                  if (dv_c > dv(k, l)) for (dv(k, l) = dv_c, i = 0; i < 4; i++) ddv(k, l, i) = ddv_c(i);
                }
          }
        correlation_fi.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, dh, dv, coeff);
        for (k = 0; k < N; k++) for (l = 0; l < N; l++) for (j = 0; j < 2; j++)
              coeff(k, l, j) *= 1 + (a_l(k) > 1e-8 ? std::pow(a_l(k) / a_res, -exp_res) : 0) + (a_l(l) > 1e-8 ? std::pow(a_l(l) / a_res, -exp_res) : 0);

        /* contributions : on prend le max entre les deux cotes */
        for (k = 0; k < N; k++) for (l = 0; l < N; l++) if (k != l)
              {
                double fac = pf(f) * vf(f);
                /* on essaie d'impliciter coeff sans ralentir la convergence en en faisant un developpement limite autour de pvit (dans la direction d'interet seulement) */
                secmem(f, k) -= fac * (coeff(k, l, 0) * (inco(f, k) - inco(f, l)) + coeff(k, l, 1) * ddv(k, l, 3) * (pvit(f, k) - pvit(f, l)) * ((inco(f, k) - inco(f, l)) - (pvit(f, k) - pvit(f, l))));
                if (mat) for (j = 0; j < 2; j++) (*mat)(N * f + k, N * f + (j ? l : k)) += fac * (j ? -1 : 1) * (coeff(k, l, 0) + coeff(k, l, 1) * ddv(k, l, 3) * (pvit(f, k) - pvit(f, l)));
              }
      }

  /* elements */
  for (e = 0; e < zone.nb_elem_tot(); e++)
    {
      /* arguments de coeff */
      for (n = 0; n < N; n++) a_l(n)   = alpha(e, n);
      for (n = 0; n < N; n++) p_l(n)   = press(e, n * (Np > 1));
      for (n = 0; n < N; n++) T_l(n)   =  temp(e, n);
      for (n = 0; n < N; n++) rho_l(n) =   rho(!cR * e, n);
      for (n = 0; n < N; n++) mu_l(n)  =    mu(!cM * e, n);
      for (n = 0; n < N; n++)
        {
          for (k = 0; k < N; k++) if(milc.has_interface(n, k))
              {
                Interface_base& sat = milc.get_interface(n, k);
                sigma_l(n,k) = sat.sigma_(temp(e,n), press(e,n * (Np > 1)));
              }
        }

      for (k = 0; k < N; k++) for (l = 0; l < N; l++) dv(k, l) = std::max(ch.v_norm(pvit, pvit, e, -1, k, l, NULL, &ddv(k, l, 0)), dv_min);
      correlation_fi.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, dh_e(e), dv, coeff);
      for (k = 0; k < N; k++) for (l = 0; l < N; l++) coeff(k, l, 1) *= (dv(k, l) > dv_min); //pas de derivee si dv < dv_min
      for (k = 0; k < N; k++) for (l = 0; l < N; l++) for (j = 0; j < 2; j++)
            coeff(k, l, j) *= 1 + (a_l(k) > 1e-8 ? std::pow(a_l(k) / a_res, -exp_res) : 0) + (a_l(l) > 1e-8 ? std::pow(a_l(l) / a_res, -exp_res) : 0);

      for (d = 0, i = nf_tot + D * e; d < D; d++, i++) for (k = 0; k < N; k++) for (l = 0; l < N; l++) if (k != l)
              {
                double fac = pe(e) * ve(e);
                /* on essaie d'impliciter coeff sans ralentir la convergence en en faisant un developpement limite autour de pvit (dans la direction d'interet seulement) */
                secmem(i, k) -= fac * (coeff(k, l, 0) * (inco(i, k) - inco(i, l)) + coeff(k, l, 1) * ddv(k, l, d) * (pvit(i, k) - pvit(i, l)) * ((inco(i, k) - inco(i, l)) - (pvit(i, k) - pvit(i, l))));
                if (mat) for (j = 0; j < 2; j++) (*mat)(N * i + k, N * i + l) += fac * (j ? -1 : 1) * (coeff(k, l, 0) + coeff(k, l, 1) * ddv(k, l, d) * (pvit(i, k) - pvit(i, l)));
              }
    }
}
