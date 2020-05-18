//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Assembleur_P_CoviMAC.h>
#include <Masse_CoviMAC_Elem.h>
#include <Masse_CoviMAC_Face.h>
#include <Solveur_UP.h>
void instancie_src_CoviMAC_Solveurs() {
Cerr << "src_CoviMAC_Solveurs" << finl;
Assembleur_P_CoviMAC inst1;verifie_pere(inst1);
Masse_CoviMAC_Elem inst2;verifie_pere(inst2);
Masse_CoviMAC_Face inst3;verifie_pere(inst3);
Solveur_UP inst4;verifie_pere(inst4);
}
