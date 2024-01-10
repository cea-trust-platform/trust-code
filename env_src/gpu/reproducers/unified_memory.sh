# L'option -gpu=managed fait TOUTES les allocations dynamiques dans la memoire unifiee
# 
rm -f unified_memory
$TRUST_CC_BASE -O3 -mp -target=gpu -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o unified_memory unified_memory.cpp || exit -1
echo "Build OK"
rm -f *nsys-rep
./unified_memory && [ "$1" = -nsys ] && exec=`pwd`/unified_memory && trust -nsys dumb

