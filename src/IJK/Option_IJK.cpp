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

#include <Option_IJK.h>
#include <Param.h>

Implemente_instanciable(Option_IJK, "Option_IJK", Interprete);
// XD Option_IJK interprete Option_IJK 1 Class of IJK options.

int Option_IJK::CHECK_DIVERGENCE = 0;
int Option_IJK::DISABLE_DIPHASIQUE = 0;
int Option_IJK::CORRECTION_PARCOURS_THOMAS = 0;
int Option_IJK::PARCOURS_SANS_TOLERANCE = 0;

Sortie& Option_IJK::printOn(Sortie& os) const { return Interprete::printOn(os); }
Entree& Option_IJK::readOn(Entree& is) { return Interprete::readOn(is); }

Entree& Option_IJK::interpreter(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_flag("check_divergence", &CHECK_DIVERGENCE); // XD_ADD_P rien Flag to compute and print the value of div(u) after each pressure-correction
  param.ajouter_flag("disable_diphasique", &DISABLE_DIPHASIQUE); // XD_ADD_P rien Disable all calculations related to interfaces (phase properties, interfacial force, ... )
  param.ajouter_flag("correction_parcours_thomas", &CORRECTION_PARCOURS_THOMAS); // XD_ADD_P rien Flag to fix parcours when vertex are exactly on the faces of the elements.
  param.ajouter_flag("parcours_sans_tolerance", &PARCOURS_SANS_TOLERANCE); // XD_ADD_P rien Flag to fix parcours when vertex are exactly on the faces of the elements.
  param.lire_avec_accolades_depuis(is);
  return is;
}
