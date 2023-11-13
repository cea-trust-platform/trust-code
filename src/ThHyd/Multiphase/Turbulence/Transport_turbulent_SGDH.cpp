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

#include <Transport_turbulent_SGDH.h>
#include <Param.h>
#include <Probleme_base.h>
#include <Champ_Don.h>
#include <Pb_Multiphase.h>
#include <TRUSTTrav.h>

Implemente_instanciable(Transport_turbulent_SGDH, "Transport_turbulent_SGDH|Transport_turbulent_isotrope", Transport_turbulent_base);
// XD type_diffusion_turbulente_multiphase_SGDH type_diffusion_turbulente_multiphase_deriv SGDH 1 not_set

Sortie& Transport_turbulent_SGDH::printOn(Sortie& os) const
{
  return os;
}

Entree& Transport_turbulent_SGDH::readOn(Entree& is)
{
  Param param(que_suis_je());
  double Pr_t = -1;
  param.ajouter("Pr_t|Prandtl_turbulent|Schmitt_turbulent", &Pr_t); // XD attr Pr_t floattant Prandtl_turbulent 1 not_set
  param.ajouter("sigma|sigma_turbulent", &sigma_); // XD_ADD_P floattant not_set
  param.ajouter("no_alpha", &no_alpha_); // XD_ADD_P flag not_set
  param.ajouter("gas_turb", &gas_turb_); // XD_ADD_P flag not_set
  param.lire_avec_accolades_depuis(is);
  if (Pr_t > 0 && sigma_ != 1) Process::exit(que_suis_je() + ": cannot specify both Pr_t and sigma!");
  if (Pr_t > 0) sigma_ = 1. / Pr_t;
  return is;
}

// Modifier_nu modifie mu : alpha et rho font partie du terme
void Transport_turbulent_SGDH::modifier_mu(const Convection_Diffusion_std& eq, const Viscosite_turbulente_base& visc_turb, DoubleTab& nu) const
{
  const DoubleTab& mu0 = eq.diffusivite_pour_transport().passe(),
                   &nu0 = eq.diffusivite_pour_pas_de_temps().passe(), //viscosites moleculaires
                    *alp = (sub_type(Pb_Multiphase, pb_.valeur()) && !no_alpha_) ? &pb_->get_champ("alpha").passe() : nullptr; //produit par alpha si Pb_Multiphase
  int i, nl = nu.dimension(0), n, N = nu.dimension(1), d, D = dimension;
  //viscosite cinematique turbulente
  DoubleTrav nu_t(nl, N);
  visc_turb.eddy_viscosity(nu_t);
  //formule pour passer de nu a mu : mu0 * sigma_ * nu_t / nu0
  if (nu.nb_dim() == 2)
    for (i = 0; i < nl; i++)
      {
        for (n = 0; n < 1; n++) //isotrope
          nu(i, n) += (alp ? (*alp)(i, n) : 1) * mu0(i, n) * sigma_ * nu_t(i, n) / nu0(i, n);
        if (gas_turb_)
          for (n = 1; n < N; n++) //isotrope
            nu(i, n) += (alp ? (*alp)(i, n) : 1) * mu0(i, n) * sigma_ / nu0(i, n) * nu_t(i, 0.)  * std::min((*alp)(i,n)*10, 1.) ;
      }
  else if (nu.nb_dim() == 3)
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++) //anisotrope diagonal
          nu(i, n, d) += (alp ? (*alp)(i, n) : 1) * mu0(i, n) * sigma_ * nu_t(i, n) / nu0(i, n);
  else for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++) //anisotrope complet
          nu(i, n, d, d) += (alp ? (*alp)(i, n) : 1) * mu0(i, n) * sigma_ * nu_t(i, n) / nu0(i, n);
}
