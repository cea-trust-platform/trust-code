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

#include <Deactivate_SIGINT_Catch.h>

Implemente_instanciable(Deactivate_SIGINT_Catch,"Deactivate_SIGINT_Catch",Interprete);
// XD Deactivate_SIGINT_Catch interprete Deactivate_SIGINT_Catch -1 Flag to disable the detection of the signal SIGINT.

Sortie& Deactivate_SIGINT_Catch::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Deactivate_SIGINT_Catch::readOn(Entree& is) { return Interprete::readOn(is); }

// Description:
//    Main function of the Deactivate_SIGINT_Catch interpreter
//    Set the Deactivate_SIGINT_Catch variable to 1.
Entree& Deactivate_SIGINT_Catch::interpreter(Entree& is)
{
  DEACTIVATE_SIGINT_CATCH = 1;
  return is;
}
