#!/bin/bash
mkdir -p ../build
cp * ../build
cd ../build

[ ! -f cylindre.geom ] && gunzip -c ../Cx/cylindre.geom.gz > cylindre.geom
if [ cylindre.geom -nt DOM_0000.Zones ] || [ test.data -nt DOM_0000.Zones ]
then
   # Decoupage:
   rm -f *.Zones
   # 1.5e6 mailles decoupe sur 50 Zones sur cluster, moins sur un PC
   NB_ZONES=50 && [ "$TRUST_WITHOUT_HOST" = 1 ] && NB_ZONES=$TRUST_NB_PHYSICAL_CORES
   sed -i "1,$ s?XXX?$NB_ZONES?g" test.data
   make_PAR.data test
fi
N=`ls *.Zones | wc -l`
# Clean
trust -clean 1>/dev/null 2>&1
# Calculs
options="-journal=0"
for TRUST_DISABLE_MPIIO in 0 1
do
   cas=TRUST_ENABLE_MPIIO && [ "$TRUST_DISABLE_MPIIO" = 1 ] && cas=TRUST_DISABLE_MPIIO 
   cp PAR_test.data $cas.data
   sed -i "1,$ s/cas/$cas/g" $cas.data
   echo "Running TRUST_DISABLE_MPIIO=$TRUST_DISABLE_MPIIO ..."
   TRUST_DISABLE_MPIIO=$TRUST_DISABLE_MPIIO trust $cas $N $options 1>$cas.log 2>&1 
done
cmd="sdiff TRUST_DISABLE_MPIIO.TU TRUST_ENABLE_MPIIO.TU"
eval $cmd
echo $cmd
# Test en sequentiel
TRUST_DISABLE_MPIIO=1 trust test $options 1>test.log 2>&1 && PYTHONPATH="" meld test.TU TRUST_DISABLE_MPIIO.TU TRUST_ENABLE_MPIIO.TU &
ls -lart *.xyz

