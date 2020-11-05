//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Energie_Multiphase.h>
#include <Masse_Multiphase.h>
#include <QDM_Multiphase.h>
void instancie_src_ThHyd_Multiphase_Equations() {
Cerr << "src_ThHyd_Multiphase_Equations" << finl;
Energie_Multiphase inst1;verifie_pere(inst1);
Masse_Multiphase inst2;verifie_pere(inst2);
QDM_Multiphase inst3;verifie_pere(inst3);
}
