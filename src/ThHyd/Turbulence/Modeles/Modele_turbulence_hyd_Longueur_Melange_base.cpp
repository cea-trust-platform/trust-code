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

#include <Modele_turbulence_hyd_Longueur_Melange_base.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>

Implemente_base(Modele_turbulence_hyd_Longueur_Melange_base, "Modele_turbulence_hyd_Longueur_Melange_base", Modele_turbulence_hyd_0_eq_base);

Sortie& Modele_turbulence_hyd_Longueur_Melange_base::printOn(Sortie& is) const { return Modele_turbulence_hyd_0_eq_base::printOn(is); }
Entree& Modele_turbulence_hyd_Longueur_Melange_base::readOn(Entree& is) { return Modele_turbulence_hyd_0_eq_base::readOn(is); }

void Modele_turbulence_hyd_Longueur_Melange_base::calculer_energie_cinetique_turb()
{
  // PQ : 11/08/06 :    L'estimation de k repose sur les expressions :
  //                                 - nu_t = C_mu * k^2 / eps
  //                                - eps = k^(3/2) / l
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // pour des raisons de commodite, l'estimation de k est realisee dans calculer_viscosite_turbulente()
  /////////////////////////////////////////////////////////////////////////////////////////////////

  energie_cinetique_turb_.changer_temps(mon_equation_->inconnue().temps());
}
