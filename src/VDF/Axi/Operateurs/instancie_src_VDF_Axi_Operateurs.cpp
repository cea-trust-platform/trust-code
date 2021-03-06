//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Conv_Quick_VDF_Face_Axi.h>
#include <Op_Diff_VDF_Face_Axi.h>
#include <Op_Diff_VDF_var_Face_Axi.h>
#include <Op_Dift_VDF_Face_Axi.h>
#include <Op_Dift_VDF_var_Face_Axi.h>
void instancie_src_VDF_Axi_Operateurs() {
Cerr << "src_VDF_Axi_Operateurs" << finl;
Op_Conv_Quick_VDF_Face_Axi inst1;verifie_pere(inst1);
Op_Diff_VDF_Face_Axi inst2;verifie_pere(inst2);
Op_Diff_VDF_var_Face_Axi inst3;verifie_pere(inst3);
Op_Dift_VDF_Face_Axi inst4;verifie_pere(inst4);
Op_Dift_VDF_var_Face_Axi inst5;verifie_pere(inst5);
}
