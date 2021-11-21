//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Conv_Centre_VDF_Face.h>
#include <Op_Conv_centre4_VDF_Face.h>
#include <Op_Div_VDF_Elem.h>
#include <Op_Grad_P0_to_Face.h>
#include <Op_Grad_VDF_Face.h>
void instancie_src_VDF_Operateurs() {
Cerr << "src_VDF_Operateurs" << finl;
Op_Conv_Centre_VDF_Face inst1;verifie_pere(inst1);
Op_Conv_centre4_VDF_Face inst2;verifie_pere(inst2);
Op_Div_VDF_Elem inst3;verifie_pere(inst3);
Op_Grad_P0_to_Face inst4;verifie_pere(inst4);
Op_Grad_VDF_Face inst5;verifie_pere(inst5);
}
