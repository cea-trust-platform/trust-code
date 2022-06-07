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

#include <Pb_Thermohydraulique_WC.h>

Implemente_instanciable(Pb_Thermohydraulique_WC,"Pb_Thermohydraulique_WC",Pb_WC_base);
// XD pb_thermohydraulique_WC Pb_base pb_thermohydraulique_WC -1 Resolution of thermo-hydraulic problem for a weakly-compressible fluid. NL2 Keywords for the unknowns other than pressure, velocity, temperature are : NL2 masse_volumique : density NL2 pression : reduced pressure NL2 pression_tot : total pressure NL2 pression_hydro : hydro-static pressure NL2 pression_eos : pressure used in state equation.
// XD attr navier_stokes_WC navier_stokes_WC navier_stokes_WC 0 Navier-Stokes equation for a weakly-compressible fluid.
// XD attr convection_diffusion_chaleur_WC convection_diffusion_chaleur_WC convection_diffusion_chaleur_WC 0 Temperature equation for a weakly-compressible fluid.

Sortie& Pb_Thermohydraulique_WC::printOn(Sortie& os) const { return Probleme_base::printOn(os); }

Entree& Pb_Thermohydraulique_WC::readOn(Entree& is) { return Probleme_base::readOn(is); }

// Description:
//    Renvoie 2 car il y a 2 equations : Navier_Stokes_WC et Convection_Diffusion_Chaleur_WC
int Pb_Thermohydraulique_WC::nombre_d_equations() const { return 2; }

// Description:
//    Renvoie l'equation d'hydraulique de type Navier_Stokes_WC si i=0
//    Renvoie l'equation de la thermique de type Convection_Diffusion_Chaleur_WC si i=1
const Equation_base& Pb_Thermohydraulique_WC::equation(int i) const
{
  return equation_impl(i,eq_hydraulique,eq_thermique);
}

Equation_base& Pb_Thermohydraulique_WC::equation(int i)
{
  return equation_impl(i,eq_hydraulique,eq_thermique);
}

// Description:
//    Teste la compatibilite des equations de la thermique et de l'hydraulique.
int Pb_Thermohydraulique_WC::verifier()
{
  return verifier_impl(eq_hydraulique,eq_thermique, true /* is_thermal */);
}
