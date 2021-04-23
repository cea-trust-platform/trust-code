//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Flux_interfacial_CoviMAC.h>
#include <Frottement_interfacial_CoviMAC.h>
#include <Op_Evanescence_Homogene_CoviMAC_Elem.h>
#include <Op_Evanescence_Homogene_CoviMAC_Face.h>
#include <Travail_pression_CoviMAC.h>
void instancie_src_ThHyd_Multiphase_CoviMAC() {
Cerr << "src_ThHyd_Multiphase_CoviMAC" << finl;
Flux_interfacial_CoviMAC inst1;verifie_pere(inst1);
Frottement_interfacial_CoviMAC inst2;verifie_pere(inst2);
Op_Evanescence_Homogene_CoviMAC_Elem inst3;verifie_pere(inst3);
Op_Evanescence_Homogene_CoviMAC_Face inst4;verifie_pere(inst4);
Travail_pression_CoviMAC inst5;verifie_pere(inst5);
}
