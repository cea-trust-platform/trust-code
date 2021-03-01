//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Diff_VDF_Elem.h>
#include <Op_Diff_VDF_Multi_inco_Elem.h>
#include <Op_Diff_VDF_Multi_inco_var_Elem.h>
#include <Op_Diff_VDF_var_Elem.h>
void instancie_src_VDF_Operateurs_New_op() {
Cerr << "src_VDF_Operateurs_New_op" << finl;
Op_Diff_VDF_Elem inst1;verifie_pere(inst1);
Op_Diff_VDF_Multi_inco_Elem inst2;verifie_pere(inst2);
Op_Diff_VDF_Multi_inco_var_Elem inst3;verifie_pere(inst3);
Op_Diff_VDF_var_Elem inst4;verifie_pere(inst4);
}
