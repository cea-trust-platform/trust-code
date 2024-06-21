#!/bin/bash
# Strong scaling on a GPU node:
mkdir -p strong_scaling && cd strong_scaling
echo "Config     Ax=B B"
for version in gpu # cpu gpu
do
   if [ $HOST = topaze ] || [ $HOST = irene-amd-ccrt ]
   then
      if [ $version = cpu ]
      then
         source /ccc/scratch/cont002/triocfd/triou/Version_beta_$HOST"-build64"/TRUST/env_TRUST.sh
         [ $HOST = topaze ] 		&& mpis="128 256 4096 2048 1024 512"
	 [ $HOST = irene-amd-ccrt ] 	&& mpis="48 96 192 384 768 1536"
      fi
      if [ $version = gpu ]
      then
         source $local/trust/amgx_openmp/env_TRUST.sh
         mpis="4 8 16 32 40 64"
         gpus="4"
      fi
   fi 
   [ $HOST = petra  ] && gpus="1 2" && mpis=""
   [ $version = cpu ] && gpus="0"
   for mpi in $mpis
   do
      export CUDA_VISIBLE_DEVICES=0
      for gpu in $gpus
      do
         jdd=$mpi"_"$gpu
         mkdir -p $jdd && cd $jdd
         # Run ?
         run=1 && [ -f $jdd.TU ] && run=0
         [ $run = 1 ] && echo "$jdd ..."
         # Creation data
         [ $version = gpu ] && cp ../../OpenMP_Iterateur_BENCH_AmgX_10.data $jdd.data
         [ $version = cpu ] && cp ../../OpenMP_Iterateur_BENCH_PETSc_10.data $jdd.data
         # Decoupage
         [ $run = 1 ] && [ $mpi != 1 ] && (make_PAR.data $jdd $mpi 1>/dev/null 2>&1;cp PAR_$jdd.data $jdd.data)
         # Calcul
         [ $run = 1 ] && (trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1 || (rm -f *.TU;echo "Error:See "`pwd`/$jdd.out_err))
         # Analyse
         awk '/RAM taken/ {if ($1>RAM) RAM=$1} END {print "RAM= "RAM}' $jdd.out_err >> $jdd.TU
	 awk -v mpi=$mpi -v gpu=$gpu '/Secondes/ && /pas de temps/ {dt=$NF} /Dont solveurs/ {s=$4;b=dt-s;gsub("\\.",",",s);gsub("\\.",",",b)} END {print mpi"MPI"(gpu==0?"":"+"gpu"GPU")" "s" "b}' $jdd.TU
         cd - 1>/dev/null 2>&1
	 CUDA_VISIBLE_DEVICES=$CUDA_VISIBLE_DEVICES","$gpu
      done
   done    
done
