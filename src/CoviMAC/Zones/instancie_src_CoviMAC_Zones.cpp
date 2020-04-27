//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Assembleur_P_CoviMAC.h>
#include <Elem_CoviMAC.h>
#include <Hexa_CoviMAC.h>
#include <Masse_CoviMAC_Elem.h>
#include <Masse_CoviMAC_Face.h>
#include <CoviMAC_discretisation.h>
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
Assembleur_P_CoviMAC inst1;verifie_pere(inst1);
Elem_CoviMAC inst2;verifie_pere(inst2);
Hexa_CoviMAC inst3;verifie_pere(inst3);
Masse_CoviMAC_Elem inst4;verifie_pere(inst4);
Masse_CoviMAC_Face inst5;verifie_pere(inst5);
CoviMAC_discretisation inst6;verifie_pere(inst6);
Polyedre_CoviMAC inst7;verifie_pere(inst7);
Polygone_CoviMAC inst8;verifie_pere(inst8);
Quadri_CoviMAC inst9;verifie_pere(inst9);
Segment_CoviMAC inst10;verifie_pere(inst10);
Tetra_CoviMAC inst11;verifie_pere(inst11);
Tri_CoviMAC inst12;verifie_pere(inst12);
Zone_Cl_CoviMAC inst13;verifie_pere(inst13);
Zone_CoviMAC inst14;verifie_pere(inst14);
}
