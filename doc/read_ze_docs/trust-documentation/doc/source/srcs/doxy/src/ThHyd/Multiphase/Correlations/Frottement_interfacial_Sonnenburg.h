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

#ifndef Frottement_interfacial_Sonnenburg_included
#define Frottement_interfacial_Sonnenburg_included
#include <Frottement_interfacial_base.h>

/*! @brief classe Frottement_interfacial_Sonnenburg coefficients de frottement interfacial correspondant a la correlation
 *
 *       de "drift flux" de Sonnenburg, modifiee par Bissen, Alpy et Medale
 *       pour presenter les bonnes limites en alpha -> 0 et alpha -> 1
 *       parametres : non!
 *
 *
 */

class Frottement_interfacial_Sonnenburg : public Frottement_interfacial_base
{
  Declare_instanciable(Frottement_interfacial_Sonnenburg);
public:
  void coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                   const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma, double Dh,
                   const DoubleTab& ndv, const DoubleTab& d_bulles, DoubleTab& coeff) const override;
protected:
  int n_l = -1, n_g = -1; //phases traitees : liquide / gaz continu
};

#endif
