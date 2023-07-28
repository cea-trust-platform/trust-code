#!/bin/bash
check()
{
   ref=`awk '/Secondes/ && /pas de temps/ {print $NF}' $1.TU.ref_$2`
   new=`awk '/Secondes/ && /pas de temps/ {print $NF}' $1.TU`
   echo $ref $new | awk '// {if (($2-$1)/($1+$2)>0.05) {exit -1}}' # On verifie qu'on ne depasse pas +5% de la performance
   err=$?
   if [ $err != 0 ]
   then
      sdiff $1.TU.ref_$2 $1.TU
      echo "================================"
      echo "Performance is KO for $1 on $2 !"
      echo "================================"
   else
      ecart=`echo $ref $new | awk '// {printf("%2.1f%\n",-($1-$2)/($1+$2)*100)}'`
      echo "Performance is OK ($ecart) $new s < $ref s (reference) for $1 on $2"
   fi
}
run()
{
   gpu=$1
   np=$2
   jdd=OpenMP_Iterateur_BENCH_AmgX && [ "$3" != "" ] && jdd=$3
   if [ "$np" = "" ] || [ "$np" = 1 ]
   then
      trust $jdd 1>$jdd.out_err 2>&1
      check $jdd $gpu
   else  
      make_PAR.data $jdd $np 1>/dev/null 2>&1
      trust PAR_$jdd $np 1>$PAR_jdd.out_err 2>&1
      check PAR_$jdd $gpu
   fi
}

# Liste des machines:
[ $HOST = is157091 ] && run a6000 && run 1xa6000 2
[ $HOST = petra ]    && run a5000 && run 2xa5000 2
[ $HOST = fedora ]   && run a3000
[ $HOST = topaze ]   && run a100  && run 4xa100 4  && run 8xa100 8 OpenMP_Iterateur_BENCH_AmgX_10
[ $HOST = irene-amd-ccrt ] && run v100

