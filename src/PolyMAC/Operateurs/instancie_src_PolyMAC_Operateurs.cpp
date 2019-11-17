//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Conv_Amont_PolyMAC_Face.h>
#include <Op_Conv_Centre_PolyMAC_Face.h>
#include <Op_Conv_EF_Stab_PolyMAC_Face.h>
#include <Op_Diff_PolyMAC_Elem.h>
#include <Op_Diff_PolyMAC_Face.h>
#include <Op_Div_PolyMAC.h>
#include <Op_Grad_PolyMAC_Face.h>
void instancie_src_PolyMAC_Operateurs() {
Cerr << "src_PolyMAC_Operateurs" << finl;
Op_Conv_Amont_PolyMAC_Face inst1;verifie_pere(inst1);
Op_Conv_Centre_PolyMAC_Face inst2;verifie_pere(inst2);
Op_Conv_EF_Stab_PolyMAC_Face inst3;verifie_pere(inst3);
Op_Diff_PolyMAC_Elem inst4;verifie_pere(inst4);
Op_Diff_PolyMAC_Face inst5;verifie_pere(inst5);
Op_Div_PolyMAC inst6;verifie_pere(inst6);
Op_Grad_PolyMAC_Face inst7;verifie_pere(inst7);
}
