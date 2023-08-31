# It seems -gpu=pinned with nvc++ improve greatly. Doit apparaitre au link aussi
# Essayer sur TRUST mais cela crashe au demarrage avec free(): invalid pointer ...
# Non utlise dans TRUST...
$TRUST_CC_BASE -O3 -mp -target=gpu -gpu=cuda12.1 -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o pinned_memory pinned_memory.cpp || exit -1
echo "Build OK"
rm -f *nsys-rep
./pinned_memory && [ "$1" = -nsys ] && nsys profile ./pinned_memory && nsys-ui report1.nsys-rep

