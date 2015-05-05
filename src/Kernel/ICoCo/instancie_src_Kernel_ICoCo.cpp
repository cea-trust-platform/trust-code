//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Pilote_ICoCo.h>
#include <Testeur_MEDCoupling.h>
void instancie_src_Kernel_ICoCo() {
Cerr << "src_Kernel_ICoCo" << finl;
Pilote_ICoCo inst1;verifie_pere(inst1);
Testeur_MEDCoupling inst2;verifie_pere(inst2);
}
