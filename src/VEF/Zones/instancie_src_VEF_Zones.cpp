//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Elem_VEF.h>
#include <Hexa_VEF.h>
#include <Quadri_VEF.h>
#include <Tetra_VEF.h>
#include <Tri_VEF.h>
#include <VEF_1D.h>
#include <VEF_discretisation.h>
#include <VerifierCoin.h>
#include <Zone_Cl_VEF.h>
#include <Zone_VEF.h>
void instancie_src_VEF_Zones() {
Cerr << "src_VEF_Zones" << finl;
Elem_VEF inst1;verifie_pere(inst1);
Hexa_VEF inst2;verifie_pere(inst2);
Quadri_VEF inst3;verifie_pere(inst3);
Tetra_VEF inst4;verifie_pere(inst4);
Tri_VEF inst5;verifie_pere(inst5);
VEF_1D inst6;verifie_pere(inst6);
VEF_discretisation inst7;verifie_pere(inst7);
VerifierCoin inst8;verifie_pere(inst8);
Zone_Cl_VEF inst9;verifie_pere(inst9);
Zone_VEF inst10;verifie_pere(inst10);
}
