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

#include <Transport_turbulent_Prandtl.h>
#include <Champ_Don.h>
#include <Param.h>

Implemente_instanciable(Transport_turbulent_Prandtl, "Transport_turbulent_Prandtl", Transport_turbulent_base);

Sortie& Transport_turbulent_Prandtl::printOn(Sortie& os) const
{
  return os;
}

Entree& Transport_turbulent_Prandtl::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("Pr_t|Prandtl_turbulent", &prdl_);
  param.lire_avec_accolades_depuis(is);

  if (prdl_ < 0.) Process::exit("The turbulent Prandlt's number must be positive !");
  else Cerr << "Transport_turbulent_Prandtl model used with turbulent Prandlt's number = " << prdl_ << finl;

  return is;
}

// Modifier_nu modifie mu : alpha et rho font partie du terme
void Transport_turbulent_Prandtl::modifier_mu(const Convection_Diffusion_std& eq, const Viscosite_turbulente_base& visc_turb, DoubleTab& nu) const
{
  const DoubleTab& mu0 = eq.diffusivite_pour_transport().passe(), &nu0 = eq.diffusivite_pour_pas_de_temps().passe(); //viscosites moleculaires

  int nl = nu.dimension(0), N = nu.dimension(1), D = dimension;

  //viscosite cinematique turbulente
  DoubleTrav nu_t(nl, N);
  visc_turb.eddy_viscosity(nu_t);

  //formule pour passer de nu a mu : mu0 * sigma_ * nu_t / nu0
  if (nu.nb_dim() == 2)
    for (int i = 0; i < nl; i++)
      for (int n = 0; n < 1; n++) //isotrope
        nu(i, n) += mu0(i, n) * nu_t(i, n) / nu0(i, n) / prdl_;
  else if (nu.nb_dim() == 3)
    for (int i = 0; i < nl; i++)
      for (int n = 0; n < N; n++)
        for (int d = 0; d < D; d++) //anisotrope diagonal
          nu(i, n, d) += mu0(i, n) * nu_t(i, n) / nu0(i, n) / prdl_;
  else
    for (int i = 0; i < nl; i++)
      for (int n = 0; n < N; n++)
        for (int d = 0; d < D; d++) //anisotrope complet
          nu(i, n, d, d) += mu0(i, n) * nu_t(i, n) / nu0(i, n) / prdl_;
}
