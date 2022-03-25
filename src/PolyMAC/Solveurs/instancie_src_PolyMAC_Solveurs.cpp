//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Assembleur_P_PolyMAC.h>
#include <Masse_PolyMAC_Elem.h>
#include <Masse_PolyMAC_Face.h>
void instancie_src_PolyMAC_Solveurs() {
Cerr << "src_PolyMAC_Solveurs" << finl;
Assembleur_P_PolyMAC inst1;verifie_pere(inst1);
Masse_PolyMAC_Elem inst2;verifie_pere(inst2);
Masse_PolyMAC_Face inst3;verifie_pere(inst3);
}
