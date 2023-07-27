$TRUST_CC -mp -target=gpu -g -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o bind_mpi_device bind_mpi_device.cpp && touch dumb.data && exec=`pwd`/bind_mpi_device trust dump 8

