# Try to mix OpenMP and Kokkos
# Cela fonctionne avec: 
# 1) Kokkos 4.x (C++17) 
# 2) -cuda pour compiler du Cuda avec nvc++ !

#export NVHPC_CUDA_HOME=$CUDA_ROOT # Variable qui permet de changer le CUDA utilise par le compilateur nvc++

KOKKOS="-I$TRUST_KOKKOS_ROOT/linux/include -L$TRUST_KOKKOS_ROOT/linux/lib64 -lkokkoscontainers -lkokkoscore" # -expt-extended-lambda -Wext-lambda-captures-this 
if [ "$ROCM_PATH" != "" ]
then
   OPENMP="-fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=$ROCM_ARCH -Wno-openmp-mapping -ldl"
else
   OPENMP="-fopenmp -mp -target=gpu -cuda"
fi
echo $OPENMP
$TRUST_CC -std=c++17 $OPENMP -o mix_openmp_kokkos mix_openmp_kokkos.cpp $KOKKOS && touch dumb.data && exec=`pwd`/mix_openmp_kokkos trust dumb 1
