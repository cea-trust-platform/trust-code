//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Turbulence_paroi.h>
#include <Turbulence_paroi_scal.h>
void instancie_src_ThHyd_Modele_turbulence_base() {
Cerr << "src_ThHyd_Modele_turbulence_base" << finl;
Turbulence_paroi inst1;verifie_pere(inst1);
Turbulence_paroi_scal inst2;verifie_pere(inst2);
}
