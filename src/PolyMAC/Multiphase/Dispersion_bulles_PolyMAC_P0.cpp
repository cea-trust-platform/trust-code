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

#include <Op_Diff_Turbulent_PolyMAC_P0_Face.h>
#include <Dispersion_bulles_PolyMAC_P0.h>
#include <Viscosite_turbulente_base.h>
#include <Dispersion_bulles_base.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Milieu_composite.h>
#include <Pb_Multiphase.h>
#include <Neumann_paroi.h>

Implemente_instanciable(Dispersion_bulles_PolyMAC_P0,"Dispersion_bulles_Face_PolyMAC_P0", Source_Dispersion_bulles_base);

Sortie& Dispersion_bulles_PolyMAC_P0::printOn(Sortie& os) const { return os; }

Entree& Dispersion_bulles_PolyMAC_P0::readOn(Entree& is)
{
  Source_Dispersion_bulles_base::readOn(is);
  if sub_type(Op_Diff_Turbulent_PolyMAC_P0_Face, equation().operateur(0).l_op_base()) is_turb = 1;
  return is;
}

void Dispersion_bulles_PolyMAC_P0::dimensionner_blocs_aux(IntTrav& stencil) const
{
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur());
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, equation().domaine_dis().valeur());
  const DoubleTab& inco = ch.valeurs();

  int i, j, e, k, l, N = inco.line_size(), d, db, D = dimension, nf_tot = domaine.nb_faces_tot();

  /* elements */
  for (e = 0, i = nf_tot; e < domaine.nb_elem_tot(); e++)
    for (d = 0; d < D; d++, i++)
      for (db = 0, j = nf_tot + D * e; db < D; db++, j++)
        for (k = 0; k < N; k++)
          for (l = 0; l < N; l++) stencil.append_line(N * i + k, N * j + l);
}

