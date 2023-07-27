$TRUST_CC -mp -target=gpu -O3 -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o gpu_direct gpu_direct.cpp && touch dumb.data && exec=`pwd`/gpu_direct trust dumb 2

