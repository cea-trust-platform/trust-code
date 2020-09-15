#!/bin/bash
# Teste la validate du fichier .amgx avec le cas poisson de AmgXWrapper
echo "Usage: $0 [file.amgx]"
ROOT=$TRUST_ROOT/lib/src/LIBAMGX/AmgXWrapper
export exec=$ROOT/example/poisson/bin/poisson
configs=$ROOT/example/poisson/configs
if [ "$1" = "" ]
then
   # Cas AmgXWrapper:
   N=100 && [ `hostname` = is242981.intra.cea.fr ] && N=85 # Sinon out of memory
   NP=4
   echo -e "$NP CPU:\c"
   trust dummy $NP -caseName PETSc_GAMG_$N"x"$N"x"$N -mode PETSc  -cfgFileName $configs/PETSc_SolverOptions_GAMG.info     -Nx $N -Ny $N -Nz $N 2>&1 | tee PETSc_GAMG.log | grep averaged
   echo -e "$NP CPU + 1 GPU:\c"
   trust -gpu dummy $NP -caseName AmgX_AGG_$N"x"$N"x"$N -mode AmgX_GPU -cfgFileName $configs/AmgX_SolverOptions_Classical.info -Nx $N -Ny $N -Nz $N 2>&1 | tee AmgX_GPU.log | grep averaged
else
   valgrind="" && [ "$VALGRIND" != "" ] && VALGRIND=1
   N=10
   trust -gpu dummy -caseName AmgX_$N"x"$N"x"$N -mode AmgX_GPU -cfgFileName $1 -Nx $N -Ny $N -Nz $N
fi
