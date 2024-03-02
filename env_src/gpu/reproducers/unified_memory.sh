# MemoryManaged: une seule memoire avec des allers-retours entre CPU et GPU par page de 4000 octets (500 doubles). Donc 100^3, nbfaces=100^2=10000 faces donc 20 pages... Interessant.
# Il semble que le retour sur le host sera plus rapide qu'actuellement a cause de l'inlining, mais les premiers envois plus lents ainsi que les retours sur le device
# Bref a tester pour voir si l'on gagne vraiment.
rm -f unified_memory
$TRUST_CC_BASE -O3 -mp -target=gpu -gpu=managed -I$CUDA_ROOT/include -L$CUDA_ROOT/lib64 -lcudart -o unified_memory unified_memory.cpp || exit -1
echo "Build OK"
rm -f *nsys-rep
./unified_memory && [ "$1" = -nsys ] && exec=`pwd`/unified_memory && trust -nsys dumb

