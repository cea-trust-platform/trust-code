/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Navier_included
#define Navier_included

#include <Cond_lim_base.h>

/*! @brief classe Navier Condition aux limites sur la vitesse de type "Navier" :
 *
 *       - composante normale de la vitesse nulle
 *       - composante tangentielle de la vitesse libre
 *       - contrainte imposee
 *     Exemples : Symetrie (contrainte nulle), Paroi_frottante (contrainte non nulle)
 *
 * @sa Cond_lim_base
 */
class Navier: public Cond_lim_base
{
  Declare_base(Navier);
public:
  int compatible_avec_eqn(const Equation_base&) const override { return 1; }

  virtual double coefficient_frottement(int ) const { return 0.; }
  virtual double coefficient_frottement(int , int ) const { return 0.; }
  virtual double coefficient_frottement_grad(int ) const { return 0.; } // Change the coefficient when calculation of gradient : nu = NULL
  virtual double coefficient_frottement_grad(int , int ) const { return 0.; }
};

#endif
