//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Iterateur_Source_PolyMAC.h>
#include <Perte_Charge_Anisotrope_PolyMAC_Face.h>
#include <Perte_Charge_Circulaire_PolyMAC_Face.h>
#include <Perte_Charge_Directionnelle_PolyMAC_Face.h>
#include <Perte_Charge_Isotrope_PolyMAC_Face.h>
#include <Perte_Charge_Singuliere_PolyMAC_Face.h>
#include <Terme_Boussinesq_PolyMAC_Face.h>
#include <Terme_Puissance_Thermique_PolyMAC_Elem.h>
#include <Terme_Source_Qdm_Face_PolyMAC.h>
void instancie_src_PolyMAC_Sources() {
Cerr << "src_PolyMAC_Sources" << finl;
Iterateur_Source_PolyMAC inst1;verifie_pere(inst1);
Perte_Charge_Anisotrope_PolyMAC_Face inst2;verifie_pere(inst2);
Perte_Charge_Circulaire_PolyMAC_Face inst3;verifie_pere(inst3);
Perte_Charge_Directionnelle_PolyMAC_Face inst4;verifie_pere(inst4);
Perte_Charge_Isotrope_PolyMAC_Face inst5;verifie_pere(inst5);
Perte_Charge_Singuliere_PolyMAC_Face inst6;verifie_pere(inst6);
Terme_Boussinesq_PolyMAC_Face inst7;verifie_pere(inst7);
Terme_Puissance_Thermique_PolyMAC_Elem inst8;verifie_pere(inst8);
Terme_Source_Qdm_Face_PolyMAC inst9;verifie_pere(inst9);
}
