//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Modele_turbulence_hyd_nul.h>
void instancie_src_ThHyd_Turbulence() {
Cerr << "src_ThHyd_Turbulence" << finl;
Modele_turbulence_hyd_nul inst1;verifie_pere(inst1);
}
