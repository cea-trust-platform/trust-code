#!/bin/bash
# Scaling a mesh on several GPU
[ "$TRUST_ROOT" = "" ] && echo "TRUST_ROOT empty." && exit
mkdir -p scaling && cd scaling
echo -e "Config \t\tDOF[M] \t\tAx=B[s] \tB[s] \t\tHRAM[GB] \tDRAM[GB] \tKernel[MDOF/s]"
for version in gpu cpu
do
   gpus="1"
   [ $HOST = adastra ] && mpis=192 && gpus="1 2 3 4 5 6 7 8"   && sizes="15x15x4 20x20x5 24x24x7 31x31x10 41x41x14 51x51x17 61x61x20 74x74x25 92x92x32"
   [ $HOST = topaze ]  && mpis=128 && gpus="1 2 3 4"           && sizes="15x15x4 20x20x5 24x24x7 31x31x10 41x41x14 51x51x17 61x61x20 74x74x25 92x92x32"
   [ "`hostname`" = petra ] && mpis=32 && gpus="1 2"           && sizes="15x15x4 20x20x5 24x24x7 31x31x10 41x41x14 51x51x17 61x61x20"
   [ "`hostname`" = is157091 ] && mpis=32 && gpus="1"          && sizes="15x15x4 20x20x5 24x24x7 31x31x10 41x41x14 51x51x17 61x61x20"
   [ $version = cpu ] && gpus="0"
   for gpu in $gpus
   do
      [ $version = gpu ] && mpis=$gpu
      for mpi in $mpis
      do
         for size in $sizes
         do
            jdd=$mpi"_"$gpu"_"$size
            mkdir -p $jdd && cd $jdd
            # Run ?
            run=1 && [ -f $jdd.TU ] && run=0
            [ $run = 1 ] && echo "$jdd ..."
            # Creation data
            [ $version = gpu ] && [ "$TRUST_USE_ROCM" = 1 ] && cp ../../OpenMP_Iterateur_BENCH_rocALUTION.data $jdd.data  && exec=$TRUST_ROOT/exec/TRUST_mpi_opt
            [ $version = gpu ] && [ "$TRUST_USE_CUDA" = 1 ] && cp ../../OpenMP_Iterateur_BENCH_AmgX.data $jdd.data        && exec=$TRUST_ROOT/exec/TRUST_mpi_opt
            [ $version = cpu ] && cp ../../OpenMP_Iterateur_BENCH_PETSc.data $jdd.data && exec=$local/trust/tma/exec/TRUST_mpi_opt
            size=`echo $size | awk '{gsub("x"," ",$0);print $0}'`
            sed -i "1,$ s?31 31 10?$size?" $jdd.data
            # Decoupage
            [ $run = 1 ] && [ $mpi != 1 ] && (make_PAR.data $jdd $mpi 1>/dev/null 2>&1;cp PAR_$jdd.data $jdd.data)
            # Calcul
            [ $run = 1 ] && (trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1 || (rm -f *.TU;echo "Error:See "`pwd`/$jdd.out_err))
            # Analyse
            awk '/RAM taken/ {if ($1>RAM) RAM=$1} END {print "HRAM= "0.1*int(0.01*RAM)}' $jdd.out_err >> $jdd.TU
            awk '/RAM allocated on a GPU/ {if ($1>RAM) RAM=$1} END {print "DRAM= "RAM}' $jdd.out_err >> $jdd.TU
            #dof=`awk '/Total number of elements/ {print $NF;exit}' $jdd.out_err`
            dof=`awk '/Order of the matrix/ {print $NF;exit}' $jdd.out_err`
            ks=`awk -v dof=$dof '/Kernels / {print 0.001*0.001*dof/$3;exit}' $jdd.TU`
            ks=`awk -v dof=$dof '/solveurs / {print 0.001*0.001*dof/$4;exit}' $jdd.TU`
            awk -v mpi=$mpi -v gpu=$gpu -v dof=$dof -v ks=$ks '/Secondes/ && /pas de temps/ {dt=$NF} /Dont solveurs/ {s=$4;b=dt-s} /HRAM=/ {RAM=$2} /DRAM=/ {DRAM=$2} END {print mpi"MPI"(gpu==0?"":"+"gpu"GPU")" \t"dof"   \t"s" \t"b" \t"RAM" \t\t"DRAM" \t\t"ks}' $jdd.TU
            cd - 1>/dev/null 2>&1
         done
      done
   done    
done
