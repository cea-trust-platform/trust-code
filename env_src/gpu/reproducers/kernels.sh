rm -f kernels 
# -gpu=loadcache:L1|L2s
# -gpu=ccnative
# Aucun effet (meme O3 !) sur le device...
if [ "$ROCM_PATH" != "" ]
then
   opt="-O3 -fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=$ROCM_ARCH"
else
   opt="-O3 -cuda -mp=gpu -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart"
fi
echo "opt=$opt"
$TRUST_CC_BASE $opt -o kernels kernels.cpp || exit -1
echo "Build OK"
rm -f *nsys-rep
./kernels && [ "$1" = -nsys ] && nsys profile ./kernels && nsys-ui report1.nsys-rep
# A6000 (nvc++)
# [Kernel  a+=b] gpu: 0 Mean time: 1.87278 ms 2.7 GFLOPS
# [Kernel  a+=b] gpu: 1 Mean time: 0.101147 ms 51.4 GFLOPS
# [Kernel OpDiv] gpu: 0 Mean time: 133.323 ms 0.1 GFLOPS
# [Kernel OpDiv] gpu: 1 Mean time: 4.42834 ms 4.6 GFLOPS
# GFX1032 (amdclang++ 18.0)
# [Kernel  a+=b] gpu: 0 Mean time: 3.18877 ms 1.6 GFLOPS
# [Kernel  a+=b] gpu: 1 Mean time: 0.770906 ms 6.7 GFLOPS
# [Kernel OpDiv] gpu: 0 Mean time: 248.76 ms 0 GFLOPS
# [Kernel OpDiv] gpu: 1 Mean time: 16.4671 ms 1.2 GFLOPS
# GFX90a (amdclang++ 17.0)
# [Kernel  a+=b] gpu: 0 Mean time: 2.25466 ms 2.3 GFLOPS
# [Kernel  a+=b] gpu: 1 Mean time: 0.114298 ms 45.4 GFLOPS
# [Kernel OpDiv] gpu: 0 Mean time: 126.703 ms 0.1 GFLOPS
# [Kernel OpDiv] gpu: 1 Mean time: 5.74335 ms 3.6 GFLOPS
# GFX90a (crayCC 17.0)
# [Kernel  a+=b] gpu: 0 Mean time: 2.03136 ms 2.5 GFLOPS
# [Kernel  a+=b] gpu: 1 Mean time: 0.115541 ms 45 GFLOPS
# [Kernel OpDiv] gpu: 0 Mean time: 126.924 ms 0.1 GFLOPS
# [Kernel OpDiv] gpu: 1 Mean time: 5.79228 ms 3.5 GFLOPS
