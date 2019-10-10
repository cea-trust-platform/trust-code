//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Loi_Paroi_Nu_Impose_PolyMAC.h>
#include <Paroi_negligeable_PolyMAC.h>
#include <Source_Transport_K_Eps_PolyMAC_Elem.h>
#include <Turbulence_hyd_sous_maille_Wale_PolyMAC.h>
void instancie_src_PolyMAC_Turbulence() {
Cerr << "src_PolyMAC_Turbulence" << finl;
Loi_Paroi_Nu_Impose_PolyMAC inst1;verifie_pere(inst1);
Paroi_negligeable_PolyMAC inst2;verifie_pere(inst2);
Source_Transport_K_Eps_PolyMAC_Elem inst3;verifie_pere(inst3);
Source_Transport_K_Eps_anisotherme_PolyMAC_Elem inst4;verifie_pere(inst4);
Source_Transport_K_Eps_concen_PolyMAC_Elem inst5;verifie_pere(inst5);
Source_Transport_K_Eps_aniso_therm_concen_PolyMAC_Elem inst6;verifie_pere(inst6);
Turbulence_hyd_sous_maille_Wale_PolyMAC inst7;verifie_pere(inst7);
}
