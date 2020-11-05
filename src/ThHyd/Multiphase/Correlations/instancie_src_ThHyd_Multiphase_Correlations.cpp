//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Frottement_interfacial.h>
#include <Frottement_interfacial_bulles.h>
void instancie_src_ThHyd_Multiphase_Correlations() {
Cerr << "src_ThHyd_Multiphase_Correlations" << finl;
Frottement_interfacial inst1;verifie_pere(inst1);
Frottement_interfacial_bulles inst2;verifie_pere(inst2);
}
