//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Frottement_interfacial_CoviMAC.h>
#include <Op_Evanescence_Homogene_CoviMAC_Elem.h>
#include <Op_Evanescence_Homogene_CoviMAC_Face.h>
void instancie_src_ThHyd_Multiphase_CoviMAC() {
Cerr << "src_ThHyd_Multiphase_CoviMAC" << finl;
Frottement_interfacial_CoviMAC inst1;verifie_pere(inst1);
Op_Evanescence_Homogene_CoviMAC_Elem inst2;verifie_pere(inst2);
Op_Evanescence_Homogene_CoviMAC_Face inst3;verifie_pere(inst3);
}
