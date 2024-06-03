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

#include <Pb_Multi_Especes.h>

Implemente_instanciable(Pb_Thermohydraulique_Especes_QC,"Pb_Thermohydraulique_Especes_QC",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_QC>);
Sortie& Pb_Thermohydraulique_Especes_QC::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_QC>::printOn(os); }
Entree& Pb_Thermohydraulique_Especes_QC::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_QC>::readOn(is); }

Implemente_instanciable(Pb_Thermohydraulique_Especes_WC,"Pb_Thermohydraulique_Especes_WC",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_WC>);
Sortie& Pb_Thermohydraulique_Especes_WC::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_WC>::printOn(os); }
Entree& Pb_Thermohydraulique_Especes_WC::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_WC>::readOn(is); }

// XD pb_thermohydraulique_especes_QC pb_avec_passif pb_thermohydraulique_especes_QC -1 Resolution of thermo-hydraulic problem for a multi-species quasi-compressible fluid.
// XD attr fluide_quasi_compressible fluide_quasi_compressible fluide_quasi_compressible 0 The fluid medium associated with the problem.
// XD attr navier_stokes_QC navier_stokes_QC navier_stokes_QC 0 Navier-Stokes equation for a quasi-compressible fluid.
// XD attr convection_diffusion_chaleur_QC convection_diffusion_chaleur_QC convection_diffusion_chaleur_QC 0 Temperature equation for a quasi-compressible fluid.

// XD pb_thermohydraulique_especes_WC pb_avec_passif pb_thermohydraulique_especes_WC -1 Resolution of thermo-hydraulic problem for a multi-species weakly-compressible fluid.
// XD attr fluide_weakly_compressible fluide_weakly_compressible fluide_weakly_compressible 0 The fluid medium associated with the problem.
// XD attr navier_stokes_WC navier_stokes_WC navier_stokes_WC 0 Navier-Stokes equation for a weakly-compressible fluid.
// XD attr convection_diffusion_chaleur_WC convection_diffusion_chaleur_WC convection_diffusion_chaleur_WC 0 Temperature equation for a weakly-compressible fluid.

