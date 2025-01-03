#!/bin/bash
check()
{
   [ "$2" = -nsys ] && exit
   if [ ! -f $1.TU ]
   then
      echo "==================================================="
      echo "Performance is KO for $1 on $2 : case does not run!"
      echo "==================================================="
      exit -1
   fi
   TU=$1.TU
   TU_REF=$1.TU.$2
   if [ ! -f $TU_REF ]
   then
      mv -f $TU $TU_REF && [ "$TRUST_SCM" = 1 ] && git add $TU_REF
      echo "Creating new reference $TU_REF"
      exit
   fi 
   ref=`awk '/Secondes/ && /pas de temps/ {print $NF}' $TU_REF`
   new=`awk '/Secondes/ && /pas de temps/ {print $NF}' $TU`
   echo $ref $new | awk '// {if (($2-$1)/($1+$2)>0.05) {exit 1}}' # On verifie qu'on ne depasse pas +5% de la performance
   err=$?
   if [ $err = 1 ]
   then
      sdiff $TU_REF $TU
      echo "================================"
      echo "Performance is KO for $1 on $2 !"
      echo "================================"
   else
      ecart=`echo $ref $new | awk '// {printf("%2.1f\n",-2*($1-$2)/($1+$2)*100)}'`
      echo "Performance is OK ($ecart%) $new s < $ref s (reference) for $1 on $2"
      [ `echo "$ecart<-0.5" | bc -l` = 1 ] && echo "Performance is improved so $TU_REF is updated !" && cp $TU $TU_REF
   fi
}
run()
{
   gpu=$1 && [ $gpu = -nsys ] && nsys=$gpu
   np=$2
   solveur=AmgX && [ "$TRUST_USE_ROCM" = 1 ] && solveur=rocALUTION
   jdd=OpenMP_Iterateur_BENCH_$solveur && [ "$3" != "" ] && jdd=$3
   if [ "$np" = "" ] || [ "$np" = 1 ]
   then
      rm -f $jdd.TU
      trust $nsys $jdd 1>$jdd.out_err 2>&1
      check $jdd $gpu
   else  
      make_PAR.data $jdd $np 1>/dev/null 2>&1
      rm -f PAR_$jdd.TU
      trust $nsys PAR_$jdd $np 1>$PAR_jdd.out_err 2>&1
      check PAR_$jdd $gpu"x$np"
   fi
}
# Liste des machines:
if [ "$1" = -nsys ]
then
   run -nsys
else
   [ "$TRUST_USE_CUDA" = 1 ] && GPU_ARCH=_cc$TRUST_CUDA_CC
   [ "$TRUST_USE_ROCM" = 1 ] && GPU_ARCH=_$ROCM_ARCH
   run $HOST$GPU_ARCH
   # Multi-gpu:
   [ $HOST = is157091 ]     && run $HOST$GPU_ARCH 2
   [ "`hostname`" = petra ] && run $HOST$GPU_ARCH 2
   [ $HOST = topaze ]       && run $HOST$GPU_ARCH 4 && run $HOST$GPU_ARCH 8 OpenMP_Iterateur_BENCH_AmgX_10
   [ $HOST = adastra ]      && run $HOST$GPU_ARCH 4 && run $HOST$GPU_ARCH 8 OpenMP_Iterateur_BENCH_rocALUTION_10
   [ $HOST = jean-zay ]     && run $HOST$GPU_ARCH 4
   [ $HOST = is247056 ]     && run $HOST$GPU_ARCH 2
   [ $HOST = orcus-amd ]    && run $HOST$GPU_ARCH 2
fi   
