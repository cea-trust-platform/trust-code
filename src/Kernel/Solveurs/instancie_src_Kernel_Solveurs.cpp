//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Parametre_diffusion_implicite.h>
#include <Parametre_implicite.h>
#include <Solveur_Lineaire_Std.h>
void instancie_src_Kernel_Solveurs() {
Cerr << "src_Kernel_Solveurs" << finl;
Parametre_diffusion_implicite inst1;verifie_pere(inst1);
Parametre_implicite inst2;verifie_pere(inst2);
Solveur_Lineaire_Std inst3;verifie_pere(inst3);
}
