//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Iterateur_CoviMAC_base.h>
#include <Op_Conv_Amont_CoviMAC_Elem.h>
#include <Op_Conv_Centre_CoviMAC_Elem.h>
void instancie_src_CoviMAC_Operateurs_Conv_iterateur() {
Cerr << "src_CoviMAC_Operateurs_Conv_iterateur" << finl;
Iterateur_CoviMAC inst1;verifie_pere(inst1);
Op_Conv_Amont_CoviMAC_Elem inst2;verifie_pere(inst2);
Op_Conv_Centre_CoviMAC_Elem inst3;verifie_pere(inst3);
}
