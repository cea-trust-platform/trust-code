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

#ifndef Echange_externe_radiatif_included
#define Echange_externe_radiatif_included

#include <Echange_externe_impose.h>

static constexpr double COEFF_STEFAN_BOLTZMANN = 5.670374419e-8; // W·m⁻²·K⁻⁴

/*! @brief classe Echange_externe_radiatif: Combines radiative (sigma * eps * (T^4 - T_ext^4))
 *    and convective (h * (T - T_ext)) heat transfer boundary conditions, where sigma is the Stefan-Boltzmann constant,
 *    eps is the emissivity, h is the convective heat transfer coefficient, T is the surface temperature,
 *    and T_ext is the external temperature.
 *
 * @sa Echange_externe_impose
 */
class Echange_externe_radiatif : public Echange_externe_impose
{
  Declare_instanciable(Echange_externe_radiatif);
public:
  inline bool temperature_en_kelvin() const { return t_en_kelvin_; }

private:
  bool t_en_kelvin_ = true;
};

#endif /* Echange_externe_radiatif_included */
