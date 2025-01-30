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

#include <Portance_interfaciale_PolyVEF_P0.h>
#include <Portance_interfaciale_base.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Milieu_composite.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Portance_interfaciale_PolyVEF_P0, "Portance_interfaciale_Face_PolyVEF_P0", Source_Portance_interfaciale_base);

Sortie& Portance_interfaciale_PolyVEF_P0::printOn(Sortie& os) const { return os; }
Entree& Portance_interfaciale_PolyVEF_P0::readOn(Entree& is) {  return Source_Portance_interfaciale_base::readOn(is); }

void Portance_interfaciale_PolyVEF_P0::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue().valeur());
  const Domaine_PolyVEF_P0& domaine = ref_cast(Domaine_PolyVEF_P0, equation().domaine_dis().valeur());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  const DoubleTab& pvit = ch.passe(),
                   &alpha = pbm.equation_masse().inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                     &temp  = pbm.equation_energie().inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe(),
                        &vort  = equation().probleme().get_champ("vorticite").passe(),
                         *d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").passe() : NULL,
                          *k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : NULL ;
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  int e, f, c, d, i, k, l, n, N = pbm.nb_phases(), Np = press.line_size(), D = dimension, Nk = (k_turb) ? (*k_turb).dimension(1) : 1 ,
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav vort_l( D==2 ? 1 :D), scal_ur(N), vr_l(N,D); // Requis pour interpolation vort
  double fac_f, vl_norm;
  const Portance_interfaciale_base& correlation_pi = ref_cast(Portance_interfaciale_base, correlation_.valeur());

  Portance_interfaciale_base::input_t  in;
  Portance_interfaciale_base::output_t out;
  in.alpha.resize(N), in.T.resize(N), in.p.resize(N), in.rho.resize(N), in.mu.resize(N), in.sigma.resize(N*(N-1)/2), in.k_turb.resize(N), in.d_bulles.resize(N), in.nv.resize(N, N);
  out.Cl.resize(N, N);

  // Et pour les methodes span de la classe Interface pour choper la tension de surface
  const int ne_tot = domaine.nb_elem_tot(), nb_max_sat =  N * (N-1) /2; // oui !! suite arithmetique !!
  DoubleTrav Sigma_tab(ne_tot,nb_max_sat);

  // remplir les tabs ...
  for (k = 0; k < N; k++)
    for (l = k + 1; l < N; l++)
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
            for (i = 0 ; i<ne_tot ; i++) Sigma_tab(i,ind_trav) = sat.sigma(temp(i,k),press(i,k * (Np > 1))) ;
          }
      }

  // Faces
  for (f = 0 ; f<domaine.nb_faces() ; f++)
    {
      in.alpha=0., in.T=0., in.p=0., in.rho=0., in.mu=0., in.sigma=0., in.k_turb=0., in.d_bulles=0., in.nv=0., vort_l=0.;
      for ( c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
        {
          for (n = 0; n < N; n++)
            {
              in.alpha[n] += vf_dir(f, c)/vf(f) * alpha(e, n);
              in.p[n]     += vf_dir(f, c)/vf(f) * press(e, n * (Np > 1));
              in.T[n]     += vf_dir(f, c)/vf(f) * temp(e, n);
              in.rho[n]   += vf_dir(f, c)/vf(f) * rho(!cR * e, n);
              in.mu[n]    += vf_dir(f, c)/vf(f) * mu(!cM * e, n);
              in.d_bulles[n] += (d_bulles) ? vf_dir(f, c)/vf(f) *(*d_bulles)(e,n) : 0 ;
              for (k = n+1; k < N; k++)
                if (milc.has_interface(n,k))
                  {
                    const int ind_trav = (n*(N-1)-(n-1)*(n)/2) + (k-n-1);
                    in.sigma[ind_trav] += vf_dir(f, c) / vf(f) * Sigma_tab(e, ind_trav);
                  }
              for (k = 0; k < N; k++)
                in.nv(k, n) = ch.v_norm(pvit, f, k, n, nullptr);
            }
          for (n = 0; n <Nk; n++) in.k_turb[n]   += (k_turb)   ? vf_dir(f, c)/vf(f) * (*k_turb)(e,0) : 0;
          for (d = 0; d < vort_l.dimension(0); d++) vort_l(d) += vf_dir(f, c)/vf(f) * vort(e,N*d+n_l);
        }

      correlation_pi.coefficient(in, out);

      // Relative velocity parallel to the liquid flow
      vl_norm = 0;
      scal_ur = 0;
      for (d = 0 ; d < D ; d++) vl_norm += pvit(f, N*d+n_l)*pvit(f, N*d+n_l);
      vl_norm = std::sqrt(vl_norm);
      if (vl_norm > 1.e-6)
        {
          for (k = 0; k < N; k++)
            for (d = 0 ; d < D ; d++) scal_ur(k) += pvit(f, N*d+n_l)/vl_norm * (pvit(f, N*d+k) -pvit(f, N*d+n_l));
          for (k = 0; k < N; k++)
            for (d = 0 ; d < D ; d++) vr_l(k, d)  = pvit(f, N*d+n_l)/vl_norm * scal_ur(k) ;
        }
      else for (k=0 ; k<N ; k++)
          for (d=0 ; d<D ; d++) vr_l(k, d) = pvit(f, N*d+k)-pvit(f, N*d+n_l) ;

      // Use local vairables for the calculation of secmem
      fac_f = beta_*pf(f) * vf(f);
      for (k = 0; k < N; k++)
        if (k!= n_l) // gas phase
          {
            if (D==3)
              {
                secmem(f, N*0+n_l) += fac_f * out.Cl(n_l, k) * (vr_l(k, 1) * vort_l(2) - vr_l(k, 2) * vort_l(1)) ;
                secmem(f, N*0+ k ) -= fac_f * out.Cl(n_l, k) * (vr_l(k, 1) * vort_l(2) - vr_l(k, 2) * vort_l(1)) ;
                secmem(f, N*1+n_l) += fac_f * out.Cl(n_l, k) * (vr_l(k, 2) * vort_l(0) - vr_l(k, 0) * vort_l(2)) ;
                secmem(f, N*1+ k ) -= fac_f * out.Cl(n_l, k) * (vr_l(k, 2) * vort_l(0) - vr_l(k, 0) * vort_l(2)) ;
                secmem(f, N*2+n_l) += fac_f * out.Cl(n_l, k) * (vr_l(k, 0) * vort_l(1) - vr_l(k, 1) * vort_l(0)) ;
                secmem(f, N*2+ k ) -= fac_f * out.Cl(n_l, k) * (vr_l(k, 0) * vort_l(1) - vr_l(k, 1) * vort_l(0)) ;
              } // 100% explicit
            else if (D==2)
              {
                secmem(f, N*0+n_l) += fac_f * out.Cl(n_l, k) * vr_l(k, 1) * vort_l(0) ;
                secmem(f, N*0+ k ) -= fac_f * out.Cl(n_l, k) * vr_l(k, 1) * vort_l(0) ;
                secmem(f, N*1+n_l) -= fac_f * out.Cl(n_l, k) * vr_l(k, 0) * vort_l(0) ;
                secmem(f, N*1+ k ) += fac_f * out.Cl(n_l, k) * vr_l(k, 0) * vort_l(0) ;
              } // 100% explicit
          }
    }
}

