[ "$TRUST_USE_CUDA" = 1 ] && OPENMP="-fopenmp -mp=gpu -cuda -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart "
[ "$TRUST_USE_ROCM" = 1 ] && OPENMP="-fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=$ROCM_ARCH"
$TRUST_CC -O3 -I$MPI_ROOT/include $OPENMP -o use_device_ptr use_device_ptr.cpp || exit -1
echo "Build OK"
touch dumb.data && export exec=`pwd`/use_device_ptr
echo "===== Running on 1 process: ======"
trust dumb
echo "===== Running on 2 process: ======"
trust dumb 2
exit
# La raison de ce reproducer est la suivante:
# Chaque process essaie avec use_device_ptr de trouver le pointeur sur SON device a partir du pointeur sur le host
# Mais sur le device[2] (en fait le 1), use_device essaie de chercher le pointeur host sur le device[1] (en fait le 0)
# C'est un bug de Nvidia SDK<21.7 ! https://forums.developer.nvidia.com/t/cublas-saxpy-error/189733/3
[OpenMP] Assigning local rank 0 to device 0/1
[OpenMP] Assigning local rank 1 to device 1/1
[0] Host pointer  :0x23afb80
[0] Device pointer:0x1486d52ff000
[0] Host pointer  :0x23afb80
[1] Host pointer  :0x1ba4790
Present table dump for device[2]: NVIDIA Tesla GPU 1, compute capability 8.0, threadid=1
host:0x1ba4790 device:0x1535232ff000 size:100 presentcount:0+1 line:22 name:host_data
allocated block device:0x1535232ff000 size:512 thread:1
FATAL ERROR: data in use_device clause was not found on device 1: host:0x1ba4790

