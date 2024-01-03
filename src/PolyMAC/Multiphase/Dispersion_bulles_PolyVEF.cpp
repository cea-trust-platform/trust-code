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

#include <Op_Diff_Turbulent_PolyVEF_Face.h>
#include <Dispersion_bulles_PolyVEF.h>
#include <Viscosite_turbulente_base.h>
#include <Dispersion_bulles_base.h>
#include <Champ_Face_PolyVEF.h>
#include <Champ_Elem_PolyVEF_P0.h>
#include <Milieu_composite.h>
#include <Pb_Multiphase.h>
#include <Neumann_paroi.h>

Implemente_instanciable(Dispersion_bulles_PolyVEF,"Dispersion_bulles_Face_PolyVEF_P0|Dispersion_bulles_Face_PolyVEF_P0P1", Dispersion_bulles_PolyMAC_P0);

Sortie& Dispersion_bulles_PolyVEF::printOn(Sortie& os) const { return os; }

Entree& Dispersion_bulles_PolyVEF::readOn(Entree& is)
{
  return Dispersion_bulles_PolyMAC_P0::readOn(is);
}

void Dispersion_bulles_PolyVEF::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Champ_Face_PolyVEF& ch = ref_cast(Champ_Face_PolyVEF, equation().inconnue());
  const Domaine_PolyVEF& domaine = ref_cast(Domaine_PolyVEF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir();
  const DoubleTab& pvit = ch.passe(),
                   &alpha = pbm.equation_masse().inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                     &temp  = pbm.equation_energie().inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  DoubleTab const * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr ;
  DoubleTab const * k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : nullptr ;
  DoubleTab const * k_WIT = (equation().probleme().has_champ("k_WIT")) ? &equation().probleme().get_champ("k_WIT").passe() : nullptr ;

  int Np = press.line_size(), D = dimension, N = pbm.nb_phases(), nf = domaine.nb_faces(), ne_tot = domaine.nb_elem_tot(),  cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Nk = (k_turb) ? (*k_turb).dimension(1) : 1;
  DoubleTrav nut(domaine.nb_elem_tot(), N); //viscosite turbulente
  if (is_turb) ref_cast(Viscosite_turbulente_base, ref_cast(Op_Diff_Turbulent_PolyVEF_Face, equation().operateur(0).l_op_base()).correlation()).eddy_viscosity(nut); //remplissage par la correlation

  // Input-output
  const Dispersion_bulles_base& correlation_db = ref_cast(Dispersion_bulles_base, correlation_.valeur());
  Dispersion_bulles_base::input_t in;
  Dispersion_bulles_base::output_t out;
  in.alpha.resize(N), in.T.resize(N), in.p.resize(N), in.rho.resize(N), in.mu.resize(N), in.sigma.resize(N*(N-1)/2), in.k_turb.resize(N), in.nut.resize(N), in.d_bulles.resize(N), in.nv.resize(N, N);
  out.Ctd.resize(N, N);

  /* calculaiton of the gradient of alpha at the face */
  const Champ_Elem_PolyVEF_P0& ch_a = ref_cast(Champ_Elem_PolyVEF_P0, pbm.equation_masse().inconnue());
  DoubleTrav grad_f_a(nf, N, D);
  ch_a.init_grad(0);
  const IntTab& fgrad_d = ch_a.fgrad_d, &fgrad_e = ch_a.fgrad_e;  // Tables utilisees dans domaine_PolyVEF::fgrad pour le calcul du gradient
  const DoubleTab&  fgrad_w = ch_a.fgrad_w;
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
            // recuperer sigma ...
            const DoubleTab& sig = z_sat.get_sigma_tab();
            // fill in the good case
            for (int ii = 0; ii < ne_tot; ii++) Sigma_tab(ii, ind_trav) = sig(ii);
          }
        else if (milc.has_interface(k, l))
          {
            Interface_base& sat = milc.get_interface(k,l);
            const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
            for (int i = 0 ; i<ne_tot ; i++) Sigma_tab(i,ind_trav) = sat.sigma(temp(i,k),press(i,k * (Np > 1))) ;
          }
      }

  for (int f = 0; f < nf; f++)
    for (int i = fgrad_d(f); i < fgrad_d(f + 1); i++)
      for (int e = fgrad_e(i), d = 0; d < D; d++)
        for (int n = 0; n < N; n++)
          {
            int fb = e - ne_tot;
            if ( fb < 0)//contribution d'un element
              grad_f_a(f, n, d) += fgrad_w(i, d, 0) * alpha(e, n);
            else if (fcl_a(fb, 0) == 4) //Neumann non homogene
              grad_f_a(f, n, d) += fgrad_w(i, d, 0) * ref_cast(Neumann_paroi, cls_a[fcl_a(fb, 1)].valeur()).flux_impose(fcl_a(fb, 2), n);
            else if (fcl_a(fb, 0) == 6) // Dirichlet
              grad_f_a(f, n, d) += fgrad_w(i, d, 0) * ref_cast(Dirichlet, cls_a[fcl_a(fb, 1)].valeur()).val_imp(fcl_a(fb, 2), n);
          }

  /* faces */
  for (int f = 0; f < nf; f++)
    {
      in.alpha=0., in.T=0., in.rho=0., in.mu=0., in.sigma=0., in.k_turb=0., in.nut=0., in.d_bulles=0., in.nv=0., in.k_WIT=0;
      int e;
      for (int c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
        {
          for (int n = 0; n < N; n++)
            {
              in.alpha[n]+=vf_dir(f, c)/vf(f) * alpha(e, n);
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
            }
          for (int n = 0; n <Nk; n++) in.k_turb[n]   += (k_turb)   ? vf_dir(f, c)/vf(f) * (*k_turb)(e,0) : 0;
          in.k_WIT   += (k_WIT)   ? vf_dir(f, c)/vf(f) * (*k_WIT)(e,0) : 0.;
          in.e = e;
        }
      for (int n = 0; n < N; n++)
        for (int k = 0; k < N; k++)  in.nv(k, n) = ch.v_norm(pvit, f, k, n, nullptr);

      correlation_db.coefficient(in, out);

      for (int k = 0; k < N; k++)
        for (int l = 0; l < N; l++)
          for (int d = 0; d <D; d++)
            if (k != l)
              {
                double fac = beta_*pf(f) * vf(f);
                secmem(f, N * d + k) += fac * ( - out.Ctd(k, l) * grad_f_a(f, k, d) + out.Ctd(l, k) * grad_f_a(f, l, d));
              }
    }
}
