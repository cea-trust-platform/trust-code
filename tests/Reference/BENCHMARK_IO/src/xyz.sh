#!/bin/bash
jdd=xyz
source ./prepare
# Calculs
options="-journal=0"
for TRUST_DISABLE_MPIIO in 1 0
do
   cas=TRUST_ENABLE_MPIIO && [ "$TRUST_DISABLE_MPIIO" = 1 ] && cas=TRUST_DISABLE_MPIIO 
   cp PAR_$jdd.data $cas.data
   sed -i "1,$ s/cas/$cas/g" $cas.data
   echo "Running 1e6 cells with NP=$NB_ZONES CPUs TRUST_DISABLE_MPIIO=$TRUST_DISABLE_MPIIO :"
   TRUST_DISABLE_MPIIO=$TRUST_DISABLE_MPIIO trust $cas $N $options 1>$cas.log 2>&1 
   grep "Secondes / sauvegarde" $cas.TU
   grep "Debit " $cas.TU
done
meld TRUST_DISABLE_MPIIO.TU TRUST_ENABLE_MPIIO.TU
exit
cmd="sdiff TRUST_DISABLE_MPIIO.TU TRUST_ENABLE_MPIIO.TU"
eval $cmd
exit
echo $cmd
# Test en sequentiel
TRUST_DISABLE_MPIIO=1 trust $cas $options 1>$cas.log 2>&1 && PYTHONPATH="" meld $cas.TU TRUST_DISABLE_MPIIO.TU TRUST_ENABLE_MPIIO.TU &
ls -lart *.xyz

