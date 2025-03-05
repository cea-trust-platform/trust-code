#!/bin/bash
run()
{
   echo "Build OK"
   rm -f *nsys-rep
   touch dumb.data && exec=`pwd`/kernels trust dumb 1
   [ "$1" = -nsys ] && nsys profile ./kernels && nsys-ui report1.nsys-rep
}
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
   run
else
   # Using nvcc -ccbin=g++ (build time 5.7s)
   COMPILER="nvcc -x cu -arch=sm_$TRUST_CUDA_CC --extended-lambda -ccbin=g++ -Xcompiler=\"-O3\" -L$CUDA_ROOT/lib64/stubs -lcuda"
   cmd="$COMPILER -g -O3 -std=c++17 $KOKKOS_INC -o kernels kernels.cpp $KOKKOS_LIB"
   echo $cmd
   time eval $cmd || exit -1
   run
   # Use nvc++ -cuda (build time 7.2s)
   COMPILER="nvc++ -cuda -gpu=nordc,cc$TRUST_CUDA_CC -L$CUDA_ROOT/lib64/stubs -lcuda"
   cmd="$COMPILER -g -O3 -std=c++17 $KOKKOS_INC -o kernels kernels.cpp $KOKKOS_LIB"
   echo $cmd
   time eval $cmd || exit -1
   run
fi

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

# MI250 (hipcc)
[Kernel OpDiv] CPU    Mean time: 77.8571 ms 0.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 5.94522 ms 3.4 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 5.82839 ms 3.5 GFLOPS (Tuned kernel)

# A6000 (nvc++)
[Kernel OpDiv] CPU    Mean time: 76.6936 ms 0.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 4.44986 ms 4.6 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 4.41867 ms 4.7 GFLOPS (Tuned kernel)

# A6000 (nvcc)
[Kernel OpDiv] CPU    Mean time: 77.6529 ms 0.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 4.44078 ms 4.6 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 4.2611 ms 4.8 GFLOPS (Tuned kernel)

# GFX1100 (hipcc 18.0)
[Kernel OpDiv] CPU    Mean time: 88.7953 ms 0.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 1.56686 ms 13.2 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 1.34892 ms 15.4 GFLOPS (Tuned kernel)

# A100 (nvc++)
[Kernel OpDiv] CPU    Mean time: 144.448 ms 0.1 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 1.40876 ms 14.7 GFLOPS

# H100 (nvc++)
[Kernel OpDiv] CPU    Mean time: 122.345 ms 0.1 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 0.734824 ms 28.3 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 0.674053 ms 30.8 GFLOPS (Tuned kernel)

# H100 (nvcc)
[Kernel OpDiv] CPU    Mean time: 122.737 ms 0.1 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 0.716475 ms 29 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 0.63987 ms 32.5 GFLOPS (Tuned kernel)

# MI300 (hipcc)
[Kernel OpDiv] CPU    Mean time: 179.657 ms 0.1 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 0.694193 ms 29.9 GFLOPS
[Kernel OpDiv] Kokkos Mean time: 0.512886 ms 40.5 GFLOPS (Tuned kernel)
