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

#include <Problemes_Scalaires_Passifs.h>

implemente_pb_eq_sup(Pb_Thermohydraulique_Concentration_Scalaires_Passifs,"Pb_Thermohydraulique_Concentration_Scalaires_Passifs",Pb_Thermohydraulique_Concentration,3)
implemente_pb_eq_sup(Pb_Hydraulique_Concentration_Scalaires_Passifs,"Pb_Hydraulique_Concentration_Scalaires_Passifs",Pb_Hydraulique_Concentration,2)
implemente_pb_eq_sup(Pb_Thermohydraulique_Scalaires_Passifs,"Pb_Thermohydraulique_Scalaires_Passifs",Pb_Thermohydraulique,2)
implemente_pb_eq_sup(Pb_Conduction_Scalaires_Passifs,"Pb_Conduction_Scalaires_Passifs",Pb_Conduction,1)

// XD pb_avec_passif Pb_base pb_avec_passif -1 Class to create a classical problem with a scalar transport equation (e.g: temperature or concentration) and an additional set of passive scalars (e.g: temperature or concentration) equations.
// XD attr equations_scalaires_passifs listeqn equations_scalaires_passifs 0 Passive scalar equations. The unknowns of the passive scalar equation number N are named temperatureN or concentrationN or fraction_massiqueN. This keyword is used to define initial conditions and the post processing fields. This kind of problem is very useful to test in only one data file (and then only one calculation) different schemes or different boundary conditions for the scalar transport equation.

// XD pb_thermohydraulique_concentration_scalaires_passifs pb_avec_passif pb_thermohydraulique_concentration_scalaires_passifs -1 Resolution of Navier-Stokes/energy/multiple constituent transport equations, with the additional passive scalar equations.
// XD attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD attr convection_diffusion_concentration convection_diffusion_concentration convection_diffusion_concentration 1 Constituent transport equations (concentration diffusion convection).
// XD attr convection_diffusion_temperature convection_diffusion_temperature convection_diffusion_temperature 1 Energy equations (temperature diffusion convection).
// XD ref constituant constituant

// XD pb_thermohydraulique_scalaires_passifs pb_avec_passif pb_thermohydraulique_scalaires_passifs -1 Resolution of thermohydraulic problem, with the additional passive scalar equations.
// XD attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD attr convection_diffusion_temperature convection_diffusion_temperature convection_diffusion_temperature 1 Energy equations (temperature diffusion convection).

// XD pb_hydraulique_concentration_scalaires_passifs pb_avec_passif pb_hydraulique_concentration_scalaires_passifs -1 Resolution of Navier-Stokes/multiple constituent transport equations with the additional passive scalar equations.
// XD attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD attr convection_diffusion_concentration convection_diffusion_concentration convection_diffusion_concentration 1 Constituent transport equations (concentration diffusion convection).
// XD ref constituant constituant

