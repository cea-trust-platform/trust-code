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

#include <Neumann_paroi_flux_nul.h>

Implemente_instanciable(Neumann_paroi_flux_nul, "Paroi", Neumann_paroi_adiabatique);
// XD Paroi condlim_base Paroi 0 Impermeability condition at a wall called bord (edge) (standard flux zero). This condition must be associated with a wall type hydraulic condition.


Sortie& Neumann_paroi_flux_nul::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Neumann_paroi_flux_nul::readOn(Entree& s )
{
  if (app_domains.size() == 0)
    app_domains = {Motcle("Concentration"), Motcle("Fraction_massique"),
                   Motcle("Transport_Keps"), Motcle("Transport_Keps_V2"),
                   Motcle("Interfacial_area"), Motcle("Transport_Keps_Rea"),
                   Motcle("Transport_V2"), Motcle("Diphasique_moyenne"),
                   Motcle("Fraction_volumique"), Motcle("indetermine"),
                   Motcle("Turbulence"), Motcle("Transport_Komega")
                  };

  return Neumann_homogene::readOn(s) ;
}
