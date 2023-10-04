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

#include <Vitesse_derive_constante.h>

Implemente_instanciable(Vitesse_derive_constante, "Vitesse_relative_derive_constante", Vitesse_derive_base);

Sortie& Vitesse_derive_constante::printOn(Sortie& os) const { return Vitesse_derive_base::printOn(os); }
Entree& Vitesse_derive_constante::readOn(Entree& is) { return Vitesse_derive_base::readOn(is); }

void Vitesse_derive_constante::set_param(Param& param)
{
  param.ajouter("C0", &C0, Param::REQUIRED);
  param.ajouter("vg0_x", &vg0[0], Param::REQUIRED);
  param.ajouter("vg0_y", &vg0[1], Param::REQUIRED);
  if (dimension == 3) param.ajouter("vg0_z", &vg0[2], Param::REQUIRED);
  Vitesse_derive_base::set_param(param);
}
