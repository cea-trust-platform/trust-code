//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Fluide_Dilatable.h>
#include <Loi_Etat.h>
void instancie_src_ThHyd_Fluide_Dilatable() {
Cerr << "src_ThHyd_Fluide_Dilatable" << finl;
Fluide_Dilatable inst1;verifie_pere(inst1);
Loi_Etat inst2;verifie_pere(inst2);
}
