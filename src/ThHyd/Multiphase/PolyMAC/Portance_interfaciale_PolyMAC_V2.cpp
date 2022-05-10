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
// File:        Portance_interfaciale_PolyMAC_V2.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/PolyMAC
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Portance_interfaciale_PolyMAC_V2.h>
#include <Portance_interfaciale_base.h>
#include <Zone_PolyMAC_V2.h>
#include <Champ_Face_PolyMAC_V2.h>
#include <Op_Grad_PolyMAC_V2_Face.h>
#include <Zone_Cl_PolyMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Milieu_composite.h>
#include <Interface_base.h>

Implemente_instanciable(Portance_interfaciale_PolyMAC_V2,"Portance_interfaciale_Face_PolyMAC_V2", Source_base);

Sortie& Portance_interfaciale_PolyMAC_V2::printOn(Sortie& os) const
{
  return os;
}

Entree& Portance_interfaciale_PolyMAC_V2::readOn(Entree& is)
{
  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if (pbm->has_correlation("Portance_interfaciale")) correlation_ = pbm->get_correlation("Portance_interfaciale"); //correlation fournie par le bloc correlation
  else correlation_.typer_lire((*pbm), "Portance_interfaciale", is); //sinon -> on la lit

  pbm->creer_champ("vorticite"); // Besoin de vorticite

  return is;
}

void Portance_interfaciale_PolyMAC_V2::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const // 100% explicit
{
}

void Portance_interfaciale_PolyMAC_V2::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_V2& ch = ref_cast(Champ_Face_PolyMAC_V2, equation().inconnue().valeur());
  const Zone_PolyMAC_V2& zone = ref_cast(Zone_PolyMAC_V2, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl(), &e_f = zone.elem_faces();
  const DoubleTab& n_f = zone.face_normales(), &vf_dir = zone.volumes_entrelaces_dir();
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &fs = zone.face_surfaces();
  const DoubleTab& pvit = ch.passe(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &press = ref_cast(Pb_Multiphase, equation().probleme()).eq_qdm.pression().passe(),
                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique().passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe(),
                        &vort  = equation().probleme().get_champ("vorticite").passe();

  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  int e, f, b, c, i, k, l, n, N = ch.valeurs().line_size(), Np = press.line_size(), D = dimension,
                              cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1);
  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), sigma_l(N,N), dv(N, N), ddv(N, N, 4), ddv_c(4), coeff(N, N); //arguments pour coeff
  double dv_min = 0.1, fac_e, fac_f;
  const Portance_interfaciale_base& correlation_pi = ref_cast(Portance_interfaciale_base, correlation_.valeur());

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
      correlation_pi.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, dv, e, coeff);

      fac_e = pe(e) * ve(e);

      if (D==2)
        {
          for (k = 0; k < N; k++) if (k!= n_l) // gas phase
              {
                i = zone.nb_faces_tot() + D * e;
                secmem(i, n_l) += fac_e * coeff(n_l, k) * (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, 0) ;
                secmem(i,  k ) -= fac_e * coeff(n_l, k) * (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, 0) ;
                secmem(i+1,n_l)-= fac_e * coeff(n_l, k) * (pvit(i  , k) -pvit(i  , n_l)) * vort(e, 0) ;
                secmem(i+1, k )+= fac_e * coeff(n_l, k) * (pvit(i  , k) -pvit(i  , n_l)) * vort(e, 0) ;
              } // 100% explicit

          for (b = 0; b < e_f.dimension(1) && (f = e_f(e, b)) >= 0; b++) if (f<zone.nb_faces()) if (fcl(f, 0) < 2)
                {
                  c = (e == f_e(f,0)) ? 0 : 1 ;
                  fac_f = pf(f) * vf(f);
                  for (k = 0; k < N; k++) if (k!= n_l) // gas phase
                      {
                        secmem(f, n_l) += fac_f * vf_dir(f, c)/vf(f) * n_f(f, 0)/fs(f) * coeff(n_l, k) * (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, 0) ;
                        secmem(f,  k ) -= fac_f * vf_dir(f, c)/vf(f) * n_f(f, 0)/fs(f) * coeff(n_l, k) * (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, 0) ;
                        secmem(f, n_l) -= fac_f * vf_dir(f, c)/vf(f) * n_f(f, 1)/fs(f) * coeff(n_l, k) * (pvit(i  , k) -pvit(i  , n_l)) * vort(e, 0) ;
                        secmem(f,  k ) += fac_f * vf_dir(f, c)/vf(f) * n_f(f, 1)/fs(f) * coeff(n_l, k) * (pvit(i  , k) -pvit(i  , n_l)) * vort(e, 0) ;
                      } // 100% explicit

                }
        }

      if (D==3)
        {
          for (k = 0; k < N; k++) if (k!= n_l) // gas phase
              {
                i = zone.nb_faces_tot() + D * e;
                secmem(i, n_l) += fac_e * coeff(n_l, k) * ((pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 2) - (pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 1)) ;
                secmem(i,  k ) -= fac_e * coeff(n_l, k) * ((pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 2) - (pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 1)) ;
                secmem(i+1,n_l)+= fac_e * coeff(n_l, k) * ((pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 0) - (pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 2)) ;
                secmem(i+1, k )-= fac_e * coeff(n_l, k) * ((pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 0) - (pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 2)) ;
                secmem(i+2,n_l)+= fac_e * coeff(n_l, k) * ((pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 1) - (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 0)) ;
                secmem(i+2, k )-= fac_e * coeff(n_l, k) * ((pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 1) - (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 0)) ;
              } // 100% explicit

          for (b = 0; b < e_f.dimension(1) && (f = e_f(e, b)) >= 0; b++) if (f<zone.nb_faces()) if (fcl(f, 0) < 2)
                {
                  c = (e == f_e(f,0)) ? 0 : 1 ;
                  fac_f = pf(f) * vf(f);
                  for (k = 0; k < N; k++) if (k!= n_l) // gas phase
                      {
                        secmem(f, n_l) += fac_f * vf_dir(f, c)/vf(f) * n_f(f, 0)/fs(f) * coeff(n_l, k) * ((pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 2) - (pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 1)) ;
                        secmem(f,  k ) -= fac_f * vf_dir(f, c)/vf(f) * n_f(f, 0)/fs(f) * coeff(n_l, k) * ((pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 2) - (pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 1)) ;
                        secmem(f, n_l) += fac_f * vf_dir(f, c)/vf(f) * n_f(f, 1)/fs(f) * coeff(n_l, k) * ((pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 0) - (pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 2)) ;
                        secmem(f,  k ) -= fac_f * vf_dir(f, c)/vf(f) * n_f(f, 1)/fs(f) * coeff(n_l, k) * ((pvit(i+2, k) -pvit(i+2, n_l)) * vort(e, n_l*D+ 0) - (pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 2)) ;
                        secmem(f, n_l) += fac_f * vf_dir(f, c)/vf(f) * n_f(f, 2)/fs(f) * coeff(n_l, k) * ((pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 1) - (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 0)) ;
                        secmem(f,  k ) -= fac_f * vf_dir(f, c)/vf(f) * n_f(f, 2)/fs(f) * coeff(n_l, k) * ((pvit(i+0, k) -pvit(i+0, n_l)) * vort(e, n_l*D+ 1) - (pvit(i+1, k) -pvit(i+1, n_l)) * vort(e, n_l*D+ 0)) ;
                      } // 100% explicit

                }
        }

    }
}
