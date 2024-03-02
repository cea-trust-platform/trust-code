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

#include <Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem.h>
#include <Travail_pression_PolyMAC_P0P1NC.h>

Implemente_instanciable(Travail_pression_PolyMAC_P0P1NC, "Travail_pression_Elem_PolyMAC_P0P1NC|Travail_pression_Elem_PolyMAC_P0", Source_Travail_pression_Elem_base);

Sortie& Travail_pression_PolyMAC_P0P1NC::printOn(Sortie& os) const { return Source_Travail_pression_Elem_base::printOn(os); }
Entree& Travail_pression_PolyMAC_P0P1NC::readOn(Entree& is) { return Source_Travail_pression_Elem_base::readOn(is); }

void Travail_pression_PolyMAC_P0P1NC::completer()
{
  Source_Travail_pression_Elem_base::completer();
  const Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem *op_conv = sub_type(Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem, equation().operateur(1).l_op_base()) ?
                                                       &ref_cast(Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem, equation().operateur(1).l_op_base()) : nullptr;
  alp = op_conv ? op_conv->alpha : 1; /* meme decentrement que l'operateur de convection si il existe, amont sinon */
}
