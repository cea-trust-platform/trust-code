/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Transport_turbulent_aire_interfaciale_included
#define Transport_turbulent_aire_interfaciale_included

#include <Transport_turbulent_base.h>
#include <TRUSTTab.h>

/*! @brief classe Transport_turbulent_aire_interfaciale Transport turbulent de type aire_interfaciale:
 *
 *     < u'_i theta'> = - nu_t / Pr_t d_i theta = - sigma_t nu_t d_i theta
 *     (l'utilisateur peut donner sigma_t ou Pr_t)
 *
 *
 */
class Transport_turbulent_aire_interfaciale : public Transport_turbulent_base
{
  Declare_instanciable(Transport_turbulent_aire_interfaciale);
public:
  int dimension_min_nu() const override
  {
    return 1; //isotrope
  }
  void modifier_mu(const Convection_Diffusion_std& eq, const Viscosite_turbulente_base& visc_turb, DoubleTab& nu) const override;


private :
  int n_g2=-1 ;
  double cst_diff = 0.236 ; //2sqrt(2)/6*1/2 - ng2 : 2sqrt(2)/6*7/4
};

#endif /* Transport_turbulent_aire_interfaciale_included */

