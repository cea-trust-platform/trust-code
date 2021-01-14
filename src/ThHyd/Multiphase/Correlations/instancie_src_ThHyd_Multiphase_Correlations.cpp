//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Correlation.h>
#include <Frottement_interfacial_bulles.h>
#include <Multiplicateur_diphasique_Lottes_Flinn.h>
void instancie_src_ThHyd_Multiphase_Correlations() {
Cerr << "src_ThHyd_Multiphase_Correlations" << finl;
Correlation inst1;verifie_pere(inst1);
Frottement_interfacial_bulles inst2;verifie_pere(inst2);
Multiplicateur_diphasique_Lottes_Flinn inst3;verifie_pere(inst3);
}
