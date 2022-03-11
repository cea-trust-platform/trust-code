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
// File:        Changement_phase_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Changement_phase_base_included
#define Changement_phase_base_included

#include <Correlation_base.h>
#include <Saturation_base.h>
#include <TRUSTTab.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Changement_phase_base
//      correlations de changement de phase
//      G_{kl} = G(alpha, p, T) (a remplir pour k liquide, l vapeur)
//      cette classe definit une fonction flux avec :
//    entrees :
//        k, l      -> paire de phases
//        D_h       -> diametre hyd
//        alpha[n]  -> taux de presence de la phase n
//        T[n]      -> temperature de la phase n
//        p         -> pression
//        nv[n]     -> norme de la vitesse de la phase n
//        lambda[n], mu[n], rho[n], Cp[n] -> diverses proprietes physiques de la phase n
//        sat       -> objet "saturation" associe au couple (k, l)
//
//    sorties :
//    retour  -> flux de masse G de k vers l
//    dT_G(n) -> derivee de G en T[n]
//    da_G(n) -> derivee de G en a[n]
//    dp_G    -> derivee de G en p
//////////////////////////////////////////////////////////////////////////////

class Changement_phase_base : public Correlation_base
{
  Declare_base(Changement_phase_base);
public:
  virtual double calculer(int k, int l, const double& dh, const double *alpha, const double *T, const double p, const double *nv,
                          const double *lambda, const double *mu, const double *rho, const double *Cp, const Saturation_base& sat,
                          DoubleTab& dT_G, DoubleTab& da_G, double& dp_G) const = 0;
};

#endif
