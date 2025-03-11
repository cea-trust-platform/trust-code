#!/bin/bash
# Scaling a mesh on several GPU
[ "$TRUST_ROOT" = "" ] && echo "TRUST_ROOT empty." && exit
ROOT=`pwd`
mkdir -p scaling && cd scaling
echo -e "Config \t\tCells[M] \tSolver[s] \tSolver[its] \tKernels[s] \tRAM[GB] \tDRAM[GB] \tSolver[MDOF/s/it] \tKernels[MDOF/s] All Conv Diff Grad Div"
for version in gpu cpu
do
   gpus="1"
   sizes="8x8x2 10x10x3 15x15x4 20x20x5 24x24x7 31x31x10 41x41x14 51x51x17 61x61x20"
   [ $HOST = adastra ] && mpis=192 && gpus="1 2 3 4 5 6 7 8"   && sizes=$sizes" 74x74x25 92x92x32"
   [ $HOST = topaze ]  && mpis=128 && gpus="1 2 3 4"           && sizes=$sizes" 74x74x25 92x92x32"
   [ $HOST = orcus-amd ]  && mpis=128 && gpus="1 2 3 4"        && sizes=$sizes" 74x74x25 92x92x32"
   [ "`hostname`" = petra ] && mpis=32 && gpus="1 2"           && sizes=$sizes" "
   [ "`hostname`" = is157091 ] && mpis=32 && gpus="1"          && sizes=$sizes" "
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
            size=`echo $size | awk '{gsub("x"," ",$0);print $0}'`
            if [ -f ../../OpenMP_Iterateur.data ]
            then
               [ $version = gpu ] && [ "$TRUST_USE_ROCM" = 1 ] && cp ../../OpenMP_Iterateur_BENCH_rocALUTION.data $jdd.data  && exec=$TRUST_ROOT/exec/TRUST_mpi_opt
               [ $version = gpu ] && [ "$TRUST_USE_CUDA" = 1 ] && cp ../../OpenMP_Iterateur_BENCH_AmgX.data $jdd.data        && exec=$TRUST_ROOT/exec/TRUST_mpi_opt
               [ $version = cpu ] && cp ../../OpenMP_Iterateur_BENCH_PETSc.data $jdd.data && exec=$local/trust/tma/exec/TRUST_mpi_opt
            else
               cp ../../`basename $ROOT`.data $jdd.data
               sed -i "s|/\* Nombre_de_Noeuds [0-9]\+ [0-9]\+ [0-9]\+ \*/|Nombre_de_Noeuds $size|" $jdd.data
               #nedit $jdd.data
            fi
            sed -i "1,$ s?31 31 10?$size?" $jdd.data
            # Decoupage
            [ $run = 1 ] && [ $mpi != 1 ] && (make_PAR.data $jdd $mpi 1>/dev/null 2>&1;cp PAR_$jdd.data $jdd.data)
            # Calcul
            [ $run = 1 ] && (trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1 || (rm -f *.TU;echo "Error:See "`pwd`/$jdd.out_err))
            [ ${PIPESTATUS[0]} != 0 ] && break
            # Analyse
            awk '/RAM taken/ {if ($1>RAM) RAM=$1} END {print "HRAM= "0.1*int(0.01*RAM)}' $jdd.out_err >> $jdd.TU
            awk '/RAM allocated on a GPU/ {if ($1>RAM) RAM=$1} END {print "DRAM= "RAM}' $jdd.out_err >> $jdd.TU
            row=`awk '/Order of the matrix/ {print $NF;exit}' $jdd.out_err`
            faces=`awk '/Total number of faces/ {print $NF;exit}' $jdd.out_err`
            elems=`awk '/Total number of elements/ {print $NF;exit}' $jdd.out_err`
            dof=`echo 1*$faces | bc -l` # En VDF
            dof=`echo 3*$faces | bc -l` # En VEF
            awk -v mpi=$mpi -v gpu=$gpu -v elems=$elems -v row=$row -v dof=$dof '\
            /Secondes/ && /pas de temps/ {dt=$NF} \
            /Dont solveurs/ {ts=$4;b=dt-ts} \
            / operateurs convection / { gsub("\\(","",$6);conv=int(0.001*0.001*dof/$4*$6) } \
            / operateurs diffusion /  { gsub("\\(","",$6);diff=int(0.001*0.001*dof/$4*$6) } \
            / operateurs gradient /   { gsub("\\(","",$6);grad=int(0.001*0.001*dof/$4*$6) } \
            / operateurs divergence / { gsub("\\(","",$6);dive=int(0.001*0.001*dof/$4*$6) } \
            /Iterations / && / solveur/ {its=$NF;ls=int(0.001*0.001*row/(ts/its))} \
            /Kernels / {ks=int(0.001*0.001*dof/$3)} \
            /HRAM=/ {RAM=$2} \
            /DRAM=/ {DRAM=$2} \
            END {print mpi"MPI"(gpu==0?"":"+"gpu"GPU")" \t"elems"   \t"ts" \t"its" \t\t"b" \t"RAM" \t\t"DRAM" \t\t"ls" \t\t\t"ks" "conv" "diff" "grad" "dive}' $jdd.TU
            cd - 1>/dev/null 2>&1
         done
      done
   done    
done
