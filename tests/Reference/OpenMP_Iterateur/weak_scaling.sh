#!/bin/bash
# Weak scaling on a GPU node:
mkdir -p weak_scaling && cd weak_scaling
echo "Config     Ax=B B"
for version in cpu gpu
do
   for N in 0 1 2 3 4 5 6 7 8 9
   do
      gpus=`nvidia-smi --list-gpus | wc -l` && mpi=1 && [ $version = cpu ] && gpus=0 && mpi=`echo $N $TRUST_NB_PHYSICAL_CORES | awk '{mpi=2^$1;print (mpi>$2?$2:mpi)}'`
      a=`echo $N | awk '{print 2^(0.3333*$1)}'`
      size=`echo $N | awk -v a=$a '{print int(9*a+1.5)" "int(9*a+1.5)" "int(2*a+1.5)}'`
      dof=`echo $size | awk '{print ($1-1)*($2-1)*($3-1)*40*8}'`
      for gpu in $gpus
      do
         jdd=$dof"_"$mpi"_"$gpu
         mkdir -p $jdd && cd $jdd
         # Run ?
         run=1 && [ -f $jdd.TU ] && [ $jdd.TU -nt $jdd.data ] && run=0
	 #[ $version = cpu ] && run=0
         [ $run = 1 ] && echo "$jdd ..."
         # Creation data
         [ $version = gpu ] && cp ../../OpenMP_Iterateur_BENCH_AmgX.data $jdd.data  && exec=$local/trust/amgx_openmp/exec/TRUST_mpi_opt
         [ $version = cpu ] && cp ../../OpenMP_Iterateur_BENCH_PETSc.data $jdd.data && exec=$local/trust/tma/exec/TRUST_mpi_opt
	 sed -i "1,$ s?Nombre_de_Noeuds 31 31 10?Nombre_de_Noeuds $size?g" $jdd.data
         # Decoupage
         [ $run = 1 ] && [ $mpi != 1 ] && (make_PAR.data $jdd $mpi 1>/dev/null 2>&1;cp PAR_$jdd.data $jdd.data)
         # Calcul
         [ $run = 1 ] && (trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1 || (rm -f *.TU;echo "Error:See "`pwd`/$jdd.out_err))
         # Analyse
         awk '/RAM taken/ {if ($1>RAM) RAM=$1} END {print "RAM= "RAM}' $jdd.out_err >> $jdd.TU
	 awk -v mpi=$mpi -v gpu=$gpu -v dof=$dof '/Secondes/ && /pas de temps/ {dt=$NF} /Dont solveurs/ {s=$4;b=dt-s;gsub("\\.",",",s);gsub("\\.",",",b)} END {print dof" "mpi"MPI"(gpu==0?"":"+"gpu"GPU")" "s" "b}' $jdd.TU
         cd - 1>/dev/null 2>&1
      done
   done    
done
