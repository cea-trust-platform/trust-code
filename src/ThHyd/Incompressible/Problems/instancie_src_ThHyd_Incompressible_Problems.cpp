//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Pb_Hydraulique.h>
#include <Pb_Hydraulique_Concentration.h>
#include <Pb_Thermohydraulique.h>
#include <Pb_Thermohydraulique_Concentration.h>
#include <Problemes_Scalaires_Passifs.h>
void instancie_src_ThHyd_Incompressible_Problems() {
Cerr << "src_ThHyd_Incompressible_Problems" << finl;
Pb_Hydraulique inst1;verifie_pere(inst1);
Pb_Hydraulique_Concentration inst2;verifie_pere(inst2);
Pb_Thermohydraulique inst3;verifie_pere(inst3);
Pb_Thermohydraulique_Concentration inst4;verifie_pere(inst4);
Pb_Thermohydraulique_Concentration_Scalaires_Passifs inst5;verifie_pere(inst5);
Pb_Hydraulique_Concentration_Scalaires_Passifs inst6;verifie_pere(inst6);
Pb_Thermohydraulique_Scalaires_Passifs inst7;verifie_pere(inst7);
Pb_Conduction_Scalaires_Passifs inst8;verifie_pere(inst8);
}
