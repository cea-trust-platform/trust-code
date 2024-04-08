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

#include <Problemes_List_Concentration_Turbulent.h>

using Pb_List_Concentration_Gen1 = TRUSTProblem_List_Concentration_Gen<Pb_Thermohydraulique_Turbulent, Convection_Diffusion_Concentration_Turbulent, Constituant>;
using Pb_List_Concentration_Gen2 = TRUSTProblem_List_Concentration_Gen<Pb_Hydraulique_Turbulent, Convection_Diffusion_Concentration_Turbulent, Constituant>;

Implemente_instanciable(Pb_Thermohydraulique_List_Concentration_Turbulent, "Pb_Thermohydraulique_List_Concentration_Turbulent", Pb_List_Concentration_Gen1);
Sortie& Pb_Thermohydraulique_List_Concentration_Turbulent::printOn(Sortie& os) const { return Pb_List_Concentration_Gen1::printOn(os); }
Entree& Pb_Thermohydraulique_List_Concentration_Turbulent::readOn(Entree& is) { return Pb_List_Concentration_Gen1::readOn(is); }

Implemente_instanciable(Pb_Hydraulique_List_Concentration_Turbulent, "Pb_Hydraulique_List_Concentration_Turbulent", Pb_List_Concentration_Gen2);
Sortie& Pb_Hydraulique_List_Concentration_Turbulent::printOn(Sortie& os) const { return Pb_List_Concentration_Gen2::printOn(os); }
Entree& Pb_Hydraulique_List_Concentration_Turbulent::readOn(Entree& is) { return Pb_List_Concentration_Gen2::readOn(is); }

// XD Pb_Thermohydraulique_List_Concentration_Turbulent Pb_base Pb_Thermohydraulique_List_Concentration_Turbulent -1 Resolution of Navier-Stokes/energy/multiple constituent transport equations, with turbulence modelling.
// XD attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD attr constituant constituant constituant 1 Constituents.
// XD attr navier_stokes_turbulent navier_stokes_turbulent navier_stokes_turbulent 1 Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent 1 Constituent transport equations (concentration diffusion convection) as well as the associated turbulence model equations.
// XD attr convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent 1 Energy equation (temperature diffusion convection) as well as the associated turbulence model equations.

// XD Pb_Hydraulique_List_Concentration_Turbulent Pb_base Pb_Hydraulique_List_Concentration_Turbulent -1 Resolution of Navier-Stokes/multiple constituent transport equations, with turbulence modelling.
// XD attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD attr constituant constituant constituant 1 Constituents.
// XD attr navier_stokes_turbulent navier_stokes_turbulent navier_stokes_turbulent 1 Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent 1 Constituent transport equations (concentration diffusion convection) as well as the associated turbulence model equations.

