//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Integrale_tps_Champ.h>
#include <Integrale_tps_produit_champs.h>
#include <Moyenne_volumique.h>
#include <Op_Correlation.h>
#include <Op_Ecart_type.h>
#include <Op_Moyenne.h>
#include <Operateur_Statistique_tps.h>
#include <Operateurs_Statistique_tps.h>
void instancie_src_Kernel_Statistiques_temps() {
Cerr << "src_Kernel_Statistiques_temps" << finl;
Integrale_tps_Champ inst1;verifie_pere(inst1);
Integrale_tps_produit_champs inst2;verifie_pere(inst2);
Moyenne_volumique inst3;verifie_pere(inst3);
Op_Correlation inst4;verifie_pere(inst4);
Op_Ecart_type inst5;verifie_pere(inst5);
Op_Moyenne inst6;verifie_pere(inst6);
Operateur_Statistique_tps inst7;verifie_pere(inst7);
Operateurs_Statistique_tps inst8;verifie_pere(inst8);
}
