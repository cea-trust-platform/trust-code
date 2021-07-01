//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Convection_Diffusion_Chaleur_QC.h>
#include <Convection_Diffusion_fraction_massique_MB_QC.h>
#include <Convection_Diffusion_fraction_massique_QC.h>
#include <Navier_Stokes_QC.h>
void instancie_src_ThHyd_Fluide_Dilatable_Quasi_Compressible_Equations() {
Cerr << "src_ThHyd_Fluide_Dilatable_Quasi_Compressible_Equations" << finl;
Convection_Diffusion_Chaleur_QC inst1;verifie_pere(inst1);
Convection_Diffusion_fraction_massique_MB_QC inst2;verifie_pere(inst2);
Convection_Diffusion_fraction_massique_QC inst3;verifie_pere(inst3);
Navier_Stokes_QC inst4;verifie_pere(inst4);
}
