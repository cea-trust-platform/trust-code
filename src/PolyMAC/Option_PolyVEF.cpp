/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Option_PolyVEF.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Option_PolyVEF,"Option_PolyVEF",Interprete);

// XD Option_PolyVEF interprete Option_PolyVEF 1 Class of PolyVEF options.
// XD attr interp_ve1 rien interp_ve1 1 Flag to enable a first order velocity face-to-element interpolation (the default value is 0 which means a second order interpolation)
// XD attr traitement_axi rien traitement_axi 1 Flag used to relax the time-step stability criterion in case of a thin slice geometry while modelling an axi-symetrical case

int Option_PolyVEF::interp_vf2 = 1;
int Option_PolyVEF::traitement_axi = 0;
int Option_PolyVEF::interp_postraitement_no_poro = 0;
int Option_PolyVEF::sym_as_diri = 0;

Sortie& Option_PolyVEF::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Option_PolyVEF::readOn(Entree& is) { return Interprete::readOn(is); }

Entree& Option_PolyVEF::interpreter(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_non_std("no_v_interp",(this));     //chaine Use first-order face->cell velocity interpolation. By default, it is not activated
  param.ajouter_non_std("traitement_axi",(this));
  param.ajouter_non_std("interp_postraitement_no_poro",(this));
  param.ajouter_non_std("sym_as_diri",(this));
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Option_PolyVEF::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "no_v_interp") interp_vf2 = 0;
  else if (mot == "traitement_axi") traitement_axi = 1;
  else if (mot == "interp_postraitement_no_poro") interp_postraitement_no_poro = 1;
  else if (mot == "sym_as_diri") sym_as_diri = 1;
  else return -1;
  return 1;
}
