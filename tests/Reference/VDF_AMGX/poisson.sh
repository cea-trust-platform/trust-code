#!/bin/bash
# Teste la validate du fichier .amgx avec le cas poisson de AmgXWrapper
echo "Usage: $0 [file.amgx]"
ROOT=$TRUST_ROOT/lib/src/LIBAMGX/AmgXWrapper
export exec=$ROOT/example/poisson/bin/poisson
configs=$ROOT/example/poisson/configs
timings()
{
  awk '/setup/ {setup=$0} /Iterations / {its=$0} /Memory Usage/ {mem=$0} /averaged/ {print $0" "setup" "its" "mem}' $1
}
if [ "$1" = "" ]
then
   # Cas AmgXWrapper:
   NP=40 && N=150 # Out of memory a N=300 sur V100 16GB
   [ `hostname` = is242981.intra.cea.fr ] && NP=4 && N=85 # Sinon out of memory
   # Test CPU avec PETSc
   echo -e "$NP CPU         ($N*$N*$N, Petsc C-AMG): \c"
   [ ! -f PETSc_SolverOptions_GAMG.info ] && cp $configs/PETSc_SolverOptions_GAMG.info .
   log=PETSc_GAMG_$NP"CPU_"$N"x"$N"x"$N.log
   trust dummy $NP -caseName PETSc_GAMG_$N"x"$N"x"$N -mode PETSc  -cfgFileName ./PETSc_SolverOptions_GAMG.info     -Nx $N -Ny $N -Nz $N 1>$log 2>&1
   timings $log
   # Test GPU avec AMGX
   export CUDA_VISIBLE_DEVICES=0
   echo CUDA_VISIBLE_DEVICES=$CUDA_VISIBLE_DEVICES
   for NGPU in 1
   do
      echo -e "$NP CPU + $NGPU GPU ($N*$N*$N,  AmgX C-AMG): \c"
      [ ! -f AmgX_SolverOptions_Classical.info ] && cp $configs/AmgX_SolverOptions_Classical.info .
      log=AmgX_$NGPU"GPU_"$NP"CPU_"$N"x"$N"x"$N.log
      trust -gpu dummy $NP -caseName AmgX_AGG_$N"x"$N"x"$N -mode AmgX_GPU -cfgFileName ./AmgX_SolverOptions_Classical.info -Nx $N -Ny $N -Nz $N 1>$log 2>&1
      timings $log
   done
else
   valgrind="" && [ "$VALGRIND" != "" ] && VALGRIND=1
   N=10
   trust -gpu dummy -caseName AmgX_$N"x"$N"x"$N -mode AmgX_GPU -cfgFileName $1 -Nx $N -Ny $N -Nz $N
fi

# Comment sont geres les communications CPU<->GPU ?
# See [1] https://github.com/barbagroup/AmgXWrapper
# See [2] https://github.com/barbagroup/AmgXWrapper/tree/master/example/poisson
# AmgX selon [1] fait communiquer tous les MPIs avec les GPUs disponibles ce qui ne serait pas optimal (verifier?)
# AmgXWrapper selon [1] va s'arranger pour faire communiquer 1 MPI avec 1 seul GPU et ce pour chaque GPU disponible
# Exemple:
# Node 1: 12 MPI <-> 2 GPU alors ranks 0, 6 <-> GPU[0-1]
# Node 2:  6 MPI <-> 4 GPU alors ranks 12, 14, 16, 17 <-> GPU[0-3]
# La reservation des GPUs se fait avec SLURM mais il est possible de limiter le nombre de GPUs utilises avec export CUDA_VISIBLE_DEVICES=0,1 (ici GPU 0 et 1)
# Ainsi: CUDA_VISIBLE_DEVICES=0   mpiexec -np 10 $exec fera communiquer 1 CPU avec 1 GPU
# Ainsi: CUDA_VISIBLE_DEVICES=0,1 mpiexec -np 10 $exec fera communiquer 2 CPU avec 2 GPU
# Rappel:
# PC fixe      :   1 noeud  de  4 CPU & 1 GPU (P400)
# orcus gpu01  :   2 noeuds de 24 CPU & 2 GPU (v100)
# orcus gpu02  :   2 noeuds de 32 CPU & 2 GPU (v100)
# irene        :  32 noeuds de 40 CPU & 4 GPU (v100)
# jeanzay gpu_1: 261 noeuds de 40 CPU & 4 GPU (v100)
# jeanzay gpu_3: 351 noeuds de 40 CPU & 4 GPU (v100)
