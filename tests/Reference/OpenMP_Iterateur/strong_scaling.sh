#!/bin/bash
# Strong scaling on a GPU node:
mkdir -p strong_scaling && cd strong_scaling
echo "Config     Ax=B B"
for version in cpu gpu
do
   for mpi in 1 2 4 8 16 32
   do
      gpus=`nvidia-smi --list-gpus | wc -l` && [ $version = cpu ] && gpus=0
      for gpu in $gpus
      do
         jdd=$mpi"_"$gpu
         mkdir -p $jdd && cd $jdd
         # Run ?
         run=1 && [ -f $jdd.TU ] && [ $jdd.TU -nt $jdd.data ] && run=0
         [ $run = 1 ] && echo "$jdd ..."
         # Creation data
         [ $version = gpu ] && cp ../../OpenMP_Iterateur_BENCH_AmgX.data $jdd.data  && exec=$local/trust/amgx_openmp/exec/TRUST_mpi_opt
         [ $version = cpu ] && cp ../../OpenMP_Iterateur_BENCH_PETSc.data $jdd.data && exec=$local/trust/tma/exec/TRUST_mpi_opt
         # Decoupage
         [ $run = 1 ] && [ $mpi != 1 ] && (make_PAR.data $jdd $mpi 1>/dev/null 2>&1;cp PAR_$jdd.data $jdd.data)
         # Calcul
         [ $run = 1 ] && (trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1 || (rm -f *.TU;echo "Error:See "`pwd`/$jdd.out_err))
         # Analyse
         awk '/RAM taken/ {if ($1>RAM) RAM=$1} END {print "RAM= "RAM}' $jdd.out_err >> $jdd.TU
	 awk -v mpi=$mpi -v gpu=$gpu '/Secondes/ && /pas de temps/ {dt=$NF} /Dont solveurs/ {s=$4;b=dt-s;gsub("\\.",",",s);gsub("\\.",",",b)} END {print mpi"MPI"(gpu==0?"":"+"gpu"GPU")" "s" "b}' $jdd.TU
         cd - 1>/dev/null 2>&1
      done
   done    
done
