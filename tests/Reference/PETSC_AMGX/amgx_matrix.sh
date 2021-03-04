!/bin/bash
# Resout une matrice PETSC avec AmgXWrapper+Amgx
echo "Usage: $0 [file.petsc]"
ROOT=$TRUST_ROOT/lib/src/LIBAMGX/AmgXWrapper
export exec=$ROOT/example/solveFromFiles/bin/solveFromFiles
configs=$ROOT/example/solveFromFiles/configs
timings()
{
  awk '/setup/ {setup=$0} /Iterations / {its=$0} /Memory Usage/ {mem=$0} /averaged/ {print $0" "setup" "its" "mem}' $1
}
if [ "$1" != "" ]
then
   # Test GPU avec AMGX
   export CUDA_VISIBLE_DEVICES=0
   echo CUDA_VISIBLE_DEVICES=$CUDA_VISIBLE_DEVICES
   [ ! -f AmgX_SolverOptions_Classical.info ] && cp $configs/AmgX_SolverOptions_Classical.info .
   NP=`echo $1 | awk -F_ '{print $4}'`
   trust -gpu dummy $NP -caseName $1 -mode AmgX_GPU -matrixFileName $1 -rhsFileName $1 -cfgFileName ./AmgX_SolverOptions_Classical.info -exactFileName /dev/null
   timings $log
fi

