//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Aire_interfaciale_Yao_Morel.h>
#include <Energie_Multiphase.h>
#include <Masse_Multiphase.h>
#include <QDM_Multiphase.h>
void instancie_src_ThHyd_Multiphase_Equations() {
Cerr << "src_ThHyd_Multiphase_Equations" << finl;
Aire_interfaciale_Yao_Morel inst1;verifie_pere(inst1);
Energie_Multiphase inst2;verifie_pere(inst2);
Masse_Multiphase inst3;verifie_pere(inst3);
QDM_Multiphase inst4;verifie_pere(inst4);
}
