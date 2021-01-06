#!/bin/bash
# Run a XXX cells benchmark on XXX cores on rome partition (needs a int64 binary):
[ "$TRUST_ROOT" = "" ] && echo "Initialize Trust environment." && exit -1
# Nombre de CPUs
NP=100000 	&& RAFFINE_SEQ=4 && RAFFINE_PAR=2 # On raffine a 2 459 959 296 mailles (raffinement 6 fois avec un mix seq-par)
NP=2 		&& RAFFINE_SEQ=1 && RAFFINE_PAR=1 # On raffine 2 fois de suite

# On part de GAMELAN (9384 cells)
cp $TRUST_ROOT/tests/Reference/GAMELAN/dom.geom .
make_PAR.data GAMELAN
rm -f *.Zones
   
# Modification de DEC_GAMELAN
sed -i "1,$ s?Nb_parts 2?Nb_parts $NP?g" DEC_GAMELAN.data
while [ $RAFFINE_SEQ -gt 0 ]
do
   let RAFFINE_SEQ=$RAFFINE_SEQ-1
   raffiner=$raffiner"\nraffiner_isotrope dom"
done
sed -i "1,$ s?dom.geom?dom.geom $raffiner?g" DEC_GAMELAN.data

# Creation de RAF_GAMELAN
cp -f PAR_GAMELAN.data RAF_GAMELAN.data
raffiner=""
while [ $RAFFINE_PAR -gt 0 ]
do
   let RAFFINE_PAR=$RAFFINE_PAR-1
   raffiner=$raffiner"Raffiner_isotrope_parallele { single_hdf name_of_initial_zones DOM name_of_new_zones DOM } \n"
done
sed -i "1,$ s?Scatter?$raffiner \nFIN\n Scatter?g" RAF_GAMELAN.data

echo "Maillage:"
ls -lart dom.geom
# Partition
trust DEC_GAMELAN 1>DEC_GAMELAN.log 2>&1 || exit -1
echo "Raffinement sequentiel et partition:"
ls -lart *.Zones
sleep 2

# Raffinement parallele
trust RAF_GAMELAN $NP 1>RAF_GAMELAN.log 2>&1 || exit -1
echo "Raffinement parallele:"
ls -lart *.Zones
sleep 5
# Lancement
echo "Calcul sur $NP cores..."
if [ $NP != 2 ]
then
   trust PAR_GAMELAN $NP -journal=0 2>&1 | tee PAR_GAMELAN.log
   #ccc_mprun -p rome -A dendm2s -T 3600 -n $NP $exec PAR_GAMELAN $NP -journal=0 2>&1 | tee PAR_GAMELAN.log
else
   trust PAR_GAMELAN $NP -journal=0 2>&1 | tee PAR_GAMELAN.log
fi

# Attention petsc gcp va marcher ou passer a aij ?
# Attention
# Some results:
# AMD ROME ms/it GC/SSOR Trust	Load Imbalance
# 10000				XXX
# 50000				XXX
# 100000			XXX

