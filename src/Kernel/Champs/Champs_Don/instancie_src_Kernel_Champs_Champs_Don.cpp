//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Ch_input_uniforme.h>
#include <Champ_Don_Fonc_txyz.h>
#include <Champ_Don_Fonc_xyz.h>
#include <Champ_Don_lu.h>
#include <Champ_Fonc_Fonction_txyz_Morceaux.h>
#include <Champ_Fonc_Morceaux.h>
#include <Champ_Fonc_Tabule_Morceaux.h>
#include <Champ_Fonc_t.h>
#include <Champ_Tabule_Temps.h>
#include <Champ_Uniforme.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Uniforme_Morceaux_Tabule_Temps.h>
#include <Init_par_partie.h>
#include <Tayl_Green.h>
#include <champ_init_canal_sinal.h>
void instancie_src_Kernel_Champs_Champs_Don() {
Cerr << "src_Kernel_Champs_Champs_Don" << finl;
Ch_input_uniforme inst1;verifie_pere(inst1);
Champ_Don_Fonc_txyz inst2;verifie_pere(inst2);
Champ_Don_Fonc_xyz inst3;verifie_pere(inst3);
Champ_Don_lu inst4;verifie_pere(inst4);
Champ_Fonc_Fonction_txyz_Morceaux inst5;verifie_pere(inst5);
Champ_Fonc_Morceaux inst6;verifie_pere(inst6);
Champ_Fonc_Tabule_Morceaux inst7;verifie_pere(inst7);
Champ_Fonc_t inst8;verifie_pere(inst8);
Champ_Tabule_Temps inst9;verifie_pere(inst9);
Champ_Uniforme inst10;verifie_pere(inst10);
Champ_Uniforme_Morceaux inst11;verifie_pere(inst11);
Champ_Uniforme_Morceaux_Tabule_Temps inst12;verifie_pere(inst12);
Init_par_partie inst13;verifie_pere(inst13);
Tayl_Green inst14;verifie_pere(inst14);
champ_init_canal_sinal inst15;verifie_pere(inst15);
}
