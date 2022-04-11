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
// File:        Flux_interfacial_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Flux_interfacial_base_included
#define Flux_interfacial_base_included

#include <TRUSTTabs_forward.h>
#include <Correlation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Flux_interfacial_base
//      correlations de flux de chaleur interfacial de la forme
//      Phi_{kl} = h_{kl}(T_k - T_l)
//      cette classe definit une fonction flux avec :
//    entrees :
//        D_h       -> diametre hyd
//        alpha[n]  -> taux de presence de la phase n
//        T[n]      -> temperature de la phase n
//        p         -> pression
//        nv[n]     -> norme de la vitesse de la phase n
//        lambda[n], mu[n], rho[n], Cp[n] -> diverses proprietes physiques de la phase n
//
//    sorties :
//       hi(k, l)    -> coeff d'echange entre la phase k et l'interface avec la phase l (hi(l, k) != hi(k, l) !)
//    dT_hi(k, l, n) -> derivee de hi(k, l) en T[n]
//    da_hi(k, l, n) -> derivee de hi(k, l) en a[n]
//    dp_hi(k, l)    -> derivee de hi(k, l) en p
//////////////////////////////////////////////////////////////////////////////

class Flux_interfacial_base : public Correlation_base
{
  Declare_base(Flux_interfacial_base);
public:
  virtual void coeffs(const double dh, const double *alpha, const double *T, const double p, const double *nv,
                      const double *lambda, const double *mu, const double *rho, const double *Cp,
                      DoubleTab& hi, DoubleTab& dT_hi, DoubleTab& da_hi, DoubleTab& dp_hi) const = 0;
};

#endif
