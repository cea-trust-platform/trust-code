#!/bin/bash
# Cas test poisson d'Amgx
echo "Usage: $0 [file.amgx]"
# Ici on fait tourner sur 8*6*10 cores (480) et 480*40*40*40=30e6 mailles (sans pb sur irene-amd)
ROOT=$TRUST_ROOT/lib/src/LIBAMGX/AmgX
export exec=$ROOT/lib/examples/amgx_mpi_poisson7
# Fichier de config:
echo "config_version=2
solver(pcgf)=PCG
determinism_flag=1
pcgf:preconditioner(prec)=AMG
pcgf:use_scalar_norm=1
pcgf:max_iters=10000
pcgf:convergence=RELATIVE_INI_CORE
pcgf:tolerance=1e-7
pcgf:norm=L2
pcgf:print_solve_stats=1
pcgf:monitor_residual=1
pcgf:obtain_timings=1

prec:error_scaling=0
prec:print_grid_stats=1
prec:max_iters=1
prec:cycle=V
prec:min_coarse_rows=2
prec:max_levels=100

prec:smoother(smoother)=BLOCK_JACOBI
prec:presweeps=1
prec:postsweeps=1
prec:coarsest_sweeps=1

prec:coarse_solver(c_solver)=DENSE_LU_SOLVER
prec:dense_lu_num_rows=2

prec:algorithm=CLASSICAL
#prec:selector=HMIS
# Much faster for setup:
prec:selector=PMIS
prec:interpolator=D2
prec:strength=AHAT


smoother:relaxation_factor=0.8
" > config
NP=1 && DEC="1 1 1"
#NP=2 && DEC="2 1 1"
#NP=8 && DEC="2 2 2"
#NP=512 && DEC="8 8 8"
# 40x40x40=64000 cells/core
mesh="40 40 40"
echo "==========================================================="
echo "Running poisson7 in DP on $NP*($mesh) cells with $NP cores:" 
echo "==========================================================="
trust dummy $NP -mode dDDI -p $mesh $DEC -c config
echo "==========================================================="
echo "Running poisson7 in SP on $NP*($mesh) cells with $NP cores:" 
echo "==========================================================="
trust dummy $NP -mode dFFI -p $mesh $DEC -c config
