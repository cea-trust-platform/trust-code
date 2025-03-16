#!/bin/bash
# Scaling a mesh on several GPU
[ "$TRUST_ROOT" = "" ] && echo "TRUST_ROOT empty." && exit
ROOT=`pwd`
echo -e "Config     [MTET] [MDOF] TimeStep[s] Solver[s] [its] [ms/it] Kernels[s] RAM[GB] DRAM[GB] [MDOF/s] Solver Kernels Conv Diff Grad  Div"

versions=cpu && [ "$TRUST_USE_GPU" = 1 ] && versions=gpu
for version in $versions
do
   gpus="1"
   sizes="3x3x2 5x5x2 8x8x2 10x10x3 15x15x4 20x20x5 24x24x7 31x31x10 41x41x14 51x51x17 61x61x20 74x74x25 92x92x32 116x116x40 145x145x50 181x181x63"
   [ $HOST = adastra ] && mpis=192 && gpus="1 2 3 4 5 6 7 8"   
   [ $HOST = topaze ]  && mpis=128 && gpus="1 2 3 4"           
   [ $HOST = orcus-amd ]  && mpis=128 && gpus="1 2 3 4"        
   [ "`hostname`" = petra ] && mpis=32 && gpus="1 2"           
   [ "`hostname`" = is157091 ] && mpis=32 && gpus="1"          
   [ $version = cpu ] && gpus="0"
   for gpu in $gpus
   do
      [ $version = gpu ] && mpis=$gpu
      for mpi in $mpis
      do
         for size in $sizes
         do
            jdd=$mpi"_"$gpu"_"$size
            mkdir -p $ROOT/scaling/$jdd && cd $ROOT/scaling/$jdd
            # Run ?
            run=1 && [ -f $jdd.out_err ] && run=0
            [ $run = 1 ] && echo "$jdd ..."
            # Creation data
            size=`echo $size | awk '{gsub("x"," ",$0);print $0}'`
            if [ -f $ROOT/OpenMP_Iterateur.data ]
            then
               [ $version = gpu ] && [ "$TRUST_USE_ROCM" = 1 ] && cp $ROOT/OpenMP_Iterateur_BENCH_rocALUTION.data $jdd.data 
               [ $version = gpu ] && [ "$TRUST_USE_CUDA" = 1 ] && cp $ROOT/OpenMP_Iterateur_BENCH_AmgX.data $jdd.data       
               [ $version = cpu ] && cp $ROOT/OpenMP_Iterateur_BENCH_PETSc.data $jdd.data
            else
               cp $ROOT/`basename $ROOT`.data $jdd.data
               sed -i "s|/\* Nombre_de_Noeuds [0-9]\+ [0-9]\+ [0-9]\+ \*/|Nombre_de_Noeuds $size|" $jdd.data
            fi
            sed -i "1,$ s?31 31 10?$size?" $jdd.data
	    # Important change to rtol for scaling !
	    sed -i "1,$ s? seuil ? rtol ?" $jdd.data
	    sed -i "1,$ s? atol ? rtol ?" $jdd.data
	    # Important change lml en lata
	    sed -i "1,$ s? lml? lata?" $jdd.data
	    # Suppress post-processing:
	    sed -i '/Postraitement/d' $jdd.data
            # Decoupage
            [ $run = 1 ] && [ $mpi != 1 ] && (make_PAR.data $jdd $mpi 1>/dev/null 2>&1;cp PAR_$jdd.data $jdd.data)
            # Calcul
            [ $run = 1 ] && (trust $jdd $mpi -ksp_view -journal=0 1>$jdd.out_err 2>&1 || (rm -f *.TU;echo "Error:See "`pwd`/$jdd.out_err))
            [ "`grep 'Arret des process' $jdd.out_err`" = "" ] && break
            # Analyse
            hram=`awk '/RAM taken/ {if ($13>RAM) RAM=$13} END {print 0.1*int(0.01*RAM)}' $jdd.out_err`
            dram=`awk '/RAM allocated on a GPU/ {if ($1>RAM) RAM=$1} END {print RAM}' $jdd.out_err`
            row=`awk '/Order of the matrix/ {print $NF;exit}' $jdd.out_err`
            faces=`awk '/Total number of faces/ {print $NF;exit}' $jdd.out_err`
            elems=`awk '/Total number of elements/ {printf($NF);exit}' $jdd.out_err`
            # No better to use dof=row
            dof=$row
            #dof=`echo 1*$faces | bc -l` # En VDF
            #dof=`echo 3*$faces | bc -l` # En VEF
            awk -v mpi=$mpi -v gpu=$gpu -v elems=$elems -v row=$row -v dof=$dof -v hram=$hram -v dram=$dram '\
	    BEGIN {config=mpi"MPI"(gpu==0?"":"+"gpu"GPU");mdof=dof/1e6;mtet=elems/1e6} \
            /Secondes/ && /pas de temps/ {dt=$NF;dts=mdof/dt} \
            /Dont solveurs/ {ts=$4;b=dt-ts;ls=mdof/ts} \
            / operateurs convection / { gsub("\\(","",$6);conv=mdof/$4*$6 } \
            / operateurs diffusion /  { gsub("\\(","",$6);diff=mdof/$4*$6 } \
            / operateurs gradient /   { gsub("\\(","",$6);grad=mdof/$4*$6 } \
            / operateurs divergence / { gsub("\\(","",$6);dive=mdof/$4*$6 } \
            /Iterations / && / solveur/ {its=$NF;ms_it=1000*ts/its} \
            /Kernels / {ks=int(mdof/$3)} \
            END {printf("%s %7.3f %7.3f %11.3f %9.3f %5d %7.1f %10.3f %7.1f %8.1f %8.1f %6.1f %7d %4d %4d %4d %4d\n", \
	             config, mtet, mdof, dt, ts, its, ms_it,    b,   hram, dram,  dts,  ls,  ks, conv, diff, grad, dive)}' $jdd.TU
	    # Clean
	    rm -f *.sauv *.xyz *.*lata*	*.lml    
         done
      done
   done    
done
