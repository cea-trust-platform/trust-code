//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Correlation.h>
#include <Flux_interfacial_Coef_Constant.h>
#include <Frottement_interfacial_Wallis.h>
#include <Frottement_interfacial_bulles.h>
#include <Masse_ajoutee_Coef_Constant.h>
#include <Multiplicateur_diphasique_Friedel.h>
#include <Multiplicateur_diphasique_Lottes_Flinn.h>
void instancie_src_ThHyd_Multiphase_Correlations() {
Cerr << "src_ThHyd_Multiphase_Correlations" << finl;
Correlation inst1;verifie_pere(inst1);
Flux_interfacial_Coef_Constant inst2;verifie_pere(inst2);
Frottement_interfacial_Wallis inst3;verifie_pere(inst3);
Frottement_interfacial_bulles inst4;verifie_pere(inst4);
Masse_ajoutee_Coef_Constant inst5;verifie_pere(inst5);
Multiplicateur_diphasique_Friedel inst6;verifie_pere(inst6);
Multiplicateur_diphasique_Lottes_Flinn inst7;verifie_pere(inst7);
}
