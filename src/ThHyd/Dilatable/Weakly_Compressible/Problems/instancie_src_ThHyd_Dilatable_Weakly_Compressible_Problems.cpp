//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Pb_Hydraulique_Melange_Binaire_WC.h>
#include <Pb_Thermohydraulique_WC.h>
void instancie_src_ThHyd_Dilatable_Weakly_Compressible_Problems() {
Cerr << "src_ThHyd_Dilatable_Weakly_Compressible_Problems" << finl;
Pb_Hydraulique_Melange_Binaire_WC inst1;verifie_pere(inst1);
Pb_Thermohydraulique_WC inst2;verifie_pere(inst2);
}
