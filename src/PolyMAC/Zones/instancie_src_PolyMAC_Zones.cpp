//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <PolyMAC_P0_discretisation.h>
#include <PolyMAC_discretisation.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Zone_PolyMAC_P0.h>
void instancie_src_PolyMAC_Zones() {
Cerr << "src_PolyMAC_Zones" << finl;
PolyMAC_P0_discretisation inst1;verifie_pere(inst1);
PolyMAC_discretisation inst2;verifie_pere(inst2);
Zone_Cl_PolyMAC inst3;verifie_pere(inst3);
Zone_PolyMAC inst4;verifie_pere(inst4);
Zone_PolyMAC_P0 inst5;verifie_pere(inst5);
}
