#!/bin/bash
AMGX_DIR=$TRUST_ROOT/lib/src/LIBAMGX/AmgX
cd $AMGX_DIR/lib/examples
export LD_LIBRARY_PATH=$AMGX_DIR/lib:$LD_LIBRARY_PATH
# Test valgrind:
#[ "$TRUST_WITHOUT_HOST" = 1 ] && valgrind ./amgx_capi -m matrix.mtx -c ../configs/core/FGMRES_AGGREGATION.json 2>&1 | tee -a $log
exec=./amgx_capi     trust -gpu dummy   -m matrix.mtx -c ../configs/core/FGMRES_AGGREGATION.json 2>&1 | tee -a $log
exec=./amgx_mpi_capi trust -gpu dummy 2 -m matrix.mtx -c ../configs/core/FGMRES_AGGREGATION.json 2>&1 | tee -a $log

# solve: 	amgx_capi  	amgx_mpi_capi
# P600 		0.1120s		bizarre
# P2000      	0.0010s		0.0256s
# V100 		0.0011s		0.0153s
# Site AmgX	0.0006s		0.0156s

