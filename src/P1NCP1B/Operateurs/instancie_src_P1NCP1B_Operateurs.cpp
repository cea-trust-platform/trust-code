//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Op_Curl_VEFP1B.h>
#include <Op_Diff_P1NC_barprim.h>
#include <Op_Diff_RotRot.h>
#include <Op_Diff_VEFP1NCP1B_Face.h>
#include <Op_Diff_VEFP1NCP1B_Face_Var.h>
#include <Op_Dift_VEF_P1NCP1B_Face.h>
#include <Op_Div_VEFP1B_Elem.h>
#include <Op_Grad_VEF_P1B_Face.h>
#include <Op_Rot_VEFP1B.h>
void instancie_src_P1NCP1B_Operateurs() {
Cerr << "src_P1NCP1B_Operateurs" << finl;
Op_Curl_VEFP1B inst1;verifie_pere(inst1);
Op_Diff_P1NC_barprim inst2;verifie_pere(inst2);
Op_Diff_RotRot inst3;verifie_pere(inst3);
Op_Diff_VEFP1NCP1B_Face inst4;verifie_pere(inst4);
Op_Diff_VEFP1NCP1B_Face_Var inst5;verifie_pere(inst5);
Op_Dift_VEF_P1NCP1B_Face inst6;verifie_pere(inst6);
Op_Div_VEFP1B_Elem inst7;verifie_pere(inst7);
Op_Grad_VEF_P1B_Face inst8;verifie_pere(inst8);
Op_Rot_VEFP1B inst9;verifie_pere(inst9);
}
