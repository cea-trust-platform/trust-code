//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Constituant.h>
#include <Fluide_Incompressible.h>
#include <Fluide_Ostwald.h>
#include <Fluide_base.h>
void instancie_src_ThHyd_Incompressible_Milieu() {
Cerr << "src_ThHyd_Incompressible_Milieu" << finl;
Constituant inst1;verifie_pere(inst1);
Fluide_Incompressible inst2;verifie_pere(inst2);
Fluide_Ostwald inst3;verifie_pere(inst3);
Fluide_base inst4;verifie_pere(inst4);
}
