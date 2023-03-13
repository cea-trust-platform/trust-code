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

#include <Vitesse_derive_base.h>

Implemente_base(Vitesse_derive_base, "Vitesse_relative_derive_base", Vitesse_relative_base);
// XD vitesse_derive_base vitesse_relative_base vitesse_derive_base 0 Source term which corresponds to the drift-velocity between a liquid and a gas phase

Sortie& Vitesse_derive_base::printOn(Sortie& os) const { return Vitesse_relative_base::printOn(os); }
Entree& Vitesse_derive_base::readOn(Entree& is)
{
  vg0.resize(dimension);
  return Vitesse_relative_base::readOn(is);
}

void Vitesse_derive_base::vitesse_relative(const input_t& input, output_t& output) const
{
  output.vr = 0.0;
  output.dvr= 0.0;

  evaluate_C0_vg0(input); // No dependency in v in evaluate_C0_vg0 => No need for derivative

  for (int d = 0; d < dimension; d++)
    {
      output.vr(n_g, n_l, d) = ((C0 - 1.0) * input.v(d, n_l) + vg0(d)) / (1.0 - C0 * input.alpha(n_g));
      output.vr(n_l, n_g, d) = - output.vr(n_g, n_l, d);

      output.dvr(n_g, n_l, d, dimension*n_l+d) =  (C0 - 1.0) / (1.0 - C0 * input.alpha(n_g));
      output.dvr(n_l, n_g, d, dimension*n_l+d) = -(C0 - 1.0) / (1.0 - C0 * input.alpha(n_g));
    }
}
