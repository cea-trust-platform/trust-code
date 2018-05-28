//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Conduction.h>
#include <Conduction_Milieu_Variable.h>
#include <Echange_externe_impose.h>
#include <Echange_global_impose.h>
#include <Pb_Conduction.h>
#include <Pb_Conduction_Milieu_Variable.h>
#include <Solide.h>
#include <Solide_Milieu_Variable.h>
#include <Traitement_particulier_Solide.h>
void instancie_src_ThSol() {
Cerr << "src_ThSol" << finl;
Conduction inst1;verifie_pere(inst1);
Conduction_Milieu_Variable inst2;verifie_pere(inst2);
Echange_externe_impose inst3;verifie_pere(inst3);
Echange_global_impose inst4;verifie_pere(inst4);
Pb_Conduction inst5;verifie_pere(inst5);
Pb_Conduction_Milieu_Variable inst6;verifie_pere(inst6);
Solide inst7;verifie_pere(inst7);
Solide_Milieu_Variable inst8;verifie_pere(inst8);
Traitement_particulier_Solide inst9;verifie_pere(inst9);
}
