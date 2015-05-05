//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Convection_Diffusion_Chaleur_Turbulent_QC.h>
#include <Convection_Diffusion_fraction_massique_Turbulent_QC.h>
#include <Navier_Stokes_Turbulent_QC.h>
#include <Pb_Thermohydraulique_Turbulent_QC.h>
void instancie_src_ThHyd_Quasi_Compressible_Turbulence() {
Cerr << "src_ThHyd_Quasi_Compressible_Turbulence" << finl;
Convection_Diffusion_Chaleur_Turbulent_QC inst1;verifie_pere(inst1);
Convection_Diffusion_fraction_massique_Turbulent_QC inst2;verifie_pere(inst2);
Navier_Stokes_Turbulent_QC inst3;verifie_pere(inst3);
Pb_Thermohydraulique_Turbulent_QC inst4;verifie_pere(inst4);
}
