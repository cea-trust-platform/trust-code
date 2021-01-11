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
// File:        Flux_chaleur_parietal_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Flux_chaleur_parietal_base_included
#define Flux_chaleur_parietal_base_included
#include <DoubleTab.h>
#include <Param.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Flux_chaleur_parietal_base
//      correlations de flux de chaleur parietal de la forme
//      Phi_k = F(alpha_l, T_l, p, T_p, v_l, D_h, D_h, D_ch)
//      cette classe definit une fonction flux avec :
//    entrees :
//        N : nombre de phases
//        D_h, D_ch -> diametre hyd, diametre hyd chauffant
//        alpha[n]  -> taux de presence de la phase n
//        T[n]      -> temperature de la phase n
//        p         -> pression
//        v[n]      -> norme de la vitesse de la phase n
//        Tp        -> temperature de la paroi (une seule!)
//        lambda[n], mu[n], rho[n], rho_Cp[n] -> diverses proprietes physiques de la phase n
//
//    sorties :
//        F[n]           -> flux de chaleur vers la phase n
//       dFa[N * n + m]  -> derivee par rapport a alpha_m
//       dFp[n]          -> derivee par rapport a p
//       dFtf[N * n + m] -> derivee par rapport a T[m]
//       dFtp[n]         -> derivee par rapport a Tp
//////////////////////////////////////////////////////////////////////////////

class Flux_chaleur_parietal_base : public Objet_U
{
  Declare_base(Flux_chaleur_parietal_base);
public:
  virtual void flux(int N, double D_h, double D_ch,
                    const double *alpha, const double *T, const double p, const double *v, const double Tp,
                    const double *lambda, const double *mu, const double *rho, const double *rho_Cp,
                    double *F, double *dFa, double *dFp, double *dFv, double *dFtl, double *dFtp) const = 0;
  virtual Entree& lire(Entree& is); //appelle readOn, mais est publique!
};

#endif
