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
#include <Pb_Multiphase.h>

Implemente_base(Viscosite_turbulente_base, "Viscosite_turbulente_base", Correlation_base);
// XD type_diffusion_turbulente_multiphase_deriv objet_lecture type_diffusion_turbulente_multiphase_deriv -1 not_set

// XD diffusion_turbulente_multiphase diffusion_deriv turbulente 0 Turbulent diffusion operator for multiphase problem
// XD attr type type_diffusion_turbulente_multiphase_deriv type 1 Turbulence model for multiphase problem

Sortie& Viscosite_turbulente_base::printOn(Sortie& os) const {return os;}

Entree& Viscosite_turbulente_base::readOn(Entree& is) {return is;}

void Viscosite_turbulente_base::modifier_mu(DoubleTab& mu) const
{
  const DoubleTab& rho = pb_->get_champ("masse_volumique").passe(),
                   *alpha = pb_->has_champ("alpha") ? &(pb_->get_champ("alpha").passe()) : nullptr;
  int i, nl = mu.dimension(0), n, N = rho.line_size(), cR = rho.dimension(0) == 1, d, D = dimension;
  DoubleTrav nu_t(nl, N); //viscosite turbulente : toujours scalaire
  eddy_viscosity(nu_t); //remplissage par la correlation
  if (mu.nb_dim() == 2) //nu scalaire
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++) mu(i, n) += (alpha ? (*alpha)(i, n) * rho(!cR * i, n) : 1) * nu_t(i, n);
  else if (mu.nb_dim() == 3) //nu anisotrope diagonal
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++) mu(i, n, d) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_t(i, n);
  else if (mu.nb_dim() == 4) //nu anisotrope complet
    for (i = 0; i < nl; i++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++) mu(i, n, d, d) += (alpha ? (*alpha)(i, n) : 1) * rho(!cR * i, n) * nu_t(i, n);
  else abort();
  mu.echange_espace_virtuel();
}
