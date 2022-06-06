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

#include <Dispersion_bulles_PolyMAC_P0.h>
#include <Dispersion_bulles_base.h>
#include <Zone_PolyMAC_P0.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Zone_Cl_PolyMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Milieu_composite.h>
#include <Interface_base.h>
#include <Op_Diff_Turbulent_PolyMAC_P0_Face.h>
#include <Viscosite_turbulente_base.h>
#include <Neumann_paroi.h>
#include <Echange_impose_base.h>

Implemente_instanciable(Dispersion_bulles_PolyMAC_P0,"Dispersion_bulles_Face_PolyMAC_P0", Source_base);

Sortie& Dispersion_bulles_PolyMAC_P0::printOn(Sortie& os) const
{
  return os;
}

Entree& Dispersion_bulles_PolyMAC_P0::readOn(Entree& is)
{
  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");

  if (pbm->has_correlation("Dispersion_bulles")) correlation_ = pbm->get_correlation("Dispersion_bulles"); //correlation fournie par le bloc correlation
  else correlation_.typer_lire((*pbm), "Dispersion_bulles", is); //sinon -> on la lit

  if sub_type(Op_Diff_Turbulent_PolyMAC_P0_Face, equation().operateur(0).l_op_base()) is_turb = 1;

  return is;
}

