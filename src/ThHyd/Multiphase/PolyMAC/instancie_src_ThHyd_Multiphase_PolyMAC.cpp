//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Flux_interfacial_PolyMAC.h>
#include <Frottement_interfacial_PolyMAC_V2.h>
#include <Op_Evanescence_Homogene_PolyMAC_Elem.h>
#include <Op_Evanescence_Homogene_PolyMAC_V2_Face.h>
#include <Travail_pression_PolyMAC.h>
void instancie_src_ThHyd_Multiphase_PolyMAC() {
Cerr << "src_ThHyd_Multiphase_PolyMAC" << finl;
Flux_interfacial_PolyMAC inst1;verifie_pere(inst1);
Frottement_interfacial_PolyMAC_V2 inst2;verifie_pere(inst2);
Op_Evanescence_Homogene_PolyMAC_Elem inst3;verifie_pere(inst3);
Op_Evanescence_Homogene_PolyMAC_V2_Face inst4;verifie_pere(inst4);
Travail_pression_PolyMAC inst5;verifie_pere(inst5);
}
