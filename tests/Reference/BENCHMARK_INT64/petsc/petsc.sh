#!/bin/bash
# Check a big test case from PETSc
export PETSC_DIR=$PETSC_ROOT/linux_opt
cp $PETSC_DIR/share/petsc/examples/src/ksp/ksp/examples/tutorials/ex23.c $PETSC_DIR/share/petsc/examples/src/ksp/ksp/examples/tutorials/makefile .
make ex23
options="-n 10 -mat_view" 	# nnz = 20
options="-n 1000"		# nnz = 2000
mpirun -np 4 ./ex23 $options -ksp_type cg -pc_type sor -pc_sor_omega 1.6 -ksp_view -mat_type sbaij -ksp_rtol 1.e-4 -help