void Dispersion_bulles_PolyMAC_P0::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const // 100% explicit
{
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;
  const Zone_PolyMAC_P0& zone = ref_cast(Zone_PolyMAC_P0, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();
  const IntTab& fcl = ch.fcl();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  int i, j, e, f, k, l, N = inco.line_size(), d, db, D = dimension, nf_tot = zone.nb_faces_tot();

  /*faces*/
  for (f = 0; f < zone.nb_faces(); f++)
    if (fcl(f, 0) < 2)
      for (k = 0; k < N; k++)
        for (l = 0; l < N; l++) stencil.append_line(N * f + k, N * f + l);
  /* elements */
  for (e = 0, i = nf_tot; e < zone.nb_elem_tot(); e++)
    for (d = 0; d < D; d++, i++)
      for (db = 0, j = nf_tot + D * e; db < D; db++, j++)
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++) stencil.append_line(N * i + k, N * j + l);


  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Dispersion_bulles_PolyMAC_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;
  const Zone_PolyMAC_P0& zone = ref_cast(Zone_PolyMAC_P0, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl(), &e_f = zone.elem_faces();
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &fs = zone.face_surfaces();
  const DoubleTab& vf_dir = zone.volumes_entrelaces_dir(), &xp = zone.xp(), &xv = zone.xv();
  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &press = ref_cast(Pb_Multiphase, equation().probleme()).eq_qdm.pression().passe(),
                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  DoubleTab const * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : NULL ;
  DoubleTab const * k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : NULL ;

  DoubleTrav nut(mu); //viscosite turbulente
  if (is_turb)
    {
      ref_cast(Viscosite_turbulente_base, ref_cast(Op_Diff_Turbulent_PolyMAC_P0_Face, equation().operateur(0).l_op_base()).correlation().valeur()).eddy_viscosity(nut); //remplissage par la correlation
    }

  int N = inco.line_size() , Np = press.line_size(), D = dimension, nf_tot = zone.nb_faces_tot(), nf = zone.nb_faces(), ne_tot = zone.nb_elem_tot(),  cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Nk = (k_turb) ? (*k_turb).dimension(1) : 1;
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), sigma_l(N,N), dv(N, N), ddv(N, N, 4), ddv_c(4), nut_l(N), k_l(Nk), d_b_l(N), coeff(N, N, 2); //arguments pour coeff

  const Dispersion_bulles_base& correlation_db = ref_cast(Dispersion_bulles_base, correlation_.valeur());

  /* calculaiton of the gradient of alpha at the face */
  const Champ_Elem_PolyMAC_P0& ch_a = ref_cast(Champ_Elem_PolyMAC_P0, ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().valeur());
  DoubleTrav grad_f_a(equation().inconnue().valeurs());
  ch_a.init_grad(0);
  const IntTab& fg_d = ch_a.fgrad_d, &fg_e = ch_a.fgrad_e;  // Tables utilisees dans zone_PolyMAC_P0::fgrad pour le calcul du gradient
  const DoubleTab& fg_w = ch_a.fgrad_w;
  const Conds_lim&    cls_a = ch_a.zone_Cl_dis().les_conditions_limites(); 		// conditions aux limites du champ k
  const IntTab&       fcl_a = ch_a.fcl();	// tableaux utilitaires sur les CLs : fcl(f, .) = (type de la CL, no de la CL, indice dans la CL)

  for (int n = 0; n < N; n++)
    for (int f = 0; f < nf; f++)
      {
        grad_f_a(f, n) = 0;
        for (int j = fg_d(f); j < fg_d(f+1) ; j++)
          {
            int e = fg_e(j);
            int f_bord;
            if ( (f_bord = e-ne_tot) < 0) //contribution d'un element
              grad_f_a(f, n) += fg_w(j) * alpha(e, n);
            else if (fcl_a(f_bord, 0) == 1 || fcl_a(f_bord, 0) == 2) //Echange_impose_base
              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Echange_impose_base, cls_a[fcl_a(f_bord, 1)].valeur()).T_ext(fcl_a(f_bord, 2), n) : 0;
            else if (fcl_a(f_bord, 0) == 4) //Neumann non homogene
              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Neumann_paroi      , cls_a[fcl_a(f_bord, 1)].valeur()).flux_impose(fcl_a(f_bord, 2), n) : 0;
            else if (fcl_a(f_bord, 0) == 6) // Dirichlet
              grad_f_a(f, n) += fg_w(j) * ref_cast(Dirichlet, cls_a[fcl_a(f_bord, 1)].valeur()).val_imp(fcl_a(f_bord, 2), n);
          }
      }

  /* Calcul du grad aux elems */
  for (int n = 0; n < N; n++)
    for (int e = 0; e < ne_tot; e++)
      for (int j = 0, f; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        for (int d = 0; d < D; d++)
          grad_f_a(nf_tot + D*e +d, n) += (e == f_e(f, 0) ? 1 : -1) * fs(f) * (xv(f, d) - xp(e, d)) / ve(e) * grad_f_a(f, n);

  /* faces */
  for (int f = 0; f < nf; f++)
    if (fcl(f, 0) < 2)
      {
        a_l = 0 ;
        p_l = 0 ;
        T_l = 0 ;
        rho_l = 0;
        mu_l = 0 ;
        d_b_l=0;
        sigma_l=0;
        dv = 0, ddv = 0 ;
        int e;
        for (int c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
          {
            for (int n = 0; n < N; n++) a_l(n)   += vf_dir(f, c)/vf(f) * alpha(e, n);
            for (int n = 0; n < N; n++) p_l(n)   += vf_dir(f, c)/vf(f) * press(e, n * (Np > 1));
            for (int n = 0; n < N; n++) T_l(n)   += vf_dir(f, c)/vf(f) * temp(e, n);
            for (int n = 0; n < N; n++) rho_l(n) += vf_dir(f, c)/vf(f) * rho(!cR * e, n);
            for (int n = 0; n < N; n++) mu_l(n)  += vf_dir(f, c)/vf(f) * mu(!cM * e, n);
            for (int n = 0; n < N; n++) nut_l(n) += is_turb    ? vf_dir(f, c)/vf(f) * nut(e,n) : 0;
            for (int n = 0; n <Nk; n++) k_l(n)   += (k_turb)   ? vf_dir(f, c)/vf(f) * (*k_turb)(e,0) : 0;
            for (int n = 0; n < N; n++) d_b_l(n) += (d_bulles) ? vf_dir(f, c)/vf(f) * (*d_bulles)(e,n) : 0;
            for (int n = 0; n < N; n++)
              for (int k = 0; k < N; k++)
                if (milc.has_interface(n,k))
                  {
                    Interface_base& sat = milc.get_interface(n, k);
                    sigma_l(n,k) += vf_dir(f, c)/vf(f) * sat.sigma_(temp(e,n),press(e,n * (Np > 1)));
                  }
            for (int k = 0; k < N; k++)
              for (int l = 0; l < N; l++)
                {
                  double dv_c = ch.v_norm(pvit, pvit, e, f, k, l, NULL, &ddv_c(0));
                  int i ;
                  for (i = 0, dv(k, l) = dv_c; i < 4; i++) ddv(k, l, i) = ddv_c(i);
                }
          }
        correlation_db.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, nut_l, k_l, d_b_l, dv, coeff); // correlation identifies the liquid phase

        for (int k = 0; k < N; k++)
          for (int l = 0; l < N; l++)
            if (k != l)
              {
                double fac = pf(f) * vf(f);
                secmem(f, k) += fac * ( - coeff(k, l, 0) * grad_f_a(f, k) + coeff(l, k, 0) * grad_f_a(f, l));
                if (mat)
                  for (int j = 0; j < 2; j++)
                    (*mat)(N * f + k, N * f + (j ? l : k)) -= fac * (j ? -1 : 1) * ( - coeff(k, l, 1) * grad_f_a(f, k) + coeff(l, k, 1) * grad_f_a(f, l)) * ddv(k, l, 3);
              }
      }

  /* elements */
  for (int e = 0; e < ne_tot; e++)
    {
      /* arguments de coeff */
      for (int n = 0; n < N; n++) a_l(n)   = alpha(e, n);
      for (int n = 0; n < N; n++) p_l(n)   = press(e, n * (Np > 1));
      for (int n = 0; n < N; n++) T_l(n)   =  temp(e, n);
      for (int n = 0; n < N; n++) rho_l(n) =   rho(!cR * e, n);
      for (int n = 0; n < N; n++) mu_l(n)  =    mu(!cM * e, n);
      for (int n = 0; n < N; n++) nut_l(n) += is_turb    ? nut(e,n) : 0;
      for (int n = 0; n <Nk; n++) k_l(n)   += (k_turb)   ? (*k_turb)(e,0) : 0;
      for (int n = 0; n < N; n++) d_b_l(n) += (d_bulles) ? (*d_bulles)(e,n) : 0;
      for (int n = 0; n < N; n++)
        for (int k = 0; k < N; k++)
          if (milc.has_interface(n,k))
            {
              Interface_base& sat = milc.get_interface(n, k);
              sigma_l(n,k) += sat.sigma_(temp(e,n),press(e,n * (Np > 1)));
            }

      for (int k = 0; k < N; k++)
        for (int l = 0; l < N; l++) dv(k, l) = ch.v_norm(pvit, pvit, e, -1, k, l, NULL, &ddv(k, l, 0));
      correlation_db.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, nut_l, k_l, d_b_l, dv, coeff);

      for (int d = 0, i = nf_tot + D * e; d < D; d++, i++)
        for (int k = 0; k < N; k++)
          for (int l = 0; l < N; l++)
            if (k != l)
              {
                double fac = pe(e) * ve(e);
                secmem(i, k) += fac * ( - coeff(k, l, 0) * grad_f_a(i, k) + coeff(l, k, 0) * grad_f_a(i, l));
                if (mat)
                  for (int j = 0; j < 2; j++)
                    (*mat)(N * i + k, N * i + l) -= fac * (j ? -1 : 1) * ( - coeff(k, l, 1) * grad_f_a(i, k) + coeff(l, k, 1) * grad_f_a(i, l)) * ddv(k, l, d);
              }
    }
}
