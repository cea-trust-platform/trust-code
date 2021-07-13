//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Source_Gravite_Weakly_Compressible_VEF.h>
#include <Source_Weakly_Compressible_Chaleur_VEF.h>
void instancie_src_ThHyd_Fluide_Dilatable_Weakly_Compressible_VEF() {
Cerr << "src_ThHyd_Fluide_Dilatable_Weakly_Compressible_VEF" << finl;
Source_Gravite_Weakly_Compressible_VEF inst1;verifie_pere(inst1);
Source_Weakly_Compressible_Chaleur_VEF inst2;verifie_pere(inst2);
}
