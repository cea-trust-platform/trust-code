//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <DP_Impose_PolyMAC_Face.h>
#include <Iterateur_Source_PolyMAC.h>
#include <Perte_Charge_Anisotrope_PolyMAC_Face.h>
#include <Perte_Charge_Circulaire_PolyMAC_Face.h>
#include <Perte_Charge_Directionnelle_PolyMAC_Face.h>
#include <Perte_Charge_Isotrope_PolyMAC_Face.h>
#include <Perte_Charge_Singuliere_PolyMAC_Face.h>
#include <Source_Generique_Face_PolyMAC.h>
#include <Terme_Boussinesq_PolyMAC_Face.h>
#include <Terme_Puissance_Thermique_Echange_Impose_P0_PolyMAC.h>
#include <Terme_Puissance_Thermique_PolyMAC_Elem.h>
#include <Terme_Source_Constituant_PolyMAC_Elem.h>
#include <Terme_Source_Decroissance_Radioactive_P0_PolyMAC.h>
#include <Terme_Source_Qdm_Face_PolyMAC.h>
void instancie_src_PolyMAC_Sources() {
Cerr << "src_PolyMAC_Sources" << finl;
DP_Impose_PolyMAC_Face inst1;verifie_pere(inst1);
Iterateur_Source_PolyMAC inst2;verifie_pere(inst2);
Perte_Charge_Anisotrope_PolyMAC_Face inst3;verifie_pere(inst3);
Perte_Charge_Circulaire_PolyMAC_Face inst4;verifie_pere(inst4);
Perte_Charge_Directionnelle_PolyMAC_Face inst5;verifie_pere(inst5);
Perte_Charge_Isotrope_PolyMAC_Face inst6;verifie_pere(inst6);
Perte_Charge_Singuliere_PolyMAC_Face inst7;verifie_pere(inst7);
Source_Generique_Face_PolyMAC inst8;verifie_pere(inst8);
Terme_Boussinesq_PolyMAC_Face inst9;verifie_pere(inst9);
Terme_Puissance_Thermique_Echange_Impose_P0_PolyMAC inst10;verifie_pere(inst10);
Terme_Puissance_Thermique_PolyMAC_Elem inst11;verifie_pere(inst11);
Terme_Source_Constituant_PolyMAC_Elem inst12;verifie_pere(inst12);
Terme_Source_Decroissance_Radioactive_P0_PolyMAC inst13;verifie_pere(inst13);
Terme_Source_Qdm_Face_PolyMAC inst14;verifie_pere(inst14);
}
