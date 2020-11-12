//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Matrice.h>
#include <Matrice_Bloc.h>
#include <Matrice_Bloc_Sym.h>
#include <Matrice_Dense.h>
#include <Matrice_Diagonale.h>
#include <Matrice_Morse.h>
#include <Matrice_Morse_Diag.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Nulle.h>
#include <Matrice_Petsc.h>
void instancie_src_Kernel_Math_Matrices() {
Cerr << "src_Kernel_Math_Matrices" << finl;
Matrice inst1;verifie_pere(inst1);
Matrice_Bloc inst2;verifie_pere(inst2);
Matrice_Bloc_Sym inst3;verifie_pere(inst3);
Matrice_Dense inst4;verifie_pere(inst4);
Matrice_Diagonale inst5;verifie_pere(inst5);
Matrice_Morse inst6;verifie_pere(inst6);
Matrice_Morse_Diag inst7;verifie_pere(inst7);
Matrice_Morse_Sym inst8;verifie_pere(inst8);
Matrice_Nulle inst9;verifie_pere(inst9);
Matrice_Petsc inst10;verifie_pere(inst10);
}
