//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Conv_EF_Stab_CoviMAC_Elem.h>
#include <Op_Conv_EF_Stab_CoviMAC_Face.h>
#include <Op_Diff_CoviMAC_Elem.h>
#include <Op_Diff_CoviMAC_Face.h>
#include <Op_Div_CoviMAC.h>
#include <Op_Grad_CoviMAC_Face.h>
void instancie_src_CoviMAC_Operateurs() {
Cerr << "src_CoviMAC_Operateurs" << finl;
Op_Conv_EF_Stab_CoviMAC_Elem inst1;verifie_pere(inst1);
Op_Conv_Amont_CoviMAC_Elem inst2;verifie_pere(inst2);
Op_Conv_Centre_CoviMAC_Elem inst3;verifie_pere(inst3);
Op_Conv_EF_Stab_CoviMAC_Face inst4;verifie_pere(inst4);
Op_Conv_Amont_CoviMAC_Face inst5;verifie_pere(inst5);
Op_Conv_Centre_CoviMAC_Face inst6;verifie_pere(inst6);
Op_Diff_CoviMAC_Elem inst7;verifie_pere(inst7);
Op_Dift_CoviMAC_Elem inst8;verifie_pere(inst8);
Op_Diff_CoviMAC_Face inst9;verifie_pere(inst9);
Op_Div_CoviMAC inst10;verifie_pere(inst10);
Op_Grad_CoviMAC_Face inst11;verifie_pere(inst11);
}
