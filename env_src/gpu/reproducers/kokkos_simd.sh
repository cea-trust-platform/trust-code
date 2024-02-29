ARCH=linux_opt
KOKKOS="-I$TRUST_KOKKOS_ROOT/$ARCH/include -L$TRUST_KOKKOS_ROOT/$ARCH/lib64 -lkokkoscontainers -lkokkoscore -lkokkossimd"
$TRUST_CC --std=c++17 -o kokkos_simd kokkos_simd.cpp $KOKKOS && touch dumb.data && exec=`pwd`/kokkos_simd trust dumb 1
