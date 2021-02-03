#!/bin/bash
# Check a big test case from PETSc
export PETSC_DIR=$PETSC_ROOT/linux_opt
cp $PETSC_DIR/share/petsc/examples/src/ksp/ksp/tutorials/ex23.c $PETSC_DIR/share/petsc/examples/src/ksp/ksp/tutorials/makefile .
make ex23
options="-n 10 -mat_view" 
options="-n 1000"		
#options="-n 1400000000"		# 1.4e9 DOF
#mat="-mat_type sbaij" # Crash constate sur le cas a 1.4e9, en repassant a aij OK
# Equivalent a PETSc GCP { precond ssor { omega 1.6 } seuil 1.e-6 }
mpirun -np 4 ./ex23 $options $mat -ksp_type cg -pc_type sor -pc_sor_omega 1.6 -ksp_view -ksp_rtol 1.e-6 -log_view -help

