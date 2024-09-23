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

#include <Viscosite_turbulente_base.h>
#include <Dispersion_bulles_base.h>
#include <Dispersion_bulles_VDF.h>
#include <Operateur_Diff_base.h>
#include <Milieu_composite.h>
#include <Masse_Multiphase.h>
#include <Champ_Face_VDF.h>
#include <Operateur_Grad.h>
#include <Pb_Multiphase.h>
#include <Domaine_VF.h>

Implemente_instanciable(Dispersion_bulles_VDF, "Dispersion_bulles_VDF_Face", Source_Dispersion_bulles_base);

Sortie& Dispersion_bulles_VDF::printOn(Sortie& os) const { return os; }

Entree& Dispersion_bulles_VDF::readOn(Entree& is)
{
  Source_Dispersion_bulles_base::readOn(is);
  if (ref_cast(Operateur_Diff_base, equation().operateur(0).l_op_base()).is_turb()) is_turb = 1;
  return is;
}

void Dispersion_bulles_VDF::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const bool res_en_T = pbm.resolution_en_T();
  if (!res_en_T) Process::exit("Dispersion_bulles_VDF::ajouter_blocs NOT YET PORTED TO ENTHALPY EQUATION ! TODO FIXME !!");

  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = domaine.volumes_entrelaces();
  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir();
  const DoubleTab& pvit = ch.passe(),
                   &alpha = pbm.equation_masse().inconnue().passe(),
                    &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                     &temp = pbm.equation_energie().inconnue().passe(),
                      &rho   = equation().milieu().masse_volumique()->passe(),
                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique()->passe();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());

  DoubleTab const * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr ;
  DoubleTab const * k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : nullptr ;

  int N = pvit.line_size() , Np = press.line_size(), nf_tot = domaine.nb_faces_tot(), nf = domaine.nb_faces(), ne_tot = domaine.nb_elem_tot(),  cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Nk = (k_turb) ? (*k_turb).dimension(1) : 1;
  DoubleTrav nut(domaine.nb_elem_tot(), N); //viscosite turbulente
  if (is_turb) ref_cast(Viscosite_turbulente_base, (*ref_cast(Operateur_Diff_base, equation().operateur(0).l_op_base()).correlation_viscosite_turbulente())).eddy_viscosity(nut); //remplissage par la correlation

  // Input-output
  const Dispersion_bulles_base& correlation_db = ref_cast(Dispersion_bulles_base, correlation_.valeur());
  Dispersion_bulles_base::input_t in;
  Dispersion_bulles_base::output_t out;
  in.alpha.resize(N), in.T.resize(N), in.p.resize(N), in.rho.resize(N), in.mu.resize(N), in.sigma.resize(N*(N-1)/2), in.k_turb.resize(N), in.nut.resize(N), in.d_bulles.resize(N), in.nv.resize(N, N);
  out.Ctd.resize(N, N);

  /* Calcul de grad alpha aux faces */

  DoubleTab grad_f_a(nf_tot, N);
  assert ( alpha.dimension_tot(0) == ne_tot );
  const Masse_Multiphase& eq_alp = ref_cast(Masse_Multiphase, pbm.equation_masse());
  const Operateur_Grad& Op_Grad_alp = eq_alp.operateur_gradient_inconnue();
  Op_Grad_alp.calculer(alpha,grad_f_a); // compute grad(diss) at faces

  // Vitesse passee aux elems
  DoubleTab pvit_elem(0, N * dimension);
  domaine.domaine().creer_tableau_elements(pvit_elem);
  ch.get_elem_vector_field(pvit_elem, true);

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
            for (int i = 0 ; i<ne_tot ; i++)
              Sigma_tab(i,ind_trav) = res_en_T ? sat.sigma(temp(i,k),press(i,k * (Np > 1))) : sat.sigma_h(temp(i,k),press(i,k * (Np > 1)));
          }
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
                in.T[n]   += vf_dir(f, c)/vf(f) * temp(e, n); // FIXME si res_en_T
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
                  in.nv(k, n) += vf_dir(f, c)/vf(f) * ch.v_norm(pvit_elem, pvit, e, f, k, n, nullptr, nullptr);
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
}
