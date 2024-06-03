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

#ifndef Multiplicateur_diphasique_base_included
#define Multiplicateur_diphasique_base_included

#include <TRUSTTabs_forward.h>
#include <Correlation_base.h>

/*! @brief classe Multiplicateur_diphasique_base correlations de multiplicateurs diphasiques de la forme
 *
 *       F_{kp} = - C_{kp} F_{p, k seul} - C'_{kp} F_{p,melange}
 *     entrees :
 *       alpha : taux de presence
 *       rho   : masses volumique
 *         v   : vitesses (pour calcul du titre)
 *         f   : facteurs de Darcy qu'aurait l'ecoulement si tout le debit etait dans la phase k
 *        mu   : viscosites cinematiques
 *        Dh   : diametre hydraulique
 *     gamma   : tension superficielle
 *       F_k   : F_{p, k seul}
 *       F_m   : F_{p, melange}
 *     sortie :
 *         coeff(k, 0/1) -> coefficients C_{kp} et C'_{kp}
 *
 *
 */

class Multiplicateur_diphasique_base : public Correlation_base
{
  Declare_base(Multiplicateur_diphasique_base);
public:
  virtual void coefficient(const double *alpha, const double *rho, const double *v, const double *f,
                           const double *mu, const double Dh, const double gamma, const double *Fk,
                           const double Fm, DoubleTab& coeff) const  = 0;
};

#endif
