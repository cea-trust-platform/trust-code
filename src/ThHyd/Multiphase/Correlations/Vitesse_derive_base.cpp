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
// File:        Vitesse_derive_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Vitesse_derive_base.h>
Implemente_base(Vitesse_derive_base, "Vitesse_relative_derive_base", Vitesse_relative_base);
// XD vitesse_derive source_base frottement_interfacial 0 Source term which corresponds to the drift-velocity between a liquid and a gas phase

Sortie& Vitesse_derive_base::printOn(Sortie& os) const { return Vitesse_relative_base::printOn(os); }
Entree& Vitesse_derive_base::readOn(Entree& is)
{
  vg0.resize(dimension);
  return Vitesse_relative_base::readOn(is);
}

void Vitesse_derive_base::vitesse_relative(const double Dh, const DoubleTab& sigma, const DoubleTab& alpha, const DoubleTab& rho, const DoubleTab& v, const DoubleVect& g, DoubleTab& ur) const
{
  ur = 0.0;
  // if (alpha(n_g) < 1e-7) return;
  evaluate_C0_vg0(Dh, sigma, alpha, rho, v, g);

  for (int d = 0; d < dimension; d++)
    {
      ur(n_g, n_l, d) = ((C0 - 1.0) * v(n_l, d) + vg0(d)) / (1.0 - C0 * alpha(n_g));
      ur(n_l, n_g, d) = ur(n_g, n_l, d);
    }
}
