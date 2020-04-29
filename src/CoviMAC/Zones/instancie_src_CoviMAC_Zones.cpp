//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <CoviMAC_discretisation.h>
#include <Elem_CoviMAC.h>
#include <Hexa_CoviMAC.h>
#include <Polyedre_CoviMAC.h>
#include <Polygone_CoviMAC.h>
#include <Quadri_CoviMAC.h>
#include <Segment_CoviMAC.h>
#include <Tetra_CoviMAC.h>
#include <Tri_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
void instancie_src_CoviMAC_Zones() {
Cerr << "src_CoviMAC_Zones" << finl;
CoviMAC_discretisation inst1;verifie_pere(inst1);
Elem_CoviMAC inst2;verifie_pere(inst2);
Hexa_CoviMAC inst3;verifie_pere(inst3);
Polyedre_CoviMAC inst4;verifie_pere(inst4);
Polygone_CoviMAC inst5;verifie_pere(inst5);
Quadri_CoviMAC inst6;verifie_pere(inst6);
Segment_CoviMAC inst7;verifie_pere(inst7);
Tetra_CoviMAC inst8;verifie_pere(inst8);
Tri_CoviMAC inst9;verifie_pere(inst9);
Zone_Cl_CoviMAC inst10;verifie_pere(inst10);
Zone_CoviMAC inst11;verifie_pere(inst11);
}
