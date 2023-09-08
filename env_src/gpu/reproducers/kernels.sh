rm -f kernels 
# -gpu=loadcache:L1|L2s
# -gpu=ccnative
# Aucun effet (meme O3 !) sur le device...
opt="-O3 -mp -target=gpu"
echo "opt=$opt"
$TRUST_CC_BASE $opt -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o kernels kernels.cpp || exit -1
echo "Build OK"
rm -f *nsys-rep
./kernels && [ "$1" = -nsys ] && nsys profile ./kernels && nsys-ui report1.nsys-rep

