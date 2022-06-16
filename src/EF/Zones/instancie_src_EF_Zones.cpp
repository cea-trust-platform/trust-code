//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Dirichlet_paroi_fixe_iso_Genepi2.h>
#include <EF_axi_discretisation.h>
#include <EF_discretisation.h>
#include <Elem_EF.h>
#include <Hexa_EF.h>
#include <Point_EF.h>
#include <Quadri_EF.h>
#include <Segment_EF.h>
#include <Segment_EF_axi.h>
#include <Solveur_Masse_EF.h>
#include <Tetra_EF.h>
#include <Tri_EF.h>
#include <Zone_Cl_EF.h>
#include <Zone_EF.h>
#include <Zone_EF_axi.h>
void instancie_src_EF_Zones() {
Cerr << "src_EF_Zones" << finl;
Dirichlet_paroi_fixe_iso_Genepi2 inst1;verifie_pere(inst1);
EF_axi_discretisation inst2;verifie_pere(inst2);
EF_discretisation inst3;verifie_pere(inst3);
Elem_EF inst4;verifie_pere(inst4);
Hexa_EF inst5;verifie_pere(inst5);
Point_EF inst6;verifie_pere(inst6);
Quadri_EF inst7;verifie_pere(inst7);
Segment_EF inst8;verifie_pere(inst8);
Segment_EF_axi inst9;verifie_pere(inst9);
Solveur_Masse_EF inst10;verifie_pere(inst10);
Tetra_EF inst11;verifie_pere(inst11);
Tri_EF inst12;verifie_pere(inst12);
Zone_Cl_EF inst13;verifie_pere(inst13);
Zone_EF inst14;verifie_pere(inst14);
Zone_EF_axi inst15;verifie_pere(inst15);
}
