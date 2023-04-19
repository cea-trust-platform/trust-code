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

#include <Portance_interfaciale_base.h>
#include <Portance_interfaciale_VDF.h>
#include <Milieu_composite.h>
#include <Champ_Face_VDF.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Portance_interfaciale_VDF, "Portance_interfaciale_VDF_Face", Source_Portance_interfaciale_base);

Sortie& Portance_interfaciale_VDF::printOn(Sortie& os) const { return os; }
Entree& Portance_interfaciale_VDF::readOn(Entree& is) {  return Source_Portance_interfaciale_base::readOn(is); }

void Portance_interfaciale_VDF::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue().valeur());
  const Domaine_VDF& domaine = ref_cast(Domaine_VDF, equation().domaine_dis().valeur());
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleTab& n_f = domaine.face_normales(), &vf_dir = domaine.volumes_entrelaces_dir();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces();
  const DoubleTab& pvit = ch.passe(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &press = ref_cast(Pb_Multiphase, equation().probleme()).eq_qdm.pression().passe(),
                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe(),
//                        &grad_v = equation().probleme().get_champ("gradient_vitesse").valeurs(),
                        &vort  = equation().probleme().get_champ("vorticite").valeurs(),
                         * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : NULL,
                           * k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : NULL ;
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  int e, f, c, d, i, k, l, n, N = ch.valeurs().line_size(), Np = press.line_size(), D = dimension, Nk = (k_turb) ? (*k_turb).dimension(1) : 1 ,
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav vr_l(N,D), scal_ur(N), scal_u(N), pvit_l(N, D), vort_l( D==2 ? 1 :D), grad_l(D,D), scal_grad(D); // Requis pour corrections vort et u_l-u-g
  double fac_f, vl_norm;
  const Portance_interfaciale_base& correlation_pi = ref_cast(Portance_interfaciale_base, correlation_.valeur());

  // Vitesse passee aux elems
  DoubleTab pvit_elem(0, N * dimension);
  domaine.domaine().creer_tableau_elements(pvit_elem);
  ch.get_elem_vector_field(pvit_elem, true);

  // in/out pour correlation
  Portance_interfaciale_base::input_t in;
  Portance_interfaciale_base::output_t out;
  in.alpha.resize(N), in.T.resize(N), in.p.resize(N), in.rho.resize(N), in.mu.resize(N), in.sigma.resize(N*(N-1)/2), in.k_turb.resize(N), in.d_bulles.resize(N), in.nv.resize(N, N);
  out.Cl.resize(N, N);

  // Et pour les methodes span de la classe Interface pour choper la tension de surface
  const int nbelem_tot = domaine.nb_elem_tot(), nb_max_sat =  N * (N-1) /2; // oui !! suite arithmetique !!
  DoubleTrav Sigma_tab(nbelem_tot,nb_max_sat);

  // remplir les tabs ...
  for (k = 0; k < N; k++)
    for (l = k + 1; l < N; l++)
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
            VectorD sigma_(nbelem_tot);
            SpanD sigma__(sigma_);
            z_sat.get_sigma(temp.get_span_tot(), press.get_span_tot(), sigma__, N, ind_trav);
            for (i = 0 ; i<nbelem_tot ; i++) Sigma_tab(i, ind_trav) = sigma__[i];
          }
        else if (milc.has_interface(k, l))
          {
            Interface_base& sat = milc.get_interface(k,l);
            const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
            for (i = 0 ; i<nbelem_tot ; i++) Sigma_tab(i,ind_trav) = sat.sigma(temp(i,k),press(i,k * (Np > 1))) ;
          }
      }


  for (f = 0 ; f<domaine.nb_faces() ; f++)
    if (fcl(f, 0) < 2)
      {
        in.alpha=0., in.T=0., in.p=0., in.rho=0., in.mu=0., in.sigma=0., in.k_turb=0., in.d_bulles=0., in.nv=0.;
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
                  in.nv(k, n) += vf_dir(f, c)/vf(f) * ch.v_norm(pvit, pvit, e, f, k, n, nullptr, nullptr);
              }
            for (n = 0; n <Nk; n++) in.k_turb[n]   += (k_turb)   ? vf_dir(f, c)/vf(f) * (*k_turb)(e,0) : 0;
          }

        correlation_pi.coefficient(in, out);

        /*          grad_l = 0; // we fill grad_l so that grad_l(d, d2) = du_d/dx_d2 by averaging between both elements
                  for (d = 0 ; d<D ; d++)
                    for (d2 = 0 ; d2<D ; d2++)
                      for (c=0 ; c<2  && (e = f_e(f, c)) >= 0; c++)
                        grad_l(d, d2) += vf_dir(f, c)/vf(f)*grad_v(nf_tot + D*e + d2 , n_l * D + d) ;
                  //We replace the n_l components by the one calculated without interpolation to elements
                  scal_grad = 0 ; // scal_grad(d) = grad(u_d).n_f
                  for (d = 0 ; d<D ; d++)
                    for (d2 = 0 ; d2<D ; d2++)
                      scal_grad(d) += grad_l(d, d2)*n_f(f, d2)/fs(f);
                  for (d = 0 ; d<D ; d++)
                    for (d2 = 0 ; d2<D ; d2++)
                      grad_l(d, d2) += (grad_v(f ,n_l*D+d) - scal_grad(d)) * n_f(f, d2)/fs(f);
                  // We calculate the local vorticity using this local gradient
                  vort_l(0) = grad_l(2, 1) - grad_l(1, 2); // dUz/dy - dUy/dz
                  vort_l(1) = grad_l(0, 2) - grad_l(2, 0); // dUx/dz - dUz/dx
                  vort_l(2) = grad_l(1, 0) - grad_l(0, 1); // dUy/dx - dUx/dy
        */

        // Quid de n_l != 0 ?
        vort_l = 0;
        n = 0;
        if (D==2)
          {
            for (c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
              vort_l(0) += vort(e, n) * vf_dir(f, c)/vf(f);
          }
        if (D==3)
          {
            for (c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
              for (d=0; d<D; d++)
                vort_l(d) += vort(e, N*d+n) * vf_dir(f, c)/vf(f);
          }

        // We also need to calculate relative velocity at the face
        pvit_l = 0 ;
        for (d = 0 ; d<D ; d++)
          for (k = 0 ; k<N ; k++)
            for (c=0 ; c<2 && (e = f_e(f, c)) >= 0; c++)
              pvit_l(k, d) += vf_dir(f, c)/vf(f)*pvit_elem(e, D*k + d) ;
        scal_u = 0;
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            scal_u(k) += pvit_l(k, d)*n_f(f, d)/fs(f);
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            pvit_l(k, d) += (pvit(f, k) - scal_u(k)) * n_f(f, d)/fs(f) ; // Corect velocity at the face
        vl_norm = 0;
        scal_ur = 0;
        for (d = 0 ; d < D ; d++) vl_norm += pvit_l(n_l, d)*pvit_l(n_l, d);
        vl_norm = std::sqrt(vl_norm);
        if (vl_norm > 1.e-6)
          {
            for (k = 0; k < N; k++)
              for (d = 0 ; d < D ; d++) scal_ur(k) += pvit_l(n_l, d)/vl_norm * (pvit_l(k, d) -pvit_l(n_l, d));
            for (k = 0; k < N; k++)
              for (d = 0 ; d < D ; d++) vr_l(k, d)  = pvit_l(n_l, d)/vl_norm * scal_ur(k) ;
          }
        else for (k=0 ; k<N ; k++)
            for (d=0 ; d<D ; d++) vr_l(k, d) = pvit_l(k, d)-pvit_l(n_l, d) ;

        if (D==2)
          {
            // Use local vairables for the calculation of secmem
            fac_f = beta_*pf(f) * vf(f);
            for (k = 0; k < N; k++)
              if (k!= n_l) // gas phase
                {
                  secmem(f, n_l) += fac_f * n_f(f, 0)/fs(f) * out.Cl(n_l, k) * vr_l(k, 1) * vort_l(0) ;
                  secmem(f,  k ) -= fac_f * n_f(f, 0)/fs(f) * out.Cl(n_l, k) * vr_l(k, 1) * vort_l(0) ;
                  secmem(f, n_l) -= fac_f * n_f(f, 1)/fs(f) * out.Cl(n_l, k) * vr_l(k, 0) * vort_l(0) ;
                  secmem(f,  k ) += fac_f * n_f(f, 1)/fs(f) * out.Cl(n_l, k) * vr_l(k, 0) * vort_l(0) ;
                } // 100% explicit
          }

        if (D==3)
          {
            // Use local vairables for the calculation of secmem
            fac_f = beta_*pf(f) * vf(f);
            for (k = 0; k < N; k++)
              if (k!= n_l) // gas phase
                {
                  secmem(f, n_l) += fac_f * n_f(f, 0)/fs(f) * out.Cl(n_l, k) * (vr_l(k, 1) * vort_l(2) - vr_l(k, 2) * vort_l(1)) ;
                  secmem(f,  k ) -= fac_f * n_f(f, 0)/fs(f) * out.Cl(n_l, k) * (vr_l(k, 1) * vort_l(2) - vr_l(k, 2) * vort_l(1)) ;
                  secmem(f, n_l) += fac_f * n_f(f, 1)/fs(f) * out.Cl(n_l, k) * (vr_l(k, 2) * vort_l(0) - vr_l(k, 0) * vort_l(2)) ;
                  secmem(f,  k ) -= fac_f * n_f(f, 1)/fs(f) * out.Cl(n_l, k) * (vr_l(k, 2) * vort_l(0) - vr_l(k, 0) * vort_l(2)) ;
                  secmem(f, n_l) += fac_f * n_f(f, 2)/fs(f) * out.Cl(n_l, k) * (vr_l(k, 0) * vort_l(1) - vr_l(k, 1) * vort_l(0)) ;
                  secmem(f,  k ) -= fac_f * n_f(f, 2)/fs(f) * out.Cl(n_l, k) * (vr_l(k, 0) * vort_l(1) - vr_l(k, 1) * vort_l(0)) ;
                } // 100% explicit
          }
      }
}
