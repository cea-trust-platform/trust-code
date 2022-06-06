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

#ifndef Frottement_interfacial_base_included
#define Frottement_interfacial_base_included

#include <TRUSTTabs_forward.h>
#include <Correlation_base.h>
#include <TRUSTTab.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Frottement_interfacial_base
//      utilitaire pour les operateurs de frottement interfacial prenant la forme
//      F_{kl} = - F_{lk} = - C_{kl} (u_k - u_l)
//      cette classe definit une fonction C_{kl} dependant de :
//        alpha, p, T -> inconnues (une valeur par phase chacune)
//        rho, mu, sigma -> proprietes physiques (idem)
//        ndv(k, l) -> ||v_k - v_l||, a remplir pour k < l
//    sortie :
//        coeff(k, l, 0/1) -> coefficient C_{kl} et sa derivee en ndv(k, l), rempli pour k < l
//////////////////////////////////////////////////////////////////////////////

class Frottement_interfacial_base : public Correlation_base
{
  Declare_base(Frottement_interfacial_base);
public:
  virtual void coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                           const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma, double Dh,
                           const DoubleTab& ndv, const DoubleTab& d_bulles, DoubleTab& coeff) const  = 0;
};

#endif
