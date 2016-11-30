//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Dirichlet_paroi_fixe_iso_Genepi2.h>
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
Dirichlet_paroi_fixe_iso_Genepi2 inst1;verifie_pere(inst1);
EF_discretisation inst2;verifie_pere(inst2);
Elem_EF inst3;verifie_pere(inst3);
Hexa_EF inst4;verifie_pere(inst4);
Quadri_EF inst5;verifie_pere(inst5);
Segment_EF inst6;verifie_pere(inst6);
Solveur_Masse_EF inst7;verifie_pere(inst7);
Tetra_EF inst8;verifie_pere(inst8);
Tri_EF inst9;verifie_pere(inst9);
Zone_Cl_EF inst10;verifie_pere(inst10);
Zone_EF inst11;verifie_pere(inst11);
}
