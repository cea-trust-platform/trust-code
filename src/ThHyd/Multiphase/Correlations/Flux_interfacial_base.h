/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <DoubleTab.h>
#include <Correlation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Flux_interfacial_base
//      correlations de flux de chaleur interfacial de la forme
//      Phi_k = F(alpha_l, T_l, p, Ti, v_l, D_h, D_h)
//      cette classe definit une fonction flux avec :
//    entrees :
//        N : nombre de phases
//        D_h       -> diametre hyd
//        alpha[n]  -> taux de presence de la phase n
//        T[n]      -> temperature de la phase n
//        p         -> pression
//        v[n]      -> norme de la vitesse de la phase n
//        Ti        -> temperature de l'interface
//        lambda[n], mu[n], rho[n], rho_Cp[n] -> diverses proprietes physiques de la phase n
//
//    sorties :
//        F[n]           -> flux de chaleur vers la phase n
//       dFa[N * n + m]  -> derivee par rapport a alpha_m
//       dFp[n]          -> derivee par rapport a p
//       dFtf[N * n + m] -> derivee par rapport a T[m]
//////////////////////////////////////////////////////////////////////////////

class Flux_interfacial_base : public Correlation_base
{
  Declare_base(Flux_interfacial_base);
public:
  virtual void flux(const double al, const double ag, const double Tl, const double Tg, const double Ti, const double dP_Ti,
                    double& Fl, double& Fg, double& dTl_Fl, double& dTg_Fg, double& dP_Fl, double& dP_Fg) const = 0;
};

#endif
