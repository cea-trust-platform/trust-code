//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Iterateur_PolyMAC_base.h>
#include <Op_Conv_Amont_PolyMAC_Elem.h>
#include <Op_Conv_Centre_PolyMAC_Elem.h>
void instancie_src_PolyMAC_Operateurs_Conv_iterateur() {
Cerr << "src_PolyMAC_Operateurs_Conv_iterateur" << finl;
Iterateur_PolyMAC inst1;verifie_pere(inst1);
Op_Conv_Amont_PolyMAC_Elem inst2;verifie_pere(inst2);
Op_Conv_Centre_PolyMAC_Elem inst3;verifie_pere(inst3);
}
