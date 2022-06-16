//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Source_WC_Chaleur_VDF.h>
#include <Source_WC_Gravite_VDF.h>
void instancie_src_ThHyd_Dilatable_Weakly_Compressible_VDF() {
Cerr << "src_ThHyd_Dilatable_Weakly_Compressible_VDF" << finl;
Source_WC_Chaleur_VDF inst1;verifie_pere(inst1);
Source_WC_Gravite_VDF inst2;verifie_pere(inst2);
}
