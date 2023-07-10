$TRUST_CC_BASE -O3 -mp -target=gpu -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o pinned_memory pinned_memory.cpp || exit -1
echo "Build OK"
rm -f *nsys-rep
./pinned_memory && [ "$1" = -nsys ] && nsys profile ./pinned_memory && nsys-ui report1.nsys-rep

