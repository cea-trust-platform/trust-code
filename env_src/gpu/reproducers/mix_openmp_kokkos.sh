# Try to mix OpenMP and Kokkos
# Cela fonctionne avec: 
# 1) Kokkos 4.x (C++17) 
# 2) -cuda pour compiler du Cuda avec nvc++ !

#export NVHPC_CUDA_HOME=$CUDA_ROOT # Variable qui permet de changer le CUDA utilise par le compilateur nvc++

KOKKOS="-I$TRUST_KOKKOS_ROOT/include -L$TRUST_KOKKOS_ROOT/lib64 -lkokkoscontainers -lkokkoscore" # -expt-extended-lambda -Wext-lambda-captures-this 
OPENMP="-mp=gpu"
$TRUST_CC -cuda -std=c++17 $OPENMP -o mix_openmp_kokkos mix_openmp_kokkos.cpp $KOKKOS && touch dumb.data && exec=`pwd`/mix_openmp_kokkos trust dumb 1
