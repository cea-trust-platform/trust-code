#!/bin/bash
# Cas test poisson d'Amgx
echo "Usage: $0 [file.amgx]"
# Ici on fait tourner sur 8*6*10 cores (480) et 480*40*40*40=30e6 mailles (sans pb sur irene-amd)
ROOT=$TRUST_ROOT/lib/src/LIBAMGX/AmgX
export exec=$ROOT/lib/examples/amgx_mpi_poisson7
# Fichier de config:
echo "config_version=2
solver(s)=GMRES
s:convergence=RELATIVE_INI_CORE
s:tolerance=1.000000e-08
s:preconditioner(p)=GS
s:store_res_history=1
s:monitor_residual=1
s:print_solve_stats=1
s:obtain_timings=1
s:max_iters=10000
" > config
NP=2 && DEC="2 1 1"
#NP=8 && DEC="2 2 2"
#NP=512 && DEC="8 8 8"
# 40x40x40=64000 cells/core
mesh="40 40 40"
echo "Running poisson7 on $NP*($mesh) cells with $NP cores:" 
trust dummy $NP -mode dDDI -p $mesh $DEC -c config
