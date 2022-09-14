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

#include <Masse_ajoutee_Coef_Constant.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Masse_ajoutee_Coef_Constant, "Masse_ajoutee_Coef_Constant", Masse_ajoutee_base);

Sortie& Masse_ajoutee_Coef_Constant::printOn(Sortie& os) const
{
  return os;
}

Entree& Masse_ajoutee_Coef_Constant::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("beta", &beta);
  param.ajouter("inj_ajoutee_liquide", &inj_ajoutee_liquide_);
  param.ajouter("inj_ajoutee_gaz", &inj_ajoutee_gaz_);
  param.lire_avec_accolades_depuis(is);

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  return is;
}

void Masse_ajoutee_Coef_Constant::ajouter(const double *alpha, const double *rho, DoubleTab& a_r) const
{
  int k, N = a_r.dimension(0);
  for (k = 0; k < N; k++)
    if (n_l != k)
      {
        a_r(k,  k ) += std::min(beta * rho[n_l] * alpha[k], limiter_liquid_ * rho[n_l] * alpha[n_l]) ;
        a_r(k, n_l) -= std::min(beta * rho[n_l] * alpha[k], limiter_liquid_ * rho[n_l] * alpha[n_l]) ;
        a_r(n_l,n_l)+= std::min(beta * rho[n_l] * alpha[k], limiter_liquid_ * rho[n_l] * alpha[n_l]) ;
        a_r(n_l, k) -= std::min(beta * rho[n_l] * alpha[k], limiter_liquid_ * rho[n_l] * alpha[n_l]) ;
      }
}

void Masse_ajoutee_Coef_Constant::coefficient(  const double *alpha, const double *rho, DoubleTab& coeff) const
{
  int k, N = coeff.dimension(0);
  for (k = 0; k < N; k++)
    if (n_l != k)
      {
        coeff(k) = (alpha[k]>1.e-6) ? std::min(beta, limiter_liquid_ * alpha[n_l] /  alpha[k]) : beta ;
      }
}


void Masse_ajoutee_Coef_Constant::ajouter_inj(const double *flux_alpha, const double *alpha,  const double *rho, DoubleTab& f_a_r) const
{
  int N = f_a_r.dimension(0);
  for (int k = 0; k < N; k++)
    if (n_l != k)
      {
        double flux_ma = (alpha[k] < 1.e-3) ? beta * rho[n_l] * flux_alpha[k] : std::min(beta * rho[n_l] * flux_alpha[k], limiter_liquid_ * rho[n_l] * alpha[n_l] * flux_alpha[k] / alpha[k]) ;
        f_a_r(k,  k ) += inj_ajoutee_gaz_ * flux_ma ;
        f_a_r(k, n_l) -= 0.;
        f_a_r(n_l,n_l)+= 0.;
        f_a_r(n_l, k) -= inj_ajoutee_liquide_ * flux_ma ; //beta * rho[n_l] * flux_alpha[k];
      }
}
