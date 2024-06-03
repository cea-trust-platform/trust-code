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

#ifndef Frottement_interfacial_Rusche_included
#define Frottement_interfacial_Rusche_included
#include <Frottement_interfacial_base.h>
#include <Correlation.h>

/*! @brief classe Frottement_interfacial_Rusche coefficients de frottement interfacial d'un ecoulement a bulles en prennant en compte la densite
 *
 *
 *
 */
class Frottement_interfacial_Rusche : public Frottement_interfacial_base
{
  Declare_instanciable(Frottement_interfacial_Rusche);
public:
  void coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                   const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma, double Dh,
                   const DoubleTab& ndv, const DoubleTab& d_bulles, DoubleTab& coeff) const override;

protected:

  int n_l = -1; //liquid phase
  double K1_ = 3.64, K2_ = 0.864 ;
  Correlation frottement_bulle_seule_ ;
};

#endif
