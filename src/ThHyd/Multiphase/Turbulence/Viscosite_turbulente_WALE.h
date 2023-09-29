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

#ifndef Viscosite_turbulente_WALE_included
#define Viscosite_turbulente_WALE_included

#include <Viscosite_turbulente_LES_base.h>

/*! @brief classe Viscosite_turbulente_WALE
 *
 *                                               (Sij_d * Sij_d)^1.5
 *    visc_SGS = (Cw * delta)^2 * ---------------------------------------------------,
 *                                  (Sij_bar * Sij_bar)^2.5 + (Sij_d * Sij_d)^1.25
 *
 *    avec
 *
 *        Sij_d = 0.5 * (gij_bar^2 + gji_bar^2) -1/3 gkk_bar^2,
 *        gij_bar = du_i / dx_j,
 *        gij_bar^2 = gik_bar * gkj_bar,
 *        Sij_bar = 0.5 * (du_i / dx_j + du_j / dx_i)
 *
 *    See https://imag.umontpellier.fr/~nicoud/PDF/ICFD_WALE.pdf
 */
class Viscosite_turbulente_WALE: public Viscosite_turbulente_LES_base
{
  Declare_instanciable(Viscosite_turbulente_WALE);
public:
  void eddy_viscosity(DoubleTab& nu_t) const override;
  int gradu_required() const override { return 1; }
};

#endif /* Viscosite_turbulente_WALE_included */
