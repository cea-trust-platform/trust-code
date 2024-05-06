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

#include <Option_Interpolation.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Option_Interpolation, "Option_Interpolation", Interprete);
// XD Option_Interpolation interprete Option_Interpolation 1 Class for interpolation fields using MEDCoupling.

int Option_Interpolation::USE_DEC = 1;
int Option_Interpolation::SHARING_ALGO = 0;

Sortie& Option_Interpolation::printOn(Sortie& os) const { return Interprete::printOn(os); }
Entree& Option_Interpolation::readOn(Entree& is) { return Interprete::readOn(is); }

Entree& Option_Interpolation::interpreter(Entree& is)
{
  int no_dec = false;

  Param param(que_suis_je());
  param.ajouter_flag("sans_dec|without_dec", &no_dec); // XD_ADD_P rien Use remapper even for a parallel calculation
  param.ajouter("sharing_algo", &SHARING_ALGO); // XD_ADD_P entier Setting the DEC sharing algo : 0,1,2
  param.lire_avec_accolades_depuis(is);

  if(SHARING_ALGO < 0 || SHARING_ALGO > 2)
    {
      Cerr << "Option_Interpolation : wrong sharing_algo read : " << SHARING_ALGO << finl;
      Cerr << "Available values are 0, 1 or 2 ! See the MEDCoupling Doc ! "  << finl;
      Process::exit();
    }

  if (no_dec) /* for experts only ;) */
    {
      Cerr << "Forcing the usage of MEDCouplingRemapper ! " << finl;
      USE_DEC = 0;
    }
  else
    USE_DEC = 1;

  return is;
}

