ARCH=linux_opt
KOKKOS="-I$TRUST_KOKKOS_ROOT/$ARCH/include -L$TRUST_KOKKOS_ROOT/$ARCH/lib64 -lkokkoscontainers -lkokkoscore -lkokkossimd -ldl"
cmd="$TRUST_CC_BASE -g -O3 -march=native --std=c++17 -ftree-vectorize -o kokkos_simd kokkos_simd.cpp -lmvec $KOKKOS"
echo $cmd
eval $cmd && touch dumb.data && exec=`pwd`/kokkos_simd trust $1 dumb
