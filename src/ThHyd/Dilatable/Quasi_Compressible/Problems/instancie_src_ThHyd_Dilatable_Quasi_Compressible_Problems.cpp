//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Pb_Hydraulique_Melange_Binaire_QC.h>
#include <Pb_Thermohydraulique_QC.h>
void instancie_src_ThHyd_Dilatable_Quasi_Compressible_Problems() {
Cerr << "src_ThHyd_Dilatable_Quasi_Compressible_Problems" << finl;
Pb_Hydraulique_Melange_Binaire_QC inst1;verifie_pere(inst1);
Pb_Thermohydraulique_QC inst2;verifie_pere(inst2);
}
