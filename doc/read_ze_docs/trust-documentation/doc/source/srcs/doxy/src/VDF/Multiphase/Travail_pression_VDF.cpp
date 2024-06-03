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

#include <Travail_pression_VDF.h>
#include <Operateur_base.h>
#include <Equation_base.h>
#include <Operateur.h>

Implemente_instanciable(Travail_pression_VDF, "travail_pression_VDF_P0_VDF", Source_Travail_pression_Elem_base);
// XD travail_pression source_base travail_pression 0 Source term which corresponds to the additional pressure work term that appears when dealing with compressible multiphase fluids

Sortie& Travail_pression_VDF::printOn(Sortie& os) const { return Source_Travail_pression_Elem_base::printOn(os); }
Entree& Travail_pression_VDF::readOn(Entree& is) { return Source_Travail_pression_Elem_base::readOn(is); }

void Travail_pression_VDF::completer()
{
  Source_Travail_pression_Elem_base::completer();
  alp = equation().operateur(1).l_op_base().que_suis_je().debute_par("Op_Conv_Amont");
}
