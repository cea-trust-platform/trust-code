//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Fluide_sodium_gaz.h>
#include <Fluide_sodium_liquide.h>
#include <Milieu_composite.h>
void instancie_src_ThHyd_Multiphase_Milieu() {
Cerr << "src_ThHyd_Multiphase_Milieu" << finl;
Fluide_sodium_gaz inst1;verifie_pere(inst1);
Fluide_sodium_liquide inst2;verifie_pere(inst2);
Milieu_composite inst3;verifie_pere(inst3);
}
