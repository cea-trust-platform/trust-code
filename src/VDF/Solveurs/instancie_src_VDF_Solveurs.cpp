//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Assembleur_P_VDF.h>
#include <Assembleur_P_VDF_Q4.h>
#include <Masse_VDF_Elem.h>
#include <Masse_VDF_Face.h>
void instancie_src_VDF_Solveurs() {
Cerr << "src_VDF_Solveurs" << finl;
Assembleur_P_VDF inst1;verifie_pere(inst1);
Assembleur_P_VDF_Q4 inst2;verifie_pere(inst2);
Masse_VDF_Elem inst3;verifie_pere(inst3);
Masse_VDF_Face inst4;verifie_pere(inst4);
}
