#!/bin/bash
# Evaluation rapide de la relation RAM=f(maillage)
echo "Usage: `basename $0` cas [NB_PROCS]"
[ "$1" = "" ] && exit -1
jdd=$1
cas=$jdd
NB_PROCS=1 && [ "$2" != "" ] && NB_PROCS=$2 && cas=PAR_$jdd
tmp=`mktemp_ -d`
cd $tmp
copie_cas_test $jdd
cd $jdd
pwd

# On supprime dt_min et dt_max #
# On fixe nb_pas_dt_max a 1 #
$TRUST_Awk '!/dt_min/ && !/dt_max/ && !/nb_pas_dt_max/ {print $0} /dt_min/ {print "nb_pas_dt_max 1"}' $jdd.data > tmp.data

mv -f tmp.data $jdd.data
let loop=0
err=0
#####################################
# Boucle en raffinant a chaque fois #
#####################################
echo "# $cas $NB_PROCS" > ram.plot
echo "# MESH[elements] RAM[MB] A=LU[s] x=(A-1)B[s]" >> ram.plot
RAM=0
while [ $err = 0 ] && [ $RAM -lt 1000 ]
do
   if [ $loop -gt 0 ]
   then
      $TRUST_Awk '/omaine / {dom=$NF} /# END MESH #/ {print "Raffiner_anisotrope "dom} // {print $0}' $jdd.data > tmp.data 
      mv -f tmp.data $jdd.data 
   fi
   echo $ECHO_OPTS "$cas ($NB_PROCS CPU) refinement $loop...\c"
   # Faire appel a Check_decoupage.ksh sur chaque cas qui sortira
   # Ax=B (0) Ax=B(1) et dt(1) et RAM RAM/cell cell/Procs
   # Check_decoupage.ksh $cas -cpu
   [ "$NB_PROCS" != 1 ] && make_PAR.data $jdd $NB_PROCS 1>/dev/null 2>&1
   triou $cas $NB_PROCS 1>$cas.out 2>$cas.err
   err=$?
   if [ $err = 0 ]
   then
      RAM=`$TRUST_Awk '/RAM taken/ {RAM=($1>RAM?$1:RAM)} END {print RAM}' $cas.out`
      ELEM=`$TRUST_Awk '/Total number of elements/ {print $NF}' $cas".err"`
      RATIO=`echo $ELEM $RAM | $TRUST_Awk '{print int($2*1000*1000/$1)}'`
      LU=`$TRUST_Awk '/clock Ax=B/ {print $3;exit}' $cas".out"`
      X=`$TRUST_Awk '/clock Ax=B/ {x=$3} END {print x}' $cas".out"`
      echo "OK Mesh size $ELEM needs $RAM MB ( $RATIO bytes/cell ) A=LU: $LU s x=(A-1).B: $X s"
      echo $ELEM" "$RAM" "$RATIO" "$LU" "$X >> ram.plot
   else
      echo KO
      nedit $cas.err $cas.data
   fi
   let loop=$loop+1
done 
rm -r -f $tmp
