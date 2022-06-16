//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Modele_Permeabilite.h>
#include <PDF_model.h>
void instancie_src_ThHyd_Incompressible() {
Cerr << "src_ThHyd_Incompressible" << finl;
Modele_Permeabilite inst1;verifie_pere(inst1);
PDF_model inst2;verifie_pere(inst2);
}
