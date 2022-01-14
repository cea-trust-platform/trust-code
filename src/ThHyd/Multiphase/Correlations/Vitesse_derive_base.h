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
// File:        Vitesse_derive_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Vitesse_derive_base_included
#define Vitesse_derive_base_included
#include <DoubleTab.h>
#include <Correlation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Vitesse_derive_base
//      correlations de vitesse de derive de la forme
//      v_d = (v_k - v_l)
//		cette classe definit les composantes de la vitesse de derives v_dx, v_dy, v_dz avec en parametres d'entree :
//        D_h       -> diametre hydraulique
//        g         -> gravite
//        rho[n]  	-> densite des phases
//
//    sorties :
//       C0(k, l)		   -> coefficient de distribution par rapport a la vitesse moyenne des deux phases k et l (v = rho[n_l]*v[n_l] + rho[n_g]*v[n_g])
//       vg0(k, l, x/y/z)  -> vitesse de separation du gaz (a trois composantes)
//
//////////////////////////////////////////////////////////////////////////////

class Vitesse_derive_base : public Correlation_base
{
  Declare_base(Vitesse_derive_base);
public:
  virtual void vitesse_derive(const double& Dh, const DoubleTab& alpha, const DoubleTab& rho, const DoubleTab& g, DoubleTab& C0, DoubleTab& vg0) const = 0;
};

#endif
