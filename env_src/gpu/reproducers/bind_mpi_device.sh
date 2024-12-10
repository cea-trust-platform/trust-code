KOKKOS="-I$TRUST_KOKKOS_ROOT/linux_opt/include -L$TRUST_KOKKOS_ROOT/linux_opt/lib64 -lkokkoscontainers -lkokkoscore" # -expt-extended-lambda -Wext-lambda-captures-this 
OPENMP="-fopenmp -mp=gpu -cuda -L$CUDA_ROOT/lib64/stubs -lcuda -g"
$TRUST_CC -std=c++17 $OPENMP -o bind_mpi_device bind_mpi_device.cpp -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart $KOKKOS && touch dumb.data && exec=`pwd`/bind_mpi_device trust dump 8

