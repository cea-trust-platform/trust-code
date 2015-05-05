//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Conv_negligeable.h>
#include <Op_Diff_negligeable.h>
#include <Operateur_Conv.h>
#include <Operateur_Diff.h>
#include <Operateur_Div.h>
#include <Operateur_Grad.h>
void instancie_src_Kernel_Operateurs() {
Cerr << "src_Kernel_Operateurs" << finl;
Op_Conv_negligeable inst1;verifie_pere(inst1);
Op_Diff_negligeable inst2;verifie_pere(inst2);
Op_Dift_negligeable inst3;verifie_pere(inst3);
Operateur_Conv inst4;verifie_pere(inst4);
Operateur_Diff inst5;verifie_pere(inst5);
Operateur_Div inst6;verifie_pere(inst6);
Operateur_Grad inst7;verifie_pere(inst7);
}
