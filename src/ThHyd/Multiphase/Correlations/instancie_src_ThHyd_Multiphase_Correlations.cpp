//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Changement_phase_Silver_Simpson.h>
#include <Correlation.h>
#include <Diametre_bulles_champ.h>
#include <Diametre_bulles_constant.h>
#include <Flux_interfacial_Coef_Constant.h>
#include <Flux_interfacial_Ranz_Marshall.h>
#include <Frottement_interfacial_Sonnenburg.h>
#include <Frottement_interfacial_Tomiyama.h>
#include <Frottement_interfacial_Wallis.h>
#include <Frottement_interfacial_Weber.h>
#include <Frottement_interfacial_bulles.h>
#include <Masse_ajoutee_Coef_Constant.h>
#include <Multiplicateur_diphasique_Friedel.h>
#include <Multiplicateur_diphasique_Lottes_Flinn.h>
#include <Multiplicateur_diphasique_Muhler_Steinhagen.h>
#include <Multiplicateur_diphasique_homogene.h>
#include <Portance_interfaciale_Tomiyama.h>
void instancie_src_ThHyd_Multiphase_Correlations() {
Cerr << "src_ThHyd_Multiphase_Correlations" << finl;
Changement_phase_Silver_Simpson inst1;verifie_pere(inst1);
Correlation inst2;verifie_pere(inst2);
Diametre_bulles_champ inst3;verifie_pere(inst3);
Diametre_bulles_constant inst4;verifie_pere(inst4);
Flux_interfacial_Coef_Constant inst5;verifie_pere(inst5);
Flux_interfacial_Ranz_Marshall inst6;verifie_pere(inst6);
Frottement_interfacial_Sonnenburg inst7;verifie_pere(inst7);
Frottement_interfacial_Tomiyama inst8;verifie_pere(inst8);
Frottement_interfacial_Wallis inst9;verifie_pere(inst9);
Frottement_interfacial_Weber inst10;verifie_pere(inst10);
Frottement_interfacial_bulles inst11;verifie_pere(inst11);
Masse_ajoutee_Coef_Constant inst12;verifie_pere(inst12);
Multiplicateur_diphasique_Friedel inst13;verifie_pere(inst13);
Multiplicateur_diphasique_Lottes_Flinn inst14;verifie_pere(inst14);
Multiplicateur_diphasique_Muhler_Steinhagen inst15;verifie_pere(inst15);
Multiplicateur_diphasique_homogene inst16;verifie_pere(inst16);
Portance_interfaciale_Tomiyama inst17;verifie_pere(inst17);
}
