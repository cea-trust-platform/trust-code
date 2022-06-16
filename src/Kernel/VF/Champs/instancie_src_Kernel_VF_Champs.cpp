//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Champ_Fonc_reprise.h>
#include <Champ_front_recyclage.h>
#include <Champ_front_softanalytique.h>
void instancie_src_Kernel_VF_Champs() {
Cerr << "src_Kernel_VF_Champs" << finl;
Champ_Fonc_reprise inst1;verifie_pere(inst1);
Champ_front_recyclage inst2;verifie_pere(inst2);
Champ_front_softanalytique inst3;verifie_pere(inst3);
}
