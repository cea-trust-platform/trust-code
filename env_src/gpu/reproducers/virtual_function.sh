# Using virtual functio
KOKKOS="-I$TRUST_KOKKOS_ROOT/linux_opt/include -L$TRUST_KOKKOS_ROOT/linux_opt/lib64 -lkokkoscontainers -lkokkoscore" # -expt-extended-lambda -Wext-lambda-captures-this 
if [ "$ROCM_PATH" != "" ]
then
   OPENMP="-fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=$ROCM_ARCH -Wno-openmp-mapping -ldl"
else
   OPENMP="-fopenmp -mp -target=gpu -cuda"
fi
cmd="$TRUST_CC_BASE -std=c++17 $OPENMP -o virtual_function virtual_function.cpp $KOKKOS"
echo $cmd
eval $cmd && touch dumb.data && exec=`pwd`/virtual_function trust dumb 1
