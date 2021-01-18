#!/bin/bash
jdd=xyz
source ./prepare
# Calculs
options="-journal=0"
for cas in EcrFicPartageBin EcrFicPartageMPIIO
do
   cp PAR_$jdd.data $cas.data
   sed -i "1,$ s/cas/$cas/g" $cas.data || exit -1
   echo "Writing xyz file with $cas :"
   trust $cas $N $options 1>$cas.log 2>&1 
   grep "Secondes / sauvegarde" $cas.TU
   grep "Debit " $cas.TU
done
meld EcrFicPartageBin.TU EcrFicPartageMPIIO.TU &
meld EcrFicPartageBin_comm.TU EcrFicPartageMPIIO_comm.TU

