#!/bin/bash
check()
{
   if [ ! -f $1.TU ]
   then
      echo "==================================================="
      echo "Performance is KO for $1 on $2 : case does not run!"
      echo "==================================================="
      exit -1
   fi
   TU=$1.TU_OpenMP && [ "$TRUST_DISABLE_KOKKOS" != 1 ] && TU=$1.TU_Kokkos
   mv $1.TU $TU
   ref=`awk '/Secondes/ && /pas de temps/ {print $NF}' $1.TU.ref_$2`
   new=`awk '/Secondes/ && /pas de temps/ {print $NF}' $TU`
   echo $ref $new | awk '// {if (($2-$1)/($1+$2)>0.05) {exit -1}}' # On verifie qu'on ne depasse pas +5% de la performance
   err=$?
   if [ $err != 0 ]
   then
      sdiff $1.TU.ref_$2 $TU
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
   solveur=AmgX && [ "$TRUST_USE_ROCM" = 1 ] && solveur=rocALUTION
   jdd=OpenMP_Iterateur_BENCH_$solveur && [ "$3" != "" ] && jdd=$3
   if [ "$np" = "" ] || [ "$np" = 1 ]
   then
      rm -f $jdd.TU
      trust $jdd 1>$jdd.out_err 2>&1
      check $jdd $gpu
   else  
      make_PAR.data $jdd $np 1>/dev/null 2>&1
      rm -f PAR_$jdd.TU
      trust PAR_$jdd $np 1>$PAR_jdd.out_err 2>&1
      check PAR_$jdd $gpu
   fi
}
# Kokkos
[ "$TRUST_DISABLE_KOKKOS" != 1 ] && echo "Kokkos is enabled. To disable Kokkos, use TRUST_DISABLE_KOKKOS=1"
# Liste des machines:
[ $HOST = is157091 ] && run a6000 && run 1xa6000 2
[ "`hostname`" = petra ] && run a5000 && run 2xa5000 2
[ $HOST = is246827 ] && run a3000 && echo "Verifier que le portage se charge, debrancher et rebrancher la prise pour que le GPU tourne a fond !"
[ $HOST = topaze ]   && run a100   && run 4xa100 4   && run 8xa100 8 OpenMP_Iterateur_BENCH_AmgX_10
[ $HOST = adastra ]  && run MI250X && run 4xMI250X 4 && run 8xMI250X 8 OpenMP_Iterateur_BENCH_rocALUTION_10
[ $HOST = jean-zay ] && run v100-32g && run 4xv100-32g 4
[ $HOST = irene-amd-ccrt ] && run v100

