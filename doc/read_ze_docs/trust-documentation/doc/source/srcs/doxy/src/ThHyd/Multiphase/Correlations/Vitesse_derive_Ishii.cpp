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

#include <Vitesse_derive_Ishii.h>
#include <cmath>

Implemente_instanciable(Vitesse_derive_Ishii, "Vitesse_relative_derive_Ishii", Vitesse_derive_base);

Sortie& Vitesse_derive_Ishii::printOn(Sortie& os) const { return Vitesse_derive_base::printOn(os); }
Entree& Vitesse_derive_Ishii::readOn(Entree& is) { return Vitesse_derive_base::readOn(is); }

void Vitesse_derive_Ishii::set_param(Param& param)
{
  param.ajouter("subcooled_boiling", &sb_, Param::REQUIRED);
  Vitesse_derive_base::set_param(param);
}

void Vitesse_derive_Ishii::evaluate_C0_vg0(const input_t& input) const
{
  const int D = dimension;
  const double norm_g = sqrt(local_carre_norme_vect(input.g));

  int N = input.v.dimension(0);
  int ind_trav = 0 ; // Et oui ! matrice triang sup !
  if (n_g>n_l) ind_trav = (n_l*(N-1)-(n_l-1)*(n_l)/2) + (n_g-n_l-1);
  else         ind_trav = (n_g*(N-1)-(n_g-1)*(n_g)/2) + (n_l-n_g-1);

  /* distribution parameter */
  C0 = ( Cinf + (1.0 - Cinf) * std::sqrt(input.rho(n_g) / input.rho(n_l)) ) * (1.0 - sb_ * exp(-zeta * input.alpha(n_g)));

  /* drift velocity */
  double dv = std::sqrt(2.0) * pow((input.rho(n_l) - input.rho(n_g)) * norm_g * input.sigma(ind_trav) / input.rho(n_l) / input.rho(n_l), 0.25) * pow(1.0 - input.alpha(n_g), theta);
  for (int d = 0; d < D; d++) vg0(d) = - dv * input.g(d) / norm_g;
}
