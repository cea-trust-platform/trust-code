//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Assembleur_P_PolyMAC.h>
#include <Assembleur_P_PolyMAC_V2.h>
#include <Masse_PolyMAC_Elem.h>
#include <Masse_PolyMAC_Face.h>
#include <Masse_PolyMAC_V2_Elem.h>
#include <Masse_PolyMAC_V2_Face.h>
void instancie_src_PolyMAC_Solveurs() {
Cerr << "src_PolyMAC_Solveurs" << finl;
Assembleur_P_PolyMAC inst1;verifie_pere(inst1);
Assembleur_P_PolyMAC_V2 inst2;verifie_pere(inst2);
Masse_PolyMAC_Elem inst3;verifie_pere(inst3);
Masse_PolyMAC_Face inst4;verifie_pere(inst4);
Masse_PolyMAC_V2_Elem inst5;verifie_pere(inst5);
Masse_PolyMAC_V2_Face inst6;verifie_pere(inst6);
}
