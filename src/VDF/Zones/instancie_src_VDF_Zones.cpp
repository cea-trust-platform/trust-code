//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <VDF_discretisation.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>
void instancie_src_VDF_Zones() {
Cerr << "src_VDF_Zones" << finl;
VDF_discretisation inst1;verifie_pere(inst1);
Zone_Cl_VDF inst2;verifie_pere(inst2);
Zone_VDF inst3;verifie_pere(inst3);
}
