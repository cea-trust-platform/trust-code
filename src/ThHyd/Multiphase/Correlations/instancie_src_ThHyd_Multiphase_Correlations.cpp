//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Changement_phase_Silver_Simpson.h>
#include <Correlation.h>
#include <Flux_interfacial_Coef_Constant.h>
#include <Frottement_interfacial_Wallis.h>
#include <Frottement_interfacial_bulles.h>
#include <Masse_ajoutee_Coef_Constant.h>
#include <Multiplicateur_diphasique_Friedel.h>
#include <Multiplicateur_diphasique_Lottes_Flinn.h>
void instancie_src_ThHyd_Multiphase_Correlations() {
Cerr << "src_ThHyd_Multiphase_Correlations" << finl;
Changement_phase_Silver_Simpson inst1;verifie_pere(inst1);
Correlation inst2;verifie_pere(inst2);
Flux_interfacial_Coef_Constant inst3;verifie_pere(inst3);
Frottement_interfacial_Wallis inst4;verifie_pere(inst4);
Frottement_interfacial_bulles inst5;verifie_pere(inst5);
Masse_ajoutee_Coef_Constant inst6;verifie_pere(inst6);
Multiplicateur_diphasique_Friedel inst7;verifie_pere(inst7);
Multiplicateur_diphasique_Lottes_Flinn inst8;verifie_pere(inst8);
}
