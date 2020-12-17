/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Lois_sodium_liquide.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Lois_sodium_liquide_included
#define Lois_sodium_liquide_included
#include <Lois_milieu_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Lois_sodium_liquide
//      utilitaire pour les operateurs de frottement interfacial prenant la forme
//      F_{kl} = - F_{lk} = - C_{kl} (u_k - u_l)
//      cette classe definit une fonction C_{kl} dependant de :
//        alpha, p, T -> inconnues (une valeur par phase chacune)
//        rho, mu, sigma -> proprietes physiques (idem)
//        dv_abs(i, k, l) -> i-eme ecart ||v_k - v_l||, a remplir pour k < l
//    sortie :
//        coeff(i, k, l) -> i_eme coefficient C_{kl}, rempli pour k < l
//////////////////////////////////////////////////////////////////////////////

class Lois_sodium_liquide : virtual public Lois_milieu_base
{
public:
  virtual ~Lois_sodium_liquide() = default;
  // densite
  virtual double    rho_(const double T, const double P) const;
  virtual double dP_rho_(const double T, const double P) const;
  virtual double dT_rho_(const double T, const double P) const;
  // capacite calorifique
  virtual double     cp_(const double T, const double P) const;
  virtual double  dP_cp_(const double T, const double P) const;
  virtual double  dT_cp_(const double T, const double P) const;
  // lois champs "faibles" -> pas de derivees
  virtual double     mu_(const double T) const;
  virtual double lambda_(const double T) const;
  virtual double   beta_(const double T) const;
};

#endif
