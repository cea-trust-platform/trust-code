//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Conduction.h>
#include <Echange_externe_impose.h>
#include <Echange_global_impose.h>
#include <Pb_Conduction.h>
#include <Solide.h>
#include <Traitement_particulier_Solide.h>
void instancie_src_ThSol() {
Cerr << "src_ThSol" << finl;
Conduction inst1;verifie_pere(inst1);
Echange_externe_impose inst2;verifie_pere(inst2);
Echange_global_impose inst3;verifie_pere(inst3);
Pb_Conduction inst4;verifie_pere(inst4);
Solide inst5;verifie_pere(inst5);
Traitement_particulier_Solide inst6;verifie_pere(inst6);
}
