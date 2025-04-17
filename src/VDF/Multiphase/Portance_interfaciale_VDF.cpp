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
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const bool res_en_T = pbm.resolution_en_T();
  if (!res_en_T) Process::exit("Portance_interfaciale_VDF::ajouter_blocs NOT YET PORTED TO ENTHALPY EQUATION ! TODO FIXME !!");

  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue());
  const Domaine_VDF& domaine = ref_cast(Domaine_VDF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& n_f = domaine.face_normales(), &vf_dir = domaine.volumes_entrelaces_dir(),  &xv = domaine.xv();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces();
  const DoubleTab& pvit = ch.passe(),
                   &alpha = pbm.equation_masse().inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                     &temp  = pbm.equation_energie().inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe(),
                        &grad_v = equation().probleme().get_champ("gradient_vitesse").valeurs(),
                         &vort  = equation().probleme().get_champ("vorticite").valeurs(),
                          * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr,
                            * k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : nullptr ;
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  int e, f, c, d, i, k, l, n, N = ch.valeurs().line_size(), Np = press.line_size(), D = dimension, Nk = (k_turb) ? (*k_turb).dimension(1) : 1 ,
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav vr_l(N,D), scal_ur(N), scal_u(N), pvit_l(N, D), vort_l( D==2 ? 1 :D); // Requis pour corrections vort et u_l-u-g
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
            // recuperer sigma ...
            const DoubleTab& sig = z_sat.get_sigma_tab();
            // fill in the good case
            for (int ii = 0; ii < nbelem_tot; ii++) Sigma_tab(ii, ind_trav) = sig(ii);
          }
        else if (milc.has_interface(k, l))
          {
            Interface_base& sat = milc.get_interface(k,l);
            const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
            for (i = 0 ; i<nbelem_tot ; i++)
              Sigma_tab(i,ind_trav) = res_en_T ? sat.sigma(temp(i,k),press(i,k * (Np > 1))) : sat.sigma_h(temp(i,k),press(i,k * (Np > 1)));
          }
      }


  for (f = 0 ; f<domaine.nb_faces() ; f++)
    if (fcl(f, 0) < 2)
      {
        in.alpha=0., in.T=0., in.p=0., in.rho=0., in.mu=0., in.sigma=0., in.k_turb=0., in.d_bulles=0., in.nv=0.;
        for ( c = 0; c < 2 ; c++)
          if ((e = f_e(f, c)) >= 0 )
            {
              for (n = 0; n < N; n++)
                {
                  in.alpha[n] += vf_dir(f, c)/vf(f) * alpha(e, n);
                  in.p[n]     += vf_dir(f, c)/vf(f) * press(e, n * (Np > 1));
                  in.T[n]     += vf_dir(f, c)/vf(f) * temp(e, n); // FIXME SI res_en_T
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
                    in.nv(k, n) += vf_dir(f, c)/vf(f) * ch.v_norm(pvit_elem, pvit, e, f, k, n, nullptr, nullptr);
                }
              for (n = 0; n <Nk; n++) in.k_turb[n]   += (k_turb)   ? vf_dir(f, c)/vf(f) * (*k_turb)(e,0) : 0;
            }

        correlation_pi.coefficient(in, out);

        // Quid de n_l != 0 ?
        vort_l = 0;

        if ( (f_e(f, 0)<0) || (f_e(f,1)<0) ) vort_l(0) = f_e(f, 0)>=0 ? vort(f_e(f, 0), n_l) : vort(f_e(f, 1), n_l) ;
        else
          {
            {
              int orif = domaine.orientation(f);
              DoubleTrav grad_loc(D);
              for ( c = 0; c < 2 ; c++)
                {
                  e = f_e(f, c);
                  for ( d = 0 ; d < e_f.line_size() ; d++)
                    {
                      int fb = e_f(e,d);
                      int orifb = domaine.orientation(fb);
                      if (orifb != orif) grad_loc(orifb) += .25*pvit(fb, n_l)/(xv(fb, orif)-xv(f, orif));
                    }
                }
              DoubleTrav grad_ll(D,D);
              for ( c = 0; c < 2 ; c++)
                for (int d_U=0; d_U<D; d_U++)
                  for (int d_X=0; d_X<D; d_X++)
                    grad_ll(d_U,d_X) += grad_v( e, N * ( D*d_U+d_X ) + n_l) * vf_dir(f, c)/vf(f);
              for (int oril = 0 ; oril<D ; oril++)
                if (oril != orif)
                  grad_ll(oril,orif) = grad_loc(oril);
              if (D==2) vort_l(0) = grad_ll(1,0) - grad_ll(0,1);
              else // (D==3)
                {
                  vort_l(0) = grad_ll(2, 1) - grad_ll(1, 2); // dUz/dy - dUy/dz
                  vort_l(1) = grad_ll(0, 2) - grad_ll(2, 0); // dUx/dz - dUz/dx
                  vort_l(2) = grad_ll(1, 0) - grad_ll(0, 1); // dUy/dx - dUx/dy
                }
            }
          }

        // We also need to calculate relative velocity at the face
        pvit_l = 0 ;
        for (d = 0 ; d<D ; d++)
          for (k = 0 ; k<N ; k++)
            for ( c = 0; c < 2 ; c++)
              if ((e = f_e(f, c)) >= 0 )
                pvit_l(k, d) += vf_dir(f, c)/vf(f)*pvit_elem(e, N*d+k) ;
        scal_u = 0;
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            scal_u(k) += pvit_l(k, d)*n_f(f, d)/fs(f);
        for (k = 0 ; k<N ; k++)
          for (d = 0 ; d<D ; d++)
            pvit_l(k, d) += (pvit(f, k) - scal_u(k)) * n_f(f, d)/fs(f) ; // Corect velocity at the face
        if (ur_parallel_ul_)
          {
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
          }
        else
          for (k=0 ; k<N ; k++)
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
