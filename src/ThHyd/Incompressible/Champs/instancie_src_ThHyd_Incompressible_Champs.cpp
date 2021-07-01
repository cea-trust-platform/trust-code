//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Champ_Ostwald.h>
#include <Champ_front_pression_from_u.h>
#include <Champ_front_synt.h>
void instancie_src_ThHyd_Incompressible_Champs() {
Cerr << "src_ThHyd_Incompressible_Champs" << finl;
Champ_Ostwald inst1;verifie_pere(inst1);
Champ_front_pression_from_u inst2;verifie_pere(inst2);
Champ_front_synt inst3;verifie_pere(inst3);
}
