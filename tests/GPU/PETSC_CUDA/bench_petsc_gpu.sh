#!/bin/bash
(cd $PETSC_DIR/share/petsc/examples/src/snes/examples/tutorials && make ex19)
ln -s -f $PETSC_DIR/share/petsc/examples/src/snes/examples/tutorials/ex19.c .
ln -s -f $PETSC_DIR/share/petsc/examples/src/snes/examples/tutorials/ex19 .
exec="./ex19"
size=7 #&& [ "$TRUST_WITHOUT_HOST" = 0 ] && size=10 # size=10 38e6 DOF
PETSC_OPTIONS="-snes_monitor -pc_type mg -da_refine $size -snes_view -pc_mg_levels 9 -mg_levels_ksp_type chebyshev -mg_levels_pc_type jacobi"
touch dummy.data
#######################
# Test sur un noeud GPU
#######################
# CPU sans GPU
PETSC_OPTIONS=$PETSC_OPTIONS" -log_view :CPUx1" 	trust -gpu dummy
PETSC_OPTIONS=$PETSC_OPTIONS" -log_view :CPUx2" 	trust -gpu dummy 2
# CPU + 1 GPU
PETSC_OPTIONS=$PETSC_OPTIONS" -cuda_view -dm_mat_type aijcusparse -dm_vec_type cuda"
PETSC_OPTIONS=$PETSC_OPTIONS" -log_view :CPUx1_GPUx1"	trust -gpu dummy
PETSC_OPTIONS=$PETSC_OPTIONS" -log_view :CPUx2_GPUx1" 	trust -gpu dummy 2

# See https://www.mcs.anl.gov/petsc/meetings/2019/slides/mills-petsc-2019.pdf page 23:
# command="./ex19 -cuda_view -snes_monitor -pc_type mg -da_refine 10 -snes_view -pc_mg_levels 9 -mg_levels_ksp_type chebyshev -mg_levels_pc_type jacobi -log_view"
# See https://www.ibm.com/support/knowledgecenter/SSWRJV_10.1.0/jsm/jsrun.html
# -n ressources		Dans une ressource, plusieurs cores et gpus
# -c core/ressource	--cpu_per_rs 
# -a task/ressource 	--tasks_per_rs
# -g gpu/ressource	--gpu_per_rs 
# CPU 42 MPI avec 6*(7 cores qui ne communiquent pas avec 1 GPU)
# jsrun -n 6 -a 7 -c 7 -g 1 $command
# GPU 24 MPI avec 6*(4 cores communiquent avec 1 GPU)
# jsrun -n 6 -a 4 -c 4 -g 1 $command -dm_mat_type aijcusparse -dm_vec_type cuda 
