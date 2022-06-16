//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Dirichlet_paroi_defilante.h>
#include <Scalaire_impose_paroi.h>
#include <Symetrie.h>
void instancie_src_Kernel_Cond_Lim() {
Cerr << "src_Kernel_Cond_Lim" << finl;
Dirichlet_paroi_defilante inst1;verifie_pere(inst1);
Scalaire_impose_paroi inst2;verifie_pere(inst2);
Symetrie inst3;verifie_pere(inst3);
}
