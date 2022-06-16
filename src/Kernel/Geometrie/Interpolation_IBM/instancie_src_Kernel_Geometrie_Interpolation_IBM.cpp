//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Interpolation_IBM.h>
#include <Interpolation_IBM_aucune.h>
#include <Interpolation_IBM_elem_fluid.h>
#include <Interpolation_IBM_hybrid.h>
#include <Interpolation_IBM_mean_gradient.h>
#include <Interpolation_IBM_power_law_tbl.h>
void instancie_src_Kernel_Geometrie_Interpolation_IBM() {
Cerr << "src_Kernel_Geometrie_Interpolation_IBM" << finl;
Interpolation_IBM inst1;verifie_pere(inst1);
Interpolation_IBM_aucune inst2;verifie_pere(inst2);
Interpolation_IBM_elem_fluid inst3;verifie_pere(inst3);
Interpolation_IBM_hybrid inst4;verifie_pere(inst4);
Interpolation_IBM_mean_gradient inst5;verifie_pere(inst5);
Interpolation_IBM_power_law_tbl inst6;verifie_pere(inst6);
}
