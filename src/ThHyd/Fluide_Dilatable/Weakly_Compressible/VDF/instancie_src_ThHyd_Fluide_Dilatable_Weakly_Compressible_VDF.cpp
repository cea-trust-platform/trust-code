//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Source_Gravite_Weakly_Compressible_VDF.h>
#include <Source_Weakly_Compressible_Chaleur_VDF.h>
void instancie_src_ThHyd_Fluide_Dilatable_Weakly_Compressible_VDF() {
Cerr << "src_ThHyd_Fluide_Dilatable_Weakly_Compressible_VDF" << finl;
Source_Gravite_Weakly_Compressible_VDF inst1;verifie_pere(inst1);
Source_Weakly_Compressible_Chaleur_VDF inst2;verifie_pere(inst2);
}