void Portance_interfaciale_PolyVEF_P0::mettre_a_jour(double temps)
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  /* Wobble si besoin */
  if ((wobble.non_nul()) || (C_lift.non_nul()))
    {
      const Champ_Face_PolyVEF_P0& ch = ref_cast(Champ_Face_PolyVEF_P0, equation().inconnue().valeur());
      const DoubleTab& pvit = equation().inconnue().valeur().passe(),
                       &alpha = pbm.equation_masse().inconnue().passe(),
                        &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe(),
                         &rho   = equation().milieu().masse_volumique().passe(),
                          &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                           &temp  = pbm.equation_energie().inconnue().passe(),
                            *d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").passe() : NULL,
                             *k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : NULL ;
      const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
      const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

      int i, k, l, e, n,
          N = pvit.line_size()/dimension, Np = press.line_size(), Nk = (k_turb) ? (*k_turb).dimension(1) : 1 , ne_tot=domaine.nb_elem_tot(),
          cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
      DoubleTrav Sigma_tab(ne_tot,N*(N-1)/2);

      for (k = 0; k < N; k++)
        for (l = k + 1; l < N; l++)
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
                for (i = 0 ; i<ne_tot ; i++) Sigma_tab(i,ind_trav) = sat.sigma(temp(i,k),press(i,k * (Np > 1))) ;
              }
          }

      if ((wobble.non_nul()))
        {
          DoubleTab& tab_wobble = wobble->valeurs();
          for (k=0 ; k<N ; k++)
            if (k!=n_l) // k gas phase
              for (e=0 ; e<ne_tot ; e++)
                {
                  int ind_trav = (k>n_l) ? (n_l*(N-1)-(n_l-1)*(n_l)/2) + (k-n_l-1) : (k*(N-1)-(k-1)*(k)/2) + (n_l-k-1);
                  double dv_loc = std::max(ch.v_norm(pvit, e, k, n_l, nullptr), 1.e-6);
                  double Eo = g_ * std::abs(rho(e,n_l)-rho(e,k)) * (*d_bulles)(e,k)*(*d_bulles)(e,k)/std::max(Sigma_tab(e,ind_trav), 1.e-6);
                  tab_wobble(e,k) = Eo * (*k_turb)(e,n_l)/(dv_loc*dv_loc) ;
                }
        }

      if (C_lift.non_nul())
        {
          DoubleTab& tab_cl = C_lift->valeurs();

          const Portance_interfaciale_base& correlation_pi = ref_cast(Portance_interfaciale_base, correlation_.valeur());

          Portance_interfaciale_base::input_t in;
          Portance_interfaciale_base::output_t out;
          in.alpha.resize(N), in.T.resize(N), in.p.resize(N), in.rho.resize(N), in.mu.resize(N), in.sigma.resize(N*(N-1)/2), in.k_turb.resize(N), in.d_bulles.resize(N), in.nv.resize(N, N);
          out.Cl.resize(N, N);
          for (e = 0; e < ne_tot; e++)
            {
              /* arguments de coeff */
              for (n=0; n<N; n++)
                {
                  in.alpha[n] = alpha(e, n);
                  in.p[n]     = press(e, n * (Np > 1));
                  in.T[n]     = temp(e, n);
                  in.rho[n]   = rho(!cR * e, n);
                  in.mu[n]    = mu(!cM * e, n);
                  in.d_bulles[n] = (d_bulles) ? (*d_bulles)(e,n) : 0 ;
                  for (k = n+1; k < N; k++)
                    if (milc.has_interface(n,k))
                      {
                        const int ind_trav = (n*(N-1)-(n-1)*(n)/2) + (k-n-1);
                        in.sigma[ind_trav] = Sigma_tab(e, ind_trav);
                      }
                  for (k = 0; k < N; k++)
                    in.nv(k, n) = ch.v_norm(pvit, e, k, n, nullptr);
                }
              for (n = 0; n <Nk; n++) in.k_turb[n]   = (k_turb) ? (*k_turb)(e,0) : 0;

              correlation_pi.coefficient(in, out);

              for (k=0 ; k<N ; k++)
                if (k!=n_l) // k gas phase
                  tab_cl(e,k) = out.Cl(n_l, k) / ( std::max(in.rho[n_l], 1.e-6) * std::max(in.alpha[k], 1.e-6) ) ;
            }
        }
    }
}
