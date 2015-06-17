//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <EF_discretisation.h>
#include <Elem_EF.h>
#include <Hexa_EF.h>
#include <Quadri_EF.h>
#include <Segment_EF.h>
#include <Solveur_Masse_EF.h>
#include <Tetra_EF.h>
#include <Tri_EF.h>
#include <Zone_Cl_EF.h>
#include <Zone_EF.h>
void instancie_src_EF_Zones() {
Cerr << "src_EF_Zones" << finl;
EF_discretisation inst1;verifie_pere(inst1);
Elem_EF inst2;verifie_pere(inst2);
Hexa_EF inst3;verifie_pere(inst3);
Quadri_EF inst4;verifie_pere(inst4);
Segment_EF inst5;verifie_pere(inst5);
Solveur_Masse_EF inst6;verifie_pere(inst6);
Tetra_EF inst7;verifie_pere(inst7);
Tri_EF inst8;verifie_pere(inst8);
Zone_Cl_EF inst9;verifie_pere(inst9);
Zone_EF inst10;verifie_pere(inst10);
}
