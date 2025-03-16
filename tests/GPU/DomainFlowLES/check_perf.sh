#!/bin/bash
check()
{
   [ "$2" = -nsys ] && exit
   grep "AMG solver" $1.out_err 2>/dev/null
   if [ ! -f $1.TU ] || [ "`grep 'Arret des processes' $1.out_err`" = "" ]
   then
      echo "==================================================="
      echo "Performance is KO for $1 on $2 : case does not run!"
      echo "==================================================="
      exit -1
   fi
   TU=$1.TU
   TU_REF=$1.TU.$2`[ "$np" != "" ] && echo x$np`
   if [ ! -f $TU_REF ]
   then
      mv -f $TU $TU_REF && [ "$TRUST_SCM" = 1 ] && git add $TU_REF
      echo "Creating new reference $TU_REF"
      exit
   fi   
   ref=`awk '/Secondes/ && /pas de temps/ {print $NF}' $TU_REF`
   new=`awk '/Secondes/ && /pas de temps/ {print $NF}' $TU`
   echo $ref $new | awk '// {if (2*($2-$1)/($1+$2)>0.05) {exit 1}}' # On verifie qu'on ne depasse pas +5% de la performance
   err=$?
   ecart=`echo $ref $new | awk '// {printf("%2.1f\n",200*($2-$1)/($1+$2))}'`
   if [ $err = 1 ]
   then
      sdiff $TU_REF $TU
      echo "=========================================="
      echo "Performance is KO ($ecart%) for $1 on $2 !"
      echo "=========================================="
   else
      echo "Performance is OK ($ecart%) $new s < $ref s (reference) for $1 on $2"
      if [ `echo "$ecart<-0.99" | bc -l` = 1 ]
      then
         echo "Performance is improved so $TU_REF is updated !" && cp $TU $TU_REF
      else
         # Non regression faiblement testee sur le nombre d'iterations des solveurs
         its_ref=`awk '/Iterations/ && /solveur/ {print $NF}' $TU_REF`
         its_new=`awk '/Iterations/ && /solveur/ {print $NF}' $TU`
         [ $its_ref != $its_new ] && echo "Solver convergence is different ($its_ref != $its_new) ! Possible regression..." && exit -1
      fi
   fi
}
run()
{
   gpu=$1 && [ $gpu = -nsys ] && nsys=$gpu
   np=$2
   [ "$3" != "" ] && jdd=$3
   if [ "$np" = "" ] || [ "$np" = 1 ]
   then
      np=""
   else  
      make_PAR.data $jdd $np 1>/dev/null 2>&1
      jdd=PAR"_"$jdd
   fi
   rm -f $jdd.TU
   trust $nsys $jdd $np 1>$jdd.out_err 2>&1
   check $jdd $gpu
}
# Jeu de donnees
jdd=`pwd`
jdd=`basename $jdd`
cp $jdd.data $jdd"_BENCH".data
jdd=$jdd"_BENCH"
for pattern in Nombre_de_Noeuds raffiner_isotrope
do
   sed -i "/"$pattern"/s/\/\*//g" $jdd.data
   sed -i "/"$pattern"/s/\*\///g" $jdd.data
done
# Replace lml format by lata for faster IO
sed -i "1,$ s?format lml?format lata?g" $jdd.data
# Surcharge CLI: check_perf.sh [-nsys] [$np] [$jdd]
nsys="" && [ "$1" = -nsys ] && nsys="-nsys"
np=1 && [ "$1" != "" ] && np=$1 && shift
[ "$1" != "" ] && jdd=$1 && shift

# Liste des machines:
if [ "$nsys" != "" ]
then
   run $nsys
else
   # MPI Ranks:
   [ "$TRUST_USE_GPU" != 1 ] && np=8
   # HOST:   
   HOST=${HOST%.intra.cea.fr} && [ "$HOST" = portable ] && HOST=is246827
   # ARCH:
   GPU_ARCH=""
   [ "$TRUST_USE_CUDA" = 1 ] && GPU_ARCH=_cc$TRUST_CUDA_CC
   [ "$TRUST_USE_ROCM" = 1 ] && GPU_ARCH=_$ROCM_ARCH
   run $HOST$GPU_ARCH $np
fi
# clean
rm -f *.sauv *.lml *.sqlite *.nsys-rep
