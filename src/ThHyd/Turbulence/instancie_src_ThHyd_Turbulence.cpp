//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Modele_turbulence_hyd_nul.h>
#include <Problemes_Scalaires_Passifs.h>
void instancie_src_ThHyd_Turbulence() {
Cerr << "src_ThHyd_Turbulence" << finl;
Modele_turbulence_hyd_nul inst1;verifie_pere(inst1);
Pb_Thermohydraulique_Scalaires_Passifs inst2;verifie_pere(inst2);
Pb_Conduction_Scalaires_Passifs inst3;verifie_pere(inst3);
Pb_Thermohydraulique_QC_fraction_massique inst4;verifie_pere(inst4);
Pb_Hydraulique_Concentration_Scalaires_Passifs inst5;verifie_pere(inst5);
Pb_Thermohydraulique_Concentration_Scalaires_Passifs inst6;verifie_pere(inst6);
}
