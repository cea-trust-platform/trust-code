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

#ifndef Masse_ajoutee_base_included
#define Masse_ajoutee_base_included

#include <TRUSTTabs_forward.h>
#include <Correlation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Masse_ajoutee_base
//      masse ajoutee de la forme
//      alpha_k rho_k Dv_k / Dt -> alpha_k rho_k Dv_k / Dt + sum_l ma(k, l) Dv_l / Dt
//      cette classe definit une fonction calculer avec :
//    entrees :
//        alpha[n]  -> taux de presence de la phase n
//        rho[n]    -> masse volumique de la phase n
//
//    entree / sortie :
//       a_r(k, l)   -> a mettre dans l'equation de qdm (par defaut : alpha(k) * rho(k) pour la phase k)
//////////////////////////////////////////////////////////////////////////////

class Masse_ajoutee_base : public Correlation_base
{
  Declare_base(Masse_ajoutee_base);
public:
  virtual void ajouter(const double *alpha, const double *rho, DoubleTab& a_r) const = 0;
};

#endif
