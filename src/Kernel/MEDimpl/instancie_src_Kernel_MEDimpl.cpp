//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Champ_Fonc_MED.h>
#include <Champ_Fonc_P0_MED.h>
#include <Champ_Fonc_P1_MED.h>
#include <Champ_Fonc_Q1_MED.h>
#include <Champ_front_MED.h>
#include <Discr_inst.h>
#include <EcrMED.h>
#include <Ecrire_Champ_MED.h>
#include <Format_Post_Med.h>
#include <Integrer_champ_med.h>
#include <LataToMED.h>
#include <LireMED.h>
#include <Pb_MED.h>
#include <Read_MED_For_Testing_No_Verify_Option.h>
#include <ScatterMED.h>
void instancie_src_Kernel_MEDimpl() {
Cerr << "src_Kernel_MEDimpl" << finl;
Champ_Fonc_MED inst1;verifie_pere(inst1);
Champ_Fonc_P0_MED inst2;verifie_pere(inst2);
Champ_Fonc_P1_MED inst3;verifie_pere(inst3);
Champ_Fonc_Q1_MED inst4;verifie_pere(inst4);
Champ_front_MED inst5;verifie_pere(inst5);
Discr_inst inst6;verifie_pere(inst6);
EcrMED inst7;verifie_pere(inst7);
Ecrire_Champ_MED inst8;verifie_pere(inst8);
Format_Post_Med inst9;verifie_pere(inst9);
Integrer_champ_med inst10;verifie_pere(inst10);
LataToMED inst11;verifie_pere(inst11);
latatoother inst12;verifie_pere(inst12);
LireMED inst13;verifie_pere(inst13);
Pb_MED inst14;verifie_pere(inst14);
Pbc_MED inst15;verifie_pere(inst15);
Read_MED_For_Testing_No_Verify_Option inst16;verifie_pere(inst16);
ScatterMED inst17;verifie_pere(inst17);
}
