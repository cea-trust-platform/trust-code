//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Conv_Amont_VDF_Elem.h>
#include <Op_Conv_Amont_VDF_Face.h>
#include <Op_Conv_Centre_VDF_Face.h>
#include <Op_Conv_Quick_VDF_Elem.h>
#include <Op_Conv_Quick_VDF_Face.h>
#include <Op_Conv_centre4_VDF_Elem.h>
#include <Op_Conv_centre4_VDF_Face.h>
#include <Op_Conv_centre_VDF_Elem.h>
#include <Op_Diff_VDF_Face.h>
#include <Op_Diff_VDF_var_Face.h>
#include <Op_Dift_VDF_Elem.h>
#include <Op_Dift_VDF_Face.h>
#include <Op_Dift_VDF_Multi_inco_Elem.h>
#include <Op_Dift_VDF_Multi_inco_var_Elem.h>
#include <Op_Dift_VDF_var_Elem.h>
#include <Op_Dift_VDF_var_Face.h>
#include <Op_Div_VDF_Elem.h>
#include <Op_Grad_P0_to_Face.h>
#include <Op_Grad_VDF_Face.h>
void instancie_src_VDF_Operateurs() {
Cerr << "src_VDF_Operateurs" << finl;
Op_Conv_Amont_VDF_Elem inst1;verifie_pere(inst1);
Op_Conv_Amont_VDF_Face inst2;verifie_pere(inst2);
Op_Conv_Centre_VDF_Face inst3;verifie_pere(inst3);
Op_Conv_Quick_VDF_Elem inst4;verifie_pere(inst4);
Op_Conv_Quick_VDF_Face inst5;verifie_pere(inst5);
Op_Conv_centre4_VDF_Elem inst6;verifie_pere(inst6);
Op_Conv_centre4_VDF_Face inst7;verifie_pere(inst7);
Op_Conv_centre_VDF_Elem inst8;verifie_pere(inst8);
Op_Diff_VDF_Face inst9;verifie_pere(inst9);
Op_Diff_VDF_var_Face inst10;verifie_pere(inst10);
Op_Dift_VDF_Elem inst11;verifie_pere(inst11);
Op_Dift_VDF_Face inst12;verifie_pere(inst12);
Op_Dift_VDF_Multi_inco_Elem inst13;verifie_pere(inst13);
Op_Dift_VDF_Multi_inco_var_Elem inst14;verifie_pere(inst14);
Op_Dift_VDF_var_Elem inst15;verifie_pere(inst15);
Op_Dift_VDF_var_Face inst16;verifie_pere(inst16);
Op_Div_VDF_Elem inst17;verifie_pere(inst17);
Op_Grad_P0_to_Face inst18;verifie_pere(inst18);
Op_Grad_VDF_Face inst19;verifie_pere(inst19);
}
