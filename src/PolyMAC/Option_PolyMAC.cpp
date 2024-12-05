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

#include <Option_PolyMAC.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Option_PolyMAC,"Option_PolyMAC",Interprete);

// XD Option_PolyMAC interprete Option_PolyMAC 1 Class of PolyMAC options.

int Option_PolyMAC::USE_NEW_M2 = 1;
int Option_PolyMAC::MAILLAGE_VDF = 0;
int Option_PolyMAC::INTERP_VE1 = 0;
int Option_PolyMAC::TRAITEMENT_AXI = 0;

Sortie& Option_PolyMAC::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Option_PolyMAC::readOn(Entree& is) { return Interprete::readOn(is); }

Entree& Option_PolyMAC::interpreter(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_non_std("use_osqp",(this)); // XD_ADD_P rien Flag to use the old formulation of the M2 matrix provided by the OSQP library. Only useful for PolyMAC version.
  param.ajouter_non_std("maillage_vdf|vdf_mesh",(this)); // XD_ADD_P rien Flag used to force the calculation of the equiv tab.
  param.ajouter_non_std("interp_ve1",(this)); // XD_ADD_P rien Flag to enable a first-order face-to-element velocity interpolation. By default, it is not activated which means a second order interpolation. Only useful for PolyMAC_P0 version.
  param.ajouter_non_std("traitement_axi",(this)); // XD_ADD_P rien Flag used to relax the time-step stability criterion in case of a thin slice geometry while modelling an axi-symetrical case. Only useful for PolyMAC_P0 version.
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Option_PolyMAC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "use_osqp")
    USE_NEW_M2 = 0;
  else if (mot == "maillage_vdf" || mot == "vdf_mesh")
    MAILLAGE_VDF = 1;
  else if (mot == "interp_ve1")
    INTERP_VE1 = 1;
  else if (mot == "traitement_axi")
    TRAITEMENT_AXI = 1;
  else
    return -1;
  return 1;
}
