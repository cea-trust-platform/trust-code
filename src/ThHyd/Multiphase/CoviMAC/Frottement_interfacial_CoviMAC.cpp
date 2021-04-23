/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Frottement_interfacial_base.h>

Implemente_instanciable(Frottement_interfacial_CoviMAC,"Frottement_interfacial_Face_CoviMAC", Source_base);

Sortie& Frottement_interfacial_CoviMAC::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_CoviMAC::readOn(Entree& is)
{
  correlation_.typer_lire(equation().probleme(), "Frottement_interfacial", is); //type et lit la correlation de FI
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
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(), &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, equation()).pression().passe(),
                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();

  int e, f, c, i, j, k, l, n, N = inco.line_size(), Np = press.line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot(),
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), dv(N, N), coeff(N, N, 2); //arguments pour coeff
  double dv_min = 0.1;
  const Frottement_interfacial_base& correlation_fi = ref_cast(Frottement_interfacial_base, correlation_.valeur());

  /* faces */
  for (f = 0; f < zone.nb_faces(); f++) if (fcl(f, 0) < 2) for (c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
        {
          for (n = 0; n < N; n++) a_l(n)   = alpha(e, n);
          for (n = 0; n < N; n++) p_l(n)   = press(e, n * (Np > 1));
          for (n = 0; n < N; n++) T_l(n)   = temp(e, n);
          for (n = 0; n < N; n++) rho_l(n) = rho(!cR * e, n);
          for (n = 0; n < N; n++) mu_l(n)  = mu(!cM * e, n);
          for (k = 0; k < N; k++) for (l = 0; l < N; l++) dv(k, l) = max(ch.v_norm(pvit, pvit, e, f, k, l, NULL, NULL), dv_min);
          correlation_fi.coefficient(a_l, p_l, T_l, rho_l, mu_l, dh_e(e), dv, coeff);

          /* contributions : on prend le max entre les deux cotes */
          for (k = 0; k < N; k++) for (l = 0; l < N; l++) if (k != l)
                {
                  double fac = pf(f) * vf(f) * mu_f(f, k, c) * coeff(k, l, 0);
                  secmem(f, k) -= fac * (inco(f, k) - inco(f, l));
                  if (mat) for (j = 0; j < 2; j++) (*mat)(N * f + k, N * f + (j ? l : k)) += fac * (j ? -1 : 1);
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

      for (k = 0; k < N; k++) for (l = 0; l < N; l++) dv(k, l) = max(ch.v_norm(pvit, pvit, e, -1, k, l, NULL, NULL), dv_min);
      correlation_fi.coefficient(a_l, p_l, T_l, rho_l, mu_l, dh_e(e), dv, coeff);

      for (d = 0, i = nf_tot + D * e; d < D; d++, i++) for (k = 0; k < N; k++) for (l = 0; l < N; l++) if (k != l)
              {
                double fac = pe(e) * ve(e) * coeff(k, l, 0);
                secmem(i, k) -= fac * (inco(i, k) - inco(i, l));
                if (mat) for (j = 0; j < 2; j++) (*mat)(N * i + k, N * i + l) += fac * (j ? -1 : 1);
              }
    }
}
