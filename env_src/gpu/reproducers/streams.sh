# Asynchonisme des kernels avec les streams:
rm -f streams
KOKKOS_INC="-I$TRUST_KOKKOS_ROOT/linux_opt/include"
KOKKOS_LIB="-L$TRUST_KOKKOS_ROOT/linux_opt/lib64 -lkokkoscontainers -lkokkoscore"
OPENMP="-fopenmp -mp=gpu -cuda"
$TRUST_CC -g -O3 -std=c++17 $OPENMP $KOKKOS_INC -o streams streams.cpp $KOKKOS_LIB || exit -1
echo "Build OK"
touch dumb.data && exec=`pwd`/streams trust $1 dumb 1
