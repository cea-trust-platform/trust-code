#!/bin/bash
rm -f kernels 
# -gpu=loadcache:L1|L2s
# -gpu=ccnative
# Aucun effet (meme O3 !) sur le device...
KOKKOS_INC="-I$TRUST_KOKKOS_ROOT/linux_opt/include"
KOKKOS_LIB="-L$TRUST_KOKKOS_ROOT/linux_opt/lib64 -lkokkoscontainers -lkokkoscore"
if [ "$ROCM_PATH" != ""  ]
then
   # Build HIP code with hipcc:
   HIP="hipcc --offload-arch=$ROCM_ARCH" # -march=$ROCM_ARCH"
   cmd="$HIP -g -O3 -std=c++17 $KOKKOS_INC -o kernels kernels.cpp $KOKKOS_LIB -ldl"
   echo $cmd
   time eval $cmd || exit -1
else
   # Use nvc++ -cuda
   COMPILER="nvc++ -cuda -gpu=nordc -L$CUDA_ROOT/lib64/stubs -lcuda"
   cmd="$COMPILER -g -O3 -std=c++17 $KOKKOS_INC -o kernels kernels.cpp $KOKKOS_LIB"
   echo $cmd
   time eval $cmd || exit -1
   #COMPILER="nvcc -ccbin g++ -L$CUDA_ROOT/lib64/stubs -lcuda"
   #cmd="$COMPILER -g -O3 -std=c++17 $KOKKOS_INC -o kernels kernels.cpp $KOKKOS_LIB"
   #echo $cmd
   #time eval $cmd || exit -1
fi
echo "Build OK"
rm -f *nsys-rep
touch dumb.data && exec=`pwd`/kernels trust dumb 1
[ "$1" = -nsys ] && nsys profile ./kernels && nsys-ui report1.nsys-rep

exit

# Some card data from slowest to fastest:

# GFX1032 (hipcc 18.0)
[Kernel OpDiv] CPU    Mean time: 200.758 ms 0.1 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 141.784 ms 0.1 GFLOPS # Regression vs OMPT 1.2 GFLOPS ?

# A3000 (nvc++)
[Kernel OpDiv] CPU    Mean time: 124.464 ms 0.1 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 10.8684 ms 1.9 GFLOPS

# GFX90a (hipcc 18.0)
[Kernel OpDiv] CPU    Mean time: 78.5479 ms 0.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 5.97553 ms 3.4 GFLOPS

# A6000 (nvc++)
[Kernel OpDiv] CPU    Mean time: 75.6418 ms 0.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 4.48062 ms 4.6 GFLOPS

# GFX1100 (hipcc 18.0)
[Kernel OpDiv] CPU    Mean time: 90.1791 ms 0.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 1.52586 ms 13.6 GFLOPS

# A100 (nvc++)
[Kernel OpDiv] CPU    Mean time: 144.448 ms 0.1 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 1.40876 ms 14.7 GFLOPS

