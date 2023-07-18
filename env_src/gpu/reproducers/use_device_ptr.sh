$TRUST_CC_BASE -O3 -mp -target=gpu -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o use_device_ptr use_device_ptr.cpp || exit -1
echo "Build OK"
./use_device_ptr
