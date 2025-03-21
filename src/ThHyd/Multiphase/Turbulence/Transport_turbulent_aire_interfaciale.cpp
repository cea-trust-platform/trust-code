/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Transport_turbulent_aire_interfaciale.h>
#include <Param.h>
#include <Probleme_base.h>

#include <Pb_Multiphase.h>
#include <TRUSTTrav.h>

Implemente_instanciable(Transport_turbulent_aire_interfaciale, "Transport_turbulent_aire_interfaciale", Transport_turbulent_base);
// XD type_diffusion_turbulente_multiphase_aire_interfaciale type_diffusion_turbulente_multiphase_deriv aire_interfaciale 1 not_set

Sortie& Transport_turbulent_aire_interfaciale::printOn(Sortie& os) const
{
  return os;
}

Entree& Transport_turbulent_aire_interfaciale::readOn(Entree& is)
{
  return is;
}

// Modifier_nu modifie mu : alpha et rho font partie du terme
void Transport_turbulent_aire_interfaciale::modifier_mu(const Convection_Diffusion_std& eq, const Viscosite_turbulente_base& visc_turb, DoubleTab& nu) const
{
  const DoubleTab& rho = eq.milieu().masse_volumique().passe();
  int  nl = nu.dimension(0), N = nu.dimension(1), D = dimension;

  //viscosite cinematique turbulente
  DoubleTrav nu_t(nl, N);
  visc_turb.eddy_viscosity(nu_t); /* on a nu turb */

  if (nu.nb_dim() == 2)
    for (int i = 0; i < nl; i++)
      {
        for (int n = 0; n < 1; n++) //isotrope
          nu(i, n) = rho(i, n) * nu_t(i, n) / 0.405;
      }
  else if (nu.nb_dim() == 3)
    for (int i = 0; i < nl; i++)
      for (int n = 0; n < N; n++)
        for (int d = 0; d < D; d++) //anisotrope diagonal
          nu(i, n, d) = rho(i, n) * nu_t(i, n) / 0.405;
  else
    for (int i = 0; i < nl; i++)
      for (int n = 0; n < N; n++)
        for (int d = 0; d < D; d++) //anisotrope complet
          nu(i, n, d, d) = rho(i, n) * nu_t(i, n) / 0.405;
}
