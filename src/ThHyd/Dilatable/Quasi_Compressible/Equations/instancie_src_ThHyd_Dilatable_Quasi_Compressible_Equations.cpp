//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Convection_Diffusion_Chaleur_QC.h>
#include <Convection_Diffusion_Espece_Binaire_QC.h>
#include <Convection_Diffusion_Espece_Multi_QC.h>
#include <Navier_Stokes_QC.h>
void instancie_src_ThHyd_Dilatable_Quasi_Compressible_Equations() {
Cerr << "src_ThHyd_Dilatable_Quasi_Compressible_Equations" << finl;
Convection_Diffusion_Chaleur_QC inst1;verifie_pere(inst1);
Convection_Diffusion_Espece_Binaire_QC inst2;verifie_pere(inst2);
Convection_Diffusion_Espece_Multi_QC inst3;verifie_pere(inst3);
Navier_Stokes_QC inst4;verifie_pere(inst4);
}
