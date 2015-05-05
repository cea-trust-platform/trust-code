//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Mod_turb_hyd.h>
#include <Modele_turbulence_scal.h>
#include <Turbulence_paroi.h>
#include <Turbulence_paroi_scal.h>
void instancie_src_ThHyd_Modele_turbulence_base() {
Cerr << "src_ThHyd_Modele_turbulence_base" << finl;
Mod_turb_hyd inst1;verifie_pere(inst1);
Modele_turbulence_scal inst2;verifie_pere(inst2);
Turbulence_paroi inst3;verifie_pere(inst3);
Turbulence_paroi_scal inst4;verifie_pere(inst4);
}
