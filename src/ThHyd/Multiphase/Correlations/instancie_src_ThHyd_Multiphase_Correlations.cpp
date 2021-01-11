//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Flux_chaleur_parietal.h>
#include <Frottement_interfacial.h>
#include <Frottement_interfacial_bulles.h>
#include <Multiplicateur_diphasique.h>
#include <Multiplicateur_diphasique_Lottes_Flinn.h>
void instancie_src_ThHyd_Multiphase_Correlations() {
Cerr << "src_ThHyd_Multiphase_Correlations" << finl;
Flux_chaleur_parietal inst1;verifie_pere(inst1);
Frottement_interfacial inst2;verifie_pere(inst2);
Frottement_interfacial_bulles inst3;verifie_pere(inst3);
Multiplicateur_diphasique inst4;verifie_pere(inst4);
Multiplicateur_diphasique_Lottes_Flinn inst5;verifie_pere(inst5);
}
