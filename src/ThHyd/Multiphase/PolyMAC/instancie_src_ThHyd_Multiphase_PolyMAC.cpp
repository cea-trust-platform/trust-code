//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Dispersion_bulles_PolyMAC_P0.h>
#include <Flux_interfacial_PolyMAC.h>
#include <Force_Tchen_PolyMAC_P0.h>
#include <Frottement_interfacial_PolyMAC_P0.h>
#include <Op_Evanescence_Homogene_PolyMAC_Elem.h>
#include <Op_Evanescence_Homogene_PolyMAC_P0_Face.h>
#include <Portance_interfaciale_PolyMAC_P0.h>
#include <Travail_pression_PolyMAC.h>
void instancie_src_ThHyd_Multiphase_PolyMAC() {
Cerr << "src_ThHyd_Multiphase_PolyMAC" << finl;
Dispersion_bulles_PolyMAC_P0 inst1;verifie_pere(inst1);
Flux_interfacial_PolyMAC inst2;verifie_pere(inst2);
Force_Tchen_PolyMAC_P0 inst3;verifie_pere(inst3);
Frottement_interfacial_PolyMAC_P0 inst4;verifie_pere(inst4);
Op_Evanescence_Homogene_PolyMAC_Elem inst5;verifie_pere(inst5);
Op_Evanescence_Homogene_PolyMAC_P0_Face inst6;verifie_pere(inst6);
Portance_interfaciale_PolyMAC_P0 inst7;verifie_pere(inst7);
Travail_pression_PolyMAC inst8;verifie_pere(inst8);
}
