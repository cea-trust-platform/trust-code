//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Fluide_sodium_gaz.h>
#include <Fluide_sodium_liquide.h>
#include <Milieu_composite.h>
#include <Saturation.h>
#include <Saturation_constant.h>
#include <Saturation_sodium.h>
#include <StiffenedGas.h>
void instancie_src_ThHyd_Multiphase_Milieu() {
Cerr << "src_ThHyd_Multiphase_Milieu" << finl;
Fluide_sodium_gaz inst1;verifie_pere(inst1);
Fluide_sodium_liquide inst2;verifie_pere(inst2);
Milieu_composite inst3;verifie_pere(inst3);
Saturation inst4;verifie_pere(inst4);
Saturation_constant inst5;verifie_pere(inst5);
Saturation_sodium inst6;verifie_pere(inst6);
StiffenedGas inst7;verifie_pere(inst7);
}
