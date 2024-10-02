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

#include <Temperature_imposee_paroi.h>

Implemente_instanciable(Temperature_imposee_paroi, "Temperature_imposee_paroi|Enthalpie_imposee_paroi", Scalaire_impose_paroi);
// XD temperature_imposee_paroi paroi_temperature_imposee temperature_imposee_paroi 0 Imposed temperature condition at the wall called bord (edge).

Sortie& Temperature_imposee_paroi::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Temperature_imposee_paroi::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique"), Motcle("indetermine") };
  if (supp_discs.size() == 0) supp_discs = { Nom("VEF"), Nom("EF"), Nom("EF_axi"), Nom("VEF_P1_P1"), Nom("VEFPreP1B"), Nom("PolyMAC"), Nom("PolyMAC_P0P1NC"), Nom("PolyMAC_P0"), Nom("PolyVEF_P0"), Nom("PolyVEF_P0P1"), Nom("PolyVEF_P0P1NC")   };

  return Dirichlet::readOn(s);
}
