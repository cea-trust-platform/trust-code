#!/bin/bash
# Previent par la cloche chaque nouvel instant
# Fichier meshtv_index
fic=$1
n=`cat $fic | $TRUST_Awk '/TEMPS/ {n++} END {print n-1}'`
#tail -1cf $fic | $TRUST_Awk -v n=$n '/TEMPS/ {n++;print "Time "n" available t="$2}'
tail -1cf $fic | $TRUST_Awk -v n=$n '/TEMPS/ {n++;print "Time "n" available : "$2" s"}'

# La suite inutile desormais
taille=`ls -la $fic | $TRUST_Awk '{print $5}'`
while [ 1 ]
do
   sleep 1
   if [ "`ls -la $fic | $TRUST_Awk '{print $5}'`" != $taille ]
   then
       # La cloche !
       echo $ECHO_OPTS "\c"
       cat $fic 2>/dev/null | $TRUST_Awk '/TEMPS/ {n++;t=$2} END {n--;print "Time "n" available t="t}'
       taille=`ls -la $fic | $TRUST_Awk '{print $5}'`
   fi
done
