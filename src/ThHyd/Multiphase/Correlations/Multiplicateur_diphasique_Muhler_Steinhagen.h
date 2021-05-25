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
// File:        Multiplicateur_diphasique_Muhler_Steinhagen.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Multiplicateur_diphasique_Muhler_Steinhagen_included
#define Multiplicateur_diphasique_Muhler_Steinhagen_included
#include <Multiplicateur_diphasique_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Multiplicateur_diphasique_Muhler_Steinhagen
//    multiplicateur diphasique par la correlation de Muhler-Steinhagen :
//      Phi^2 f_m / rho_m = (f_lo / rho_l + C (f_go / rho_g -  f_lo / rho_l) x) (1 - x)^(1/3) + f_go / rho_g x^3
//      avec C = 2 par defaut
//    - applique a la phase liquide pour alpha < alpha_min
//    - applique a la phase vapeur pour alpha > alpha_max
//////////////////////////////////////////////////////////////////////////////

class Multiplicateur_diphasique_Muhler_Steinhagen : public Multiplicateur_diphasique_base
{
  Declare_instanciable(Multiplicateur_diphasique_Muhler_Steinhagen);
public:
  virtual void coefficient(const double *alpha, const double *rho, const double *v, const double *f,
                           const double *mu, const double Dh, const double gamma, const double *Fk,
                           const double Fm, DoubleTab& coeff) const;
protected:
  double alpha_min_ = 1, alpha_max_ = 1.1, C_ = 2;
  int n_l = -1, n_g = -1, min_lottes_flinn_ = 0, min_sensas_ = 0; //indices des phases frottantes : liquide, gaz
};

#endif
