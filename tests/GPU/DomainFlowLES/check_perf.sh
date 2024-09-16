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
   TU=$1.TU
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
   [ "$3" != "" ] && jdd=$3
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
[ $HOST = is157091 ] && run a6000
