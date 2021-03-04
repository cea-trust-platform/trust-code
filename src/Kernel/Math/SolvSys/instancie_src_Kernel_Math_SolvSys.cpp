//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <ILU.h>
#include <Precond.h>
#include <PrecondA.h>
#include <PrecondSolv.h>
#include <Precond_local.h>
#include <SSOR.h>
#include <SolvElem.h>
#include <SolvElem_BiCGStab.h>
#include <SolvElem_Gmres.h>
#include <Solv_Cholesky.h>
#include <Solv_GCP.h>
#include <Solv_GCP_NS.h>
#include <Solv_Gen.h>
#include <Solv_Gmres.h>
#include <Solv_Optimal.h>
#include <Solv_Petsc.h>
#include <Solv_Petsc_AMGX.h>
#include <Solv_Petsc_GPU.h>
#include <SolveurSys.h>
void instancie_src_Kernel_Math_SolvSys() {
Cerr << "src_Kernel_Math_SolvSys" << finl;
ILU inst1;verifie_pere(inst1);
Precond inst2;verifie_pere(inst2);
PrecondA inst3;verifie_pere(inst3);
PrecondSolv inst4;verifie_pere(inst4);
Precond_local inst5;verifie_pere(inst5);
SSOR inst6;verifie_pere(inst6);
SolvElem inst7;verifie_pere(inst7);
SolvElem_BiCGStab inst8;verifie_pere(inst8);
SolvElem_Gmres inst9;verifie_pere(inst9);
Solv_Cholesky inst10;verifie_pere(inst10);
Solv_GCP inst11;verifie_pere(inst11);
Solv_GCP_NS inst12;verifie_pere(inst12);
Solv_Gen inst13;verifie_pere(inst13);
Solv_Gmres inst14;verifie_pere(inst14);
Solv_Optimal inst15;verifie_pere(inst15);
Test_solveur inst16;verifie_pere(inst16);
Solv_Petsc inst17;verifie_pere(inst17);
Solv_Petsc_AMGX inst18;verifie_pere(inst18);
Solv_Petsc_GPU inst19;verifie_pere(inst19);
SolveurSys inst20;verifie_pere(inst20);
}
