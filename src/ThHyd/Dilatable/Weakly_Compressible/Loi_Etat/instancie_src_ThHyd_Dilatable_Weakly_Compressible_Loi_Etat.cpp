//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Loi_Etat_Binaire_GP_WC.h>
#include <Loi_Etat_GP_WC.h>
#include <Loi_Etat_Multi_GP_WC.h>
void instancie_src_ThHyd_Dilatable_Weakly_Compressible_Loi_Etat() {
Cerr << "src_ThHyd_Dilatable_Weakly_Compressible_Loi_Etat" << finl;
Loi_Etat_Binaire_GP_WC inst1;verifie_pere(inst1);
Loi_Etat_GP_WC inst2;verifie_pere(inst2);
Loi_Etat_Multi_GP_WC inst3;verifie_pere(inst3);
}
