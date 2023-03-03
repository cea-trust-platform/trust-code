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
// File:        Vitesse_derive_Ishii.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Vitesse_derive_Ishii.h>
#include <cmath>

Implemente_instanciable(Vitesse_derive_Ishii, "Vitesse_relative_derive_Ishii", Vitesse_derive_base);

Sortie& Vitesse_derive_Ishii::printOn(Sortie& os) const { return os; }

Entree& Vitesse_derive_Ishii::readOn(Entree& is)
{
  Vitesse_derive_base::readOn(is);

  Param param(que_suis_je());
  param.ajouter("subcooled_boiling", &sb_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Vitesse_derive_Ishii::evaluate_C0_vg0(const double Dh, const DoubleTab& sigma, const DoubleTab& alpha, const DoubleTab& rho, const DoubleTab& v, const DoubleVect& g) const
{
  const int D = dimension;
  const double norm_g = sqrt(local_carre_norme_vect(g));

  /* distribution parameter */
  C0 = Cinf + (1.0 - Cinf) * std::sqrt(rho(n_g) / rho(n_l)) * (1.0 - sb_ * exp(-zeta * alpha(n_g)));

  /* drift velocity */
  double dv = std::sqrt(2.0) * pow((rho(n_l) - rho(n_g)) * norm_g * sigma(n_l, n_g) / rho(n_l) / rho(n_l), 0.25) * pow(1.0 - alpha(n_g), theta);
  for (int d = 0; d < D; d++) vg0(d) = dv * std::fabs(g(d)) / norm_g;
}
