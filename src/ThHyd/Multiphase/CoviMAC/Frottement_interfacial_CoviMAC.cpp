/****************************************************************************
* Copyright (c) 2020, CEA
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
#include <Op_Grad_CoviMAC_Face.h>
#include <Zone_Cl_CoviMAC.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Frottement_interfacial_CoviMAC,"Frottement_interfacial_Face_CoviMAC", Source_base);

Sortie& Frottement_interfacial_CoviMAC::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_CoviMAC::readOn(Entree& is)
{
  is >> correlation_; //type et lit la correlation de FI
  return is;
}

void Frottement_interfacial_CoviMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();
  ch.init_cl();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  int i, e, f, k, l, N = inco.line_size(), d, D = dimension;
  /* faces */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0) < 2) for (k = 0; k < N; k++) for (l = 0; l < N; l++)
          stencil.append_line(N * f + k, N * f + l);
  /* elements */
  for (e = 0, i = zone.nb_faces_tot(); e < zone.nb_elem(); e++) for (d = 0; d < D; d++, i++) for (k = 0; k < N; k++) for (l = 0; l < N; l++)
          stencil.append_line(N * i + k, N * i + l);

  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Frottement_interfacial_CoviMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &dh_e = zone.diametre_hydraulique_elem();
  const DoubleTab& inco = ch.valeurs(), &passe = ch.passe(), &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, equation()).pression().passe(),
                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();

  int e, f, i, k, l, n, N = inco.line_size(), Np = press.line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot(),
                        cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), dv(N, N), coeff(N, N, 2); //arguments pour coeff
  double dh;

  /* faces */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0) < 2)
      {
        /* interpolations */
        for (a_l = 0, p_l = 0, T_l = 0, rho_l = 0, mu_l = 0, dh = 0, dv = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (n = 0; n < N; n++) a_l(n)   += mu_f(f, n, i) * alpha.addr()[N * e + n];
            for (n = 0; n < N; n++) p_l(n)   += mu_f(f, n, i) * press.addr()[Np * e + n * (Np > 1)];
            for (n = 0; n < N; n++) T_l(n)   += mu_f(f, n, i) *  temp.addr()[N * e + n];
            for (n = 0; n < N; n++) rho_l(n) += mu_f(f, n, i) *   rho.addr()[!cR * N * e + n];
            for (n = 0; n < N; n++) mu_l(n)  += mu_f(f, n, i) *    mu.addr()[!cM * N * e + n];
            for (n = 0; n < N; n++) dh += mu_f(f, n, i) * alpha.addr()[N * e + n] * dh_e(e);
            for (k = 0; k < N; k++) for (l = k + 1; l < N; l++) dv(k, l) = max(dv(k, l), ch.v_norm(passe, passe, e, f, k, l, NULL, NULL));
          }

        /* calcul du coefficient */
        correlation_->coefficient(a_l, p_l, T_l, rho_l, mu_l, dh, dv, coeff);

        /* contributions */
        for (k = 0; k < N; k++) for (l = 0; l < N; l++) if (k != l)
              {
                double fac = pf(f) * vf(f) * coeff(min(k, l), max(k, l), 0);
                secmem.addr()[N * f + k] -= fac * (inco.addr()[N * f + k] - inco.addr()[N * f + l]);
                if (mat) (*mat)(N * f + k, N * f + k) += fac, (*mat)(N * f + k, N * f + l) -= fac;
              }
      }

  /* elements */
  for (e = 0; e < zone.nb_elem(); e++)
    {
      /* arguments de coeff */
      for (n = 0; n < N; n++) a_l(n)   = alpha.addr()[N * e + n];
      for (n = 0; n < N; n++) p_l(n)   = press.addr()[Np * e + n * (Np > 1)];
      for (n = 0; n < N; n++) T_l(n)   =  temp.addr()[N * e + n];
      for (n = 0; n < N; n++) rho_l(n) =   rho.addr()[!cR * N * e + n];
      for (n = 0; n < N; n++) mu_l(n)  =    mu.addr()[!cM * N * e + n];

      for (k = 0; k < N; k++) for (l = k + 1; l < N; l++) dv(k, l) = ch.v_norm(passe, passe, e, -1, k, l, NULL, NULL);
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (k = 0; k < N; k++) for (l = k + 1; l < N; l++)
            dv(k, l)  =max(dv(k, l), ch.v_norm(passe, passe, e, f, k, l, NULL, NULL));

      /* calcul du coefficient */
      correlation_->coefficient(a_l, p_l, T_l, rho_l, mu_l, dh_e(e), dv, coeff);

      /* contributions */
      for (d = 0, i = nf_tot + D * e; d < D; d++, i++) for (k = 0; k < N; k++) for (l = 0; l < N; l++) if (k != l)
              {
                double fac = pe(e) * ve(e) * coeff(min(k, l), max(k, l), 0);
                secmem.addr()[N * i + k] -= fac * (inco.addr()[N * i + k] - inco.addr()[N * i + l]);
                if (mat) (*mat)(N * i + k, N * i + k) += fac, (*mat)(N * i + k, N * i + l) -= fac;
              }
    }
}
