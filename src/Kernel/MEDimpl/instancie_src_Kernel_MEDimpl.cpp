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
#include <EcrMEDfile.h>
#include <Ecrire_Champ_MED.h>
#include <Format_Post_Med.h>
#include <Format_Post_Med_major.h>
#include <Format_Post_Medfile.h>
#include <Integrer_champ_med.h>
#include <LataToMED.h>
#include <LireMED.h>
#include <LireMEDfile.h>
#include <Pb_MED.h>
#include <Read_MED_For_Testing_No_Verify_Option.h>
#include <ScatterMED.h>
void instancie_src_Kernel_MEDimpl() {
Cerr << "src_Kernel_MEDimpl" << finl;
Champ_Fonc_MED inst1;verifie_pere(inst1);
Champ_Fonc_MEDfile inst2;verifie_pere(inst2);
Champ_Fonc_P0_MED inst3;verifie_pere(inst3);
Champ_Fonc_P1_MED inst4;verifie_pere(inst4);
Champ_Fonc_Q1_MED inst5;verifie_pere(inst5);
Champ_front_MED inst6;verifie_pere(inst6);
Discr_inst inst7;verifie_pere(inst7);
EcrMED inst8;verifie_pere(inst8);
EcrMEDfile inst9;verifie_pere(inst9);
Ecrire_Champ_MED inst10;verifie_pere(inst10);
Format_Post_Med inst11;verifie_pere(inst11);
Format_Post_Med_major inst12;verifie_pere(inst12);
Format_Post_Medfile inst13;verifie_pere(inst13);
Integrer_champ_med inst14;verifie_pere(inst14);
LataToMED inst15;verifie_pere(inst15);
latatoother inst16;verifie_pere(inst16);
LireMED inst17;verifie_pere(inst17);
LireMEDfile inst18;verifie_pere(inst18);
Pb_MED inst19;verifie_pere(inst19);
Pbc_MED inst20;verifie_pere(inst20);
Read_MED_For_Testing_No_Verify_Option inst21;verifie_pere(inst21);
ScatterMED inst22;verifie_pere(inst22);
}
