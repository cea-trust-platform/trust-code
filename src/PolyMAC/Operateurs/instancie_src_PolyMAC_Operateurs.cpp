//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Conv_EF_Stab_PolyMAC_Elem.h>
#include <Op_Conv_EF_Stab_PolyMAC_Face.h>
#include <Op_Conv_EF_Stab_PolyMAC_V2_Face.h>
#include <Op_Diff_PolyMAC_Elem.h>
#include <Op_Diff_PolyMAC_Face.h>
#include <Op_Diff_PolyMAC_V2_Elem.h>
#include <Op_Diff_PolyMAC_V2_Face.h>
#include <Op_Div_PolyMAC.h>
#include <Op_Div_PolyMAC_V2.h>
#include <Op_Grad_PolyMAC_Face.h>
#include <Op_Grad_PolyMAC_V2_Face.h>
void instancie_src_PolyMAC_Operateurs() {
Cerr << "src_PolyMAC_Operateurs" << finl;
Op_Conv_EF_Stab_PolyMAC_Elem inst1;verifie_pere(inst1);
Op_Conv_Amont_PolyMAC_Elem inst2;verifie_pere(inst2);
Op_Conv_Centre_PolyMAC_Elem inst3;verifie_pere(inst3);
Op_Conv_EF_Stab_PolyMAC_Face inst4;verifie_pere(inst4);
Op_Conv_Amont_PolyMAC_Face inst5;verifie_pere(inst5);
Op_Conv_Centre_PolyMAC_Face inst6;verifie_pere(inst6);
Op_Conv_EF_Stab_PolyMAC_V2_Face inst7;verifie_pere(inst7);
Op_Conv_Amont_PolyMAC_V2_Face inst8;verifie_pere(inst8);
Op_Conv_Centre_PolyMAC_V2_Face inst9;verifie_pere(inst9);
Op_Diff_PolyMAC_Elem inst10;verifie_pere(inst10);
Op_Diff_PolyMAC_Face inst11;verifie_pere(inst11);
Op_Diff_PolyMAC_V2_Elem inst12;verifie_pere(inst12);
Op_Dift_PolyMAC_V2_Elem inst13;verifie_pere(inst13);
Op_Diff_PolyMAC_V2_Face inst14;verifie_pere(inst14);
Op_Div_PolyMAC inst15;verifie_pere(inst15);
Op_Div_PolyMAC_V2 inst16;verifie_pere(inst16);
Op_Grad_PolyMAC_Face inst17;verifie_pere(inst17);
Op_Grad_PolyMAC_V2_Face inst18;verifie_pere(inst18);
}