void Dispersion_bulles_PolyMAC_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur());
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, equation().domaine_dis().valeur());
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl(), &e_f = domaine.elem_faces();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &ve = domaine.volumes(), &vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces();
  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir(), &xp = domaine.xp(), &xv = domaine.xv();
  const DoubleTab& pvit = ch.passe(),
                   &alpha = pbm.equation_masse().inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                     &temp  = pbm.equation_energie().inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  DoubleTab const * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : NULL ;
  DoubleTab const * k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : NULL ;

  int N = pvit.line_size() , Np = press.line_size(), D = dimension, nf_tot = domaine.nb_faces_tot(), nf = domaine.nb_faces(), ne_tot = domaine.nb_elem_tot(),  cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Nk = (k_turb) ? (*k_turb).dimension(1) : 1;
  DoubleTrav nut(domaine.nb_elem_tot(), N); //viscosite turbulente
  if (is_turb) ref_cast(Viscosite_turbulente_base, ref_cast(Op_Diff_Turbulent_PolyMAC_P0_Face, equation().operateur(0).l_op_base()).correlation().valeur()).eddy_viscosity(nut); //remplissage par la correlation

  // Input-output
  const Dispersion_bulles_base& correlation_db = ref_cast(Dispersion_bulles_base, correlation_->valeur());
  Dispersion_bulles_base::input_t in;
  Dispersion_bulles_base::output_t out;
  in.alpha.resize(N), in.T.resize(N), in.p.resize(N), in.rho.resize(N), in.mu.resize(N), in.sigma.resize(N*(N-1)/2), in.k_turb.resize(N), in.nut.resize(N), in.d_bulles.resize(N), in.nv.resize(N, N);
  out.Ctd.resize(N, N);

  /* calculaiton of the gradient of alpha at the face */
  const Champ_Elem_PolyMAC_P0& ch_a = ref_cast(Champ_Elem_PolyMAC_P0, pbm.equation_masse().inconnue().valeur());
  DoubleTrav grad_f_a(pvit);
  ch_a.init_grad(0);
  const IntTab& fg_d = ch_a.fgrad_d, &fg_e = ch_a.fgrad_e;  // Tables utilisees dans domaine_PolyMAC_P0::fgrad pour le calcul du gradient
  const DoubleTab&  fg_w = ch_a.fgrad_w;
  const Conds_lim& cls_a = ch_a.domaine_Cl_dis().les_conditions_limites(); 		// conditions aux limites du champ alpha
  const IntTab&    fcl_a = ch_a.fcl();	// tableaux utilitaires sur les CLs : fcl(f, .) = (type de la CL, no de la CL, indice dans la CL)

  // Et pour les methodes span de la classe Interface pour choper la tension de surface
  const int nb_max_sat =  N * (N-1) /2; // oui !! suite arithmetique !!
  DoubleTrav Sigma_tab(ne_tot,nb_max_sat);

  // remplir les tabs ...
  for (int k = 0; k < N; k++)
    for (int l = k + 1; l < N; l++)
      {
        if (milc.has_saturation(k, l))
          {
            Saturation_base& z_sat = milc.get_saturation(k, l);
            const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
            // XXX XXX XXX
            // Attention c'est dangereux ! on suppose pour le moment que le champ de pression a 1 comp. Par contre la taille de res est nb_max_sat*nbelem !!
            // Aussi, on passe le Span le nbelem pour le champ de pression et pas nbelem_tot ....
            assert(press.line_size() == 1);
            assert(temp.line_size() == N);
            VectorD sigma_(ne_tot);
            SpanD sigma__(sigma_);
            z_sat.get_sigma(temp.get_span_tot(), press.get_span_tot(), sigma__, N*(N-1)/2, ind_trav);
            for (int i = 0 ; i<ne_tot ; i++) Sigma_tab(i, ind_trav) = sigma__[i];
          }
        else if (milc.has_interface(k, l))
          {
            Interface_base& sat = milc.get_interface(k,l);
            const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
            for (int i = 0 ; i<ne_tot ; i++) Sigma_tab(i,ind_trav) = sat.sigma(temp(i,k),press(i,k * (Np > 1))) ;
          }
      }

  for (int n = 0; n < N; n++)
    for (int f = 0; f < nf_tot; f++)
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
      for (int d = 0; d < D; d++)
        {
          grad_f_a(nf_tot + D*e +d, n) = 0 ;
          for (int j = 0, f; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
            grad_f_a(nf_tot + D*e +d, n) += (e == f_e(f, 0) ? 1 : -1) * fs(f) * (xv(f, d) - xp(e, d)) / ve(e) * grad_f_a(f, n);
        }

  /* faces */
  for (int f = 0; f < nf; f++)
    if (fcl(f, 0) < 2)
      {
        in.alpha=0., in.T=0., in.rho=0., in.mu=0., in.sigma=0., in.k_turb=0., in.nut=0., in.d_bulles=0., in.nv=0.;
        int e;
        for (int c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
          {
            for (int n = 0; n < N; n++)
              {
                in.alpha[n]   += vf_dir(f, c)/vf(f) * alpha(e, n);
                in.p[n]   += vf_dir(f, c)/vf(f) * press(e, n * (Np > 1));
                in.T[n]   += vf_dir(f, c)/vf(f) * temp(e, n);
                in.rho[n] += vf_dir(f, c)/vf(f) * rho(!cR * e, n);
                in.mu[n]  += vf_dir(f, c)/vf(f) * mu(!cM * e, n);
                in.nut[n] += is_turb    ? vf_dir(f, c)/vf(f) * nut(e,n) : 0;
                in.d_bulles[n] += (d_bulles) ? vf_dir(f, c)/vf(f) * (*d_bulles)(e,n) : 0;
                for (int k = n+1; k < N; k++)
                  if (milc.has_interface(n,k))
                    {
                      const int ind_trav = (n*(N-1)-(n-1)*(n)/2) + (k-n-1);
                      in.sigma[ind_trav] += vf_dir(f, c) / vf(f) * Sigma_tab(e, ind_trav);
                    }
                for (int k = 0; k < N; k++)
                  in.nv(k, n) += vf_dir(f, c)/vf(f) * ch.v_norm(pvit, pvit, e, f, k, n, nullptr, nullptr);
              }
            for (int n = 0; n <Nk; n++) in.k_turb[n]   += (k_turb)   ? vf_dir(f, c)/vf(f) * (*k_turb)(e,0) : 0;
          }

        correlation_db.coefficient(in, out);

        for (int k = 0; k < N; k++)
          for (int l = 0; l < N; l++)
            if (k != l)
              {
                double fac = beta_*pf(f) * vf(f);
                secmem(f, k) += fac * ( - out.Ctd(k, l) * grad_f_a(f, k) + out.Ctd(l, k) * grad_f_a(f, l));
              }
      }

  /* elements */
  for (int e = 0; e < ne_tot; e++)
    {
      /* arguments de coeff */
      for (int n = 0; n < N; n++)
        {
          in.alpha[n] = alpha(e, n);
          in.p[n]     = press(e, n * (Np > 1));
          in.T[n]     = temp(e, n);
          in.rho[n]   = rho(!cR * e, n);
          in.mu[n]    = mu(!cM * e, n);
          in.nut[n]   = is_turb    ? nut(e,n) : 0;
          in.d_bulles[n] = (d_bulles) ? (*d_bulles)(e,n) : 0;
          for (int k = n+1; k < N; k++)
            if (milc.has_interface(n,k))
              {
                const int ind_trav = (n*(N-1)-(n-1)*(n)/2) + (k-n-1);
                in.sigma[ind_trav] = Sigma_tab(e, ind_trav);
              }
          for (int k = 0; k < N; k++)
            in.nv(k, n) = ch.v_norm(pvit, pvit, e, -1, k, n, nullptr, nullptr);
        }
      for (int n = 0; n <Nk; n++) in.k_turb[n]   = (k_turb) ? (*k_turb)(e,0) : 0;

      correlation_db.coefficient(in, out);

      for (int d = 0, i = nf_tot + D * e; d < D; d++, i++)
        for (int k = 0; k < N; k++)
          for (int l = 0; l < N; l++)
            if (k != l)
              {
                double fac = beta_*pe(e) * ve(e);
                secmem(i, k) += fac * ( - out.Ctd(k, l) * grad_f_a(i, k) + out.Ctd(l, k) * grad_f_a(i, l));
              }
    }
}
