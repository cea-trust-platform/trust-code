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

#ifndef Changement_phase_Silver_Simpson_included
#define Changement_phase_Silver_Simpson_included

#include <Changement_phase_base.h>
#include <TRUSTTabs_forward.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Correlation de changement de phase de Silver et Simpson (1949)
//    G_{kl} = lambda A_i alpha_k alpha_l sqrt(M / 2 pi R) (psat(T_k) / sqrt(T_k) - p / sqrt(T_l))
//////////////////////////////////////////////////////////////////////////////

class Changement_phase_Silver_Simpson : public Changement_phase_base
{
  Declare_instanciable(Changement_phase_Silver_Simpson);
public:
  double calculer(int k, int l, const double dh, const double *alpha, const double *T, const double p, const double *nv,
                  const double *lambda, const double *mu, const double *rho, const double *Cp, const Saturation_base& sat,
                  DoubleTab& dT_G, DoubleTab& da_G, double& dp_G) const override;
protected:
  double lambda_ec[2] = { 1, 1 }; //facteurs multiplicatifs d'evaporation / condensation
  double M; //masse molaire de la vapeur
  double alpha_min = 0.1; //taux de vide minimal (pour que le changement de phase soit possible a alpha = 0)
};

#endif
