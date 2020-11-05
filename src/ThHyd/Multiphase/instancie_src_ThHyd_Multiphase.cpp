//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Entree_fluide_alpha_impose.h>
#include <Pb_Multiphase.h>
#include <Phases.h>
void instancie_src_ThHyd_Multiphase() {
Cerr << "src_ThHyd_Multiphase" << finl;
Entree_fluide_alpha_impose inst1;verifie_pere(inst1);
Pb_Multiphase inst2;verifie_pere(inst2);
Phases inst3;verifie_pere(inst3);
}
