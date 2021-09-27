//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Source_WC_Chaleur_VEF.h>
#include <Source_WC_Gravite_VEF.h>
void instancie_src_ThHyd_Dilatable_Weakly_Compressible_VEF() {
Cerr << "src_ThHyd_Dilatable_Weakly_Compressible_VEF" << finl;
Source_WC_Chaleur_VEF inst1;verifie_pere(inst1);
Source_WC_Gravite_VEF inst2;verifie_pere(inst2);
}
