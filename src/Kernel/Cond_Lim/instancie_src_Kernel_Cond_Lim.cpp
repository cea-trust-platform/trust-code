//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Scalaire_impose_paroi.h>
#include <Symetrie.h>
void instancie_src_Kernel_Cond_Lim() {
Cerr << "src_Kernel_Cond_Lim" << finl;
Scalaire_impose_paroi inst1;verifie_pere(inst1);
Symetrie inst2;verifie_pere(inst2);
}
