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

#include <Frottement_interfacial_base.h>
#include <Dispersion_bulles_base.h>
#include <Portance_interfaciale_base.h>
#include <Vitesse_derive_Forces.h>
#include <Pb_Multiphase.h>
#include <cmath>

Implemente_instanciable(Vitesse_derive_Forces, "Vitesse_relative_derive_Forces", Vitesse_derive_base);

Sortie& Vitesse_derive_Forces::printOn(Sortie& os) const { return os; }

Entree& Vitesse_derive_Forces::readOn(Entree& is) 
{ 
  Param param(que_suis_je());
  param.ajouter("alpha_lim", &alpha_lim_);
  param.lire_avec_accolades_depuis(is);
  return Vitesse_derive_base::readOn(is); 
}

void Vitesse_derive_Forces::completer()
{
  Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, pb_.valeur());
  if (!pbm.has_correlation("frottement_interfacial")) Process::exit(que_suis_je() + " : there must be an interfacial friction correlation in the problem !");
  if (pbm.has_correlation("dispersion_bulles")) needs_grad_alpha_ = 1;
  if (pbm.has_correlation("portance_interfaciale")) needs_vort_ = 1, pbm.creer_champ("vorticite");  
}

void Vitesse_derive_Forces::evaluate_C0_vg0(const input_t& in) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, pb_.valeur());
  const int D = dimension, N = in.alpha.dimension(0);
  const double norm_g = sqrt(local_carre_norme_vect(in.g));

  // Newton method to determine dv along gravity
  double dv0 = 0.2, epsilon = 1.e-4; // Initialize dv at random
  int step = 1, iter_max = 20;
  DoubleTab p, T, dv(N, N), coeff(N, N, 2), alpha_l(N);
  const Frottement_interfacial_base& correlation_fi = ref_cast(Frottement_interfacial_base, pbm.get_correlation("frottement_interfacial").valeur());
  double sum_alpha = 0; 
  for (int n=0; n<N ; n++) alpha_l(n)= std::max(in.alpha(n), alpha_lim_), sum_alpha+=alpha_l(n);
  for (int n=0; n<N ; n++) alpha_l(n)/=sum_alpha;

  do
    {
      dv(n_g,n_l) = dv0;
      dv(n_l,n_g) = dv0;
      correlation_fi.coefficient(alpha_l, p, T, in.rho, in.mu, in.sigma, in.dh, dv, in.d_bulles, coeff);
      dv0 = dv0 - (coeff(n_l, n_g, 0)*dv0 - norm_g*alpha_l(n_g)*(in.rho[n_l]*in.alpha[n_l]+in.rho[n_g]*in.alpha[n_g] - in.rho[n_g])) / (coeff(n_l, n_g, 1)*dv0 + coeff(n_l, n_g, 0));
      step = step+1;
      if(step > iter_max) Process::exit(que_suis_je() + " : Newton algorithm not converging to find relative velocity !");
    }
  while(std::abs(coeff(n_l, n_g, 0)*dv0 - norm_g*alpha_l(n_g)*(in.rho[n_l]*in.alpha[n_l]+in.rho[n_g]*in.alpha[n_g]- in.rho[n_g])) > epsilon);

  /* distribution parameter */
  C0 = 1;

  /* drift velocity along gravity */
  for (int d = 0; d < D; d++) vg0(d) = - dv0 * in.g(d) / norm_g;

  DoubleTrav forces(D);
  if (pbm.has_correlation("dispersion_bulles"))
    {
      const Dispersion_bulles_base& correlation_db = ref_cast(Dispersion_bulles_base, pbm.get_correlation("dispersion_bulles").valeur());
      Dispersion_bulles_base::input_t in_td;
      Dispersion_bulles_base::output_t out_td;
      out_td.Ctd.resize(N,N);
      dv(n_g,n_l) = ( dv(n_l,n_g) = dv0) ;
      correlation_fi.coefficient(alpha_l, p, T, in.rho, in.mu, in.sigma, in.dh, dv, in.d_bulles, coeff);		// MAJ du coeff frottement interf
      in_td.alpha = alpha_l, in_td.rho = in.rho, in_td.mu = in.mu, in_td.sigma = in.sigma, in_td.nut = in.nut, in_td.k_turb = in.k, in_td.d_bulles = in.d_bulles, in_td.nv = dv;
      correlation_db.coefficient(in_td, out_td); // correlation identifies the liquid phase
      for (int d = 0; d < D; d++) forces(d) +=  - out_td.Ctd(n_g, n_l) * in.gradAlpha(d, n_g) + out_td.Ctd(n_l, n_g) * in.gradAlpha(d, n_l);
    }

  if (pbm.has_correlation("portance_interfaciale"))
    {
      const Portance_interfaciale_base& correlation_pi = ref_cast(Portance_interfaciale_base, pbm.get_correlation("portance_interfaciale").valeur());
      Portance_interfaciale_base::input_t in_pi;
      Portance_interfaciale_base::output_t out_pi;
      out_pi.Cl.resize(N,N);
      dv(n_g,n_l) = ( dv(n_l,n_g) = dv0) ;
      correlation_fi.coefficient(alpha_l, p, T, in.rho, in.mu, in.sigma, in.dh, dv, in.d_bulles, coeff);		// MAJ du coeff frottement interf
      in_pi.alpha = alpha_l, in_pi.rho = in.rho, in_pi.mu = in.mu, in_pi.sigma = in.sigma, in_pi.k_turb = in.k, in_pi.d_bulles = in.d_bulles, in_pi.nv = dv;
      correlation_pi.coefficient(in_pi, out_pi); // correlation identifies the liquid phase
      if (D==2)
      {
        forces(0) -= out_pi.Cl(n_l, n_g) * (- dv0 * in.g(1) / norm_g * in.vort(0, n_l)) ;
        forces(1) += out_pi.Cl(n_l, n_g) * (- dv0 * in.g(0) / norm_g * in.vort(0, n_l)) ;
      }
      if (D==3)
      {
        forces(0) -= out_pi.Cl(n_l, n_g) * (- dv0 * in.g(1) / norm_g * in.vort(2, n_l) + dv0 * in.g(2) / norm_g * in.vort(1, n_l)) ;
        forces(1) -= out_pi.Cl(n_l, n_g) * (- dv0 * in.g(2) / norm_g * in.vort(0, n_l) + dv0 * in.g(0) / norm_g * in.vort(2, n_l)) ;
        forces(2) -= out_pi.Cl(n_l, n_g) * (- dv0 * in.g(0) / norm_g * in.vort(1, n_l) + dv0 * in.g(1) / norm_g * in.vort(0, n_l)) ;
      }
    }


  for (int d = 0; d < D; d++) vg0(d) += forces(d)/coeff(n_g,n_l,0) ;
  for (int d = 0; d < D; d++) vg0(d) *=  (1.0 - C0 * in.alpha(n_g)) ;
}
