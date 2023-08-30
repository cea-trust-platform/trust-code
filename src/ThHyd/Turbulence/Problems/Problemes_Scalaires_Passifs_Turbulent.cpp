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

#include <Problemes_Scalaires_Passifs_Turbulent.h>

Implemente_instanciable(Pb_Thermohydraulique_Turbulent_Scalaires_Passifs,"Pb_Thermohydraulique_Turbulent_Scalaires_Passifs",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent>);
Sortie& Pb_Thermohydraulique_Turbulent_Scalaires_Passifs::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent>::printOn(os); }
Entree& Pb_Thermohydraulique_Turbulent_Scalaires_Passifs::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent>::readOn(is); }

Implemente_instanciable(Pb_Thermohydraulique_Especes_Turbulent_QC,"Pb_Thermohydraulique_Especes_Turbulent_QC",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent_QC>);
Sortie& Pb_Thermohydraulique_Especes_Turbulent_QC::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent_QC>::printOn(os); }
Entree& Pb_Thermohydraulique_Especes_Turbulent_QC::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent_QC>::readOn(is); }

Implemente_instanciable(Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs,"Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs",TRUSTProblem_sup_eqns<Pb_Hydraulique_Concentration_Turbulent>);
Sortie& Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Hydraulique_Concentration_Turbulent>::printOn(os); }
Entree& Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Hydraulique_Concentration_Turbulent>::readOn(is); }

Implemente_instanciable(Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs,"Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Concentration_Turbulent>);
Sortie& Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Concentration_Turbulent>::printOn(os); }
Entree& Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Concentration_Turbulent>::readOn(is); }

// XD pb_thermohydraulique_turbulent_scalaires_passifs pb_avec_passif pb_thermohydraulique_turbulent_scalaires_passifs -1 Resolution of thermohydraulic problem, with turbulence modelling and with the additional passive scalar equations.
// XD attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD attr constituant constituant constituant 1 Constituents.
// XD attr navier_stokes_turbulent navier_stokes_turbulent navier_stokes_turbulent 1 Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent 1 Energy equations (temperature diffusion convection) as well as the associated turbulence model equations.

// XD pb_thermohydraulique_especes_turbulent_qc pb_avec_passif pb_thermohydraulique_especes_turbulent_qc -1 Resolution of turbulent thermohydraulic problem under low Mach number with passive scalar equations.
// XD attr fluide_quasi_compressible fluide_quasi_compressible fluide_quasi_compressible 0 The fluid medium associated with the problem.
// XD attr navier_stokes_turbulent_qc navier_stokes_turbulent_qc navier_stokes_turbulent_qc 0 Navier-Stokes equations under low Mach number as well as the associated turbulence model equations.
// XD attr convection_diffusion_chaleur_turbulent_qc convection_diffusion_chaleur_turbulent_qc convection_diffusion_chaleur_turbulent_qc 0 Energy equation under low Mach number as well as the associated turbulence model equations.

// XD pb_hydraulique_concentration_turbulent_scalaires_passifs pb_avec_passif pb_hydraulique_concentration_turbulent_scalaires_passifs -1 Resolution of Navier-Stokes/multiple constituent transport equations, with turbulence modelling and with the additional passive scalar equations.
// XD attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD attr constituant constituant constituant 1 Constituents.
// XD attr navier_stokes_turbulent navier_stokes_turbulent navier_stokes_turbulent 1 Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent 1 Constituent transport equations (concentration diffusion convection) as well as the associated turbulence model equations.

// XD pb_thermohydraulique_concentration_turbulent_scalaires_passifs pb_avec_passif pb_thermohydraulique_concentration_turbulent_scalaires_passifs -1 Resolution of Navier-Stokes/energy/multiple constituent transport equations, with turbulence modelling and with the additional passive scalar equations.
// XD attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD attr constituant constituant constituant 1 Constituents.
// XD attr navier_stokes_turbulent navier_stokes_turbulent navier_stokes_turbulent 1 Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent convection_diffusion_concentration_turbulent 1 Constituent transport equations (concentration diffusion convection) as well as the associated turbulence model equations.
// XD attr convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent 1 Energy equations (temperature diffusion convection) as well as the associated turbulence model equations.
