//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Champ_Don_Face_lu.h>
#include <Champ_Face.h>
#include <Champ_Fonc_Face.h>
#include <Champ_Fonc_P0_VDF.h>
#include <Champ_Fonc_P1_VDF.h>
#include <Champ_Fonc_Q1_VDF.h>
#include <Champ_Fonc_Tabule_P0_VDF.h>
#include <Champ_Ostwald_VDF.h>
#include <Champ_P0_VDF.h>
#include <Champ_front_fonc_gradient_VDF.h>
#include <Champ_som_lu_VDF.h>
#include <Champ_val_tot_sur_vol_VDF.h>
#include <Correlation_Vec_Sca_VDF.h>
#include <Courant_maille_Champ_Face.h>
#include <Critere_Q_Champ_Face.h>
#include <Reynolds_maille_Champ_Face.h>
#include <Rotationnel_Champ_Face.h>
#include <Taux_cisaillement_P0_VDF.h>
#include <Y_plus_Champ_Face.h>
void instancie_src_VDF_Champs() {
Cerr << "src_VDF_Champs" << finl;
Champ_Don_Face_lu inst1;verifie_pere(inst1);
Champ_Face inst2;verifie_pere(inst2);
Champ_Fonc_Face inst3;verifie_pere(inst3);
Champ_Fonc_P0_VDF inst4;verifie_pere(inst4);
Champ_Fonc_P1_VDF inst5;verifie_pere(inst5);
Champ_Fonc_Q1_VDF inst6;verifie_pere(inst6);
Champ_Fonc_Tabule_P0_VDF inst7;verifie_pere(inst7);
Champ_Ostwald_VDF inst8;verifie_pere(inst8);
Champ_P0_VDF inst9;verifie_pere(inst9);
Champ_front_fonc_gradient_VDF inst10;verifie_pere(inst10);
Champ_som_lu_VDF inst11;verifie_pere(inst11);
Champ_val_tot_sur_vol_VDF inst12;verifie_pere(inst12);
Correlation_Vec_Sca_VDF inst13;verifie_pere(inst13);
Courant_maille_Champ_Face inst14;verifie_pere(inst14);
Critere_Q_Champ_Face inst15;verifie_pere(inst15);
Reynolds_maille_Champ_Face inst16;verifie_pere(inst16);
Rotationnel_Champ_Face inst17;verifie_pere(inst17);
Taux_cisaillement_P0_VDF inst18;verifie_pere(inst18);
Y_plus_Champ_Face inst19;verifie_pere(inst19);
}
