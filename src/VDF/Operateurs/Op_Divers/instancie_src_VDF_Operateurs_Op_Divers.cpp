//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Op_Div_VDF_Elem.h>
#include <Op_Grad_P0_to_Face.h>
#include <Op_Grad_VDF_Face.h>
void instancie_src_VDF_Operateurs_Op_Divers() {
Cerr << "src_VDF_Operateurs_Op_Divers" << finl;
Op_Div_VDF_Elem inst1;verifie_pere(inst1);
Op_Grad_P0_to_Face inst2;verifie_pere(inst2);
Op_Grad_VDF_Face inst3;verifie_pere(inst3);
}
