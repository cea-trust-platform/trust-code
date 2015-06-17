//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Assembleur_P_EF.h>
#include <Op_Conv_EF.h>
#include <Op_Conv_SUPG_EF.h>
#include <Op_Diff_EF.h>
#include <Op_Div_EF.h>
#include <Op_Div_EF_fois_rho.h>
#include <Op_Grad_EF.h>
void instancie_src_EF_Operateurs() {
Cerr << "src_EF_Operateurs" << finl;
Assembleur_P_EF inst1;verifie_pere(inst1);
Op_Conv_EF inst2;verifie_pere(inst2);
Op_Conv_BTD_EF inst3;verifie_pere(inst3);
Op_Conv_SUPG_EF inst4;verifie_pere(inst4);
Op_Diff_EF inst5;verifie_pere(inst5);
Op_Diff_option_EF inst6;verifie_pere(inst6);
Op_Div_EF inst7;verifie_pere(inst7);
Op_Div_EF_fois_rho inst8;verifie_pere(inst8);
Op_Grad_EF inst9;verifie_pere(inst9);
}
