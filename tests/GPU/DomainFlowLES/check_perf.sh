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
   # Non regression faiblement testee sur le nombre d'iterations des solveurs
   ref=`awk '/Iterations/ && /solveur/ {print $NF}' $1.TU.ref_$2`
   new=`awk '/Iterations/ && /solveur/ {print $NF}' $TU`
   [ $ref != $new ] && echo "Solver convergence is different ($ref != $new) ! Possible regression..." && exit -1

   ref=`awk '/Secondes/ && /pas de temps/ {print $NF}' $1.TU.ref_$2`
   new=`awk '/Secondes/ && /pas de temps/ {print $NF}' $TU`
   echo $ref $new | awk '// {if (2*($2-$1)/($1+$2)>0.05) {exit 1}}' # On verifie qu'on ne depasse pas +5% de la performance
   err=$?
   ecart=`echo $ref $new | awk '// {printf("%2.1f\n",200*($2-$1)/($1+$2))}'`
   if [ $err = 1 ]
   then
      sdiff $1.TU.ref_$2 $TU
      echo "=========================================="
      echo "Performance is KO ($ecart%) for $1 on $2 !"
      echo "=========================================="
   else
      echo "Performance is OK ($ecart%) $new s < $ref s (reference) for $1 on $2"
      [ `echo "$ecart<-0.99" | bc -l` = 1 ] && echo "Performance is improved so $1.TU.ref_$2 is updated !" && cp $TU $1.TU.ref_$2
   fi
}
run()
{
   gpu=$1 && [ $gpu = -nsys ] && nsys=$gpu
   np=$2
   [ "$3" != "" ] && jdd=$3
   if [ "$np" = "" ] || [ "$np" = 1 ]
   then
      rm -f $jdd.TU
      trust $nsys $jdd 1>$jdd.out_err 2>&1
      check $jdd $gpu
   else  
      make_PAR.data $jdd $np 1>/dev/null 2>&1
      rm -f PAR_$jdd.TU
      trust $nsys PAR_$jdd $np 1>$PAR_jdd.out_err 2>&1
      check PAR_$jdd $gpu
   fi
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

# Liste des machines:
[ "$1" = -nsys ] && run -nsys
[ $HOST = is157091 ] && run a6000
# clean
rm -f *.sauv *.lml *.sqlite *.nsys-rep
