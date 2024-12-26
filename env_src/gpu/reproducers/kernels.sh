#!/bin/bash
rm -f kernels 
# -gpu=loadcache:L1|L2s
# -gpu=ccnative
# Aucun effet (meme O3 !) sur le device...
KOKKOS_INC="-I$TRUST_KOKKOS_ROOT/linux_opt/include"
KOKKOS_LIB="-L$TRUST_KOKKOS_ROOT/linux_opt/lib64 -lkokkoscontainers -lkokkoscore"
if [ "$TRUST_USE_KOKKOS_HIP" = 1  ]
then
   # Build HIP code with hipcc or (crayCC -xhip):
   HIP="hipcc -xhip --offload-arch=$ROCM_ARCH -march=$ROCM_ARCH"
   time $HIP -g -O3 -std=c++17 $KOKKOS_INC -o kernels kernels.cpp $KOKKOS_LIB || exit -1
else
   # Mix OpenMP target and Kokkos in the same file:
   NVCPP="nvc++ -cuda -L$CUDA_ROOT/lib64/stubs -lcuda"
   time $NVCPP -g -O3 -std=c++17 $KOKKOS_INC -o kernels kernels.cpp $KOKKOS_LIB || exit -1
fi
echo "Build OK"
rm -f *nsys-rep
touch dumb.data && exec=`pwd`/kernels trust dumb 1
[ "$1" = -nsys ] && nsys profile ./kernels && nsys-ui report1.nsys-rep
# A6000 (nvc++)
# [Kernel  a+=b] CPU    Mean time: 1.87278 ms 2.7 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.101147 ms 51.4 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 133.323 ms 0.1 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 4.42834 ms 4.6 GFLOPS
# GFX1100  (amdclang++ 18.0)
# [Kernel  a+=b] CPU    Mean time: 1.15491 ms 4.5 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.181919 ms 28.5 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 210.977 ms 0 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 2.06195 ms 10 GFLOPS
# [Kernel OpDiv] KOMPT  Mean time: 1.49332 ms 13.9 GFLOPS  !!! Marche tres bien le backend OMPT de Kokkos ici
# GFX1032 (amdclang++ 18.0)
# [Kernel  a+=b] CPU    Mean time: 3.18877 ms 1.6 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.770906 ms 6.7 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 248.76 ms 0 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 16.4671 ms 1.2 GFLOPS
# GFX90a (amdclang++ 17.0)
# [Kernel  a+=b] CPU    Mean time: 2.25466 ms 2.3 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.114298 ms 45.4 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 126.703 ms 0.1 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 5.74335 ms 3.6 GFLOPS
# GFX90a (crayCC 17.0)
# [Kernel  a+=b] CPU    Mean time: 2.03136 ms 2.5 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.115541 ms 45 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 126.924 ms 0.1 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 5.79228 ms 3.5 GFLOPS
# GFX90a (crayCC 17.0) with HIP Kokkos backend:
# [Kernel  a+=b] CPU    Mean time: 1.41074 ms 3.6 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.324143 ms 16 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 124.607 ms 0.1 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 7.9224 ms 2.6 GFLOPS
# [Kernel OpDiv] Kokkos Mean time: 6.40479 ms 3.2 GFLOPS
# [Kernel  a+=b] CPU    Mean time: 1.3869  ms  3.7 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.0686  ms 75.7 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 123.17  ms  0.1 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 5.82141 ms  3.5 GFLOPS
# [Kernel OpDiv] KOMPT  Mean time: 74.8338 ms  0.2 GFLOPS	!!! x15 slowdown with Kokkos OpenMPtarget backend on MI250 !!!
# A3000 (nvc++)
# [Kernel  a+=b] CPU    Mean time: 2.0595  ms  2.5 GFLOPS
# [Kernel  a+=b] OMPT   Mean time: 0.21914 ms 23.7 GFLOPS
# [Kernel OpDiv] CPU    Mean time: 144.328 ms  0.1 GFLOPS
# [Kernel OpDiv] OMPT   Mean time: 10.7632 ms  1.9 GFLOPS
# [Kernel OpDiv] KCUDA  Mean time: 10.7454 ms  1.9 GFLOPS
