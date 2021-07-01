//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Pb_Hydraulique.h>
#include <Pb_Hydraulique_Concentration.h>
#include <Pb_Thermohydraulique.h>
#include <Pb_Thermohydraulique_Concentration.h>
void instancie_src_ThHyd_Incompressible_Problems() {
Cerr << "src_ThHyd_Incompressible_Problems" << finl;
Pb_Hydraulique inst1;verifie_pere(inst1);
Pb_Hydraulique_Concentration inst2;verifie_pere(inst2);
Pb_Thermohydraulique inst3;verifie_pere(inst3);
Pb_Thermohydraulique_Concentration inst4;verifie_pere(inst4);
}
