#!/bin/bash
# Weak scaling on nodes:
# Selon machines:
ext=$1 # Exemple: _10 pour le cas a 80 MDOF. Attention out of memory sur 2GPUs
unset TRUST_USE_MPI_GPU_AWARE
benchs="gpu cpu"              && mpis_gpu="1 2"          && mpis_cpu="1 8"              && int=""       && jdd_gpu=AmgX             && jdd_cpu=PETSc  && TRUST_USE_MPI_GPU_AWARE=1
if [ "$HOST" = adastra ]
then
   [ "$TRUST_USE_ROCM" = 1 ]  && benchs="gpu"             && mpis_gpu="4 32 256"     && int="_int64" && ext_gpu=BENCH_AMD        && TRUST_USE_MPI_GPU_AWARE=1
   [ "$TRUST_USE_ROCM" != 1 ] && benchs="cpu"             && mpis_cpu="64 512 4096 32768" && int="_int64" && ext_cpu=BENCH_CPU
fi
if [ "$HOST" = topaze ]
then
   [ "$TRUST_USE_CUDA" = 1 ]  && benchs="gpu"             && mpis_gpu="2 16 128"     && int="_int64" && ext_gpu=BENCH_NVIDIA     && TRUST_USE_MPI_GPU_AWARE=1
   [ "$TRUST_USE_CUDA" != 1 ] && benchs="cpu"             && mpis_cpu="64 512 4096 32768" && int="_int64" && ext_cpu=BENCH_CPU
fi
[ "$HOST" = jean-zay ]        && benchs="gpu"             && mpis_gpu="2 16 128 512"      && int="_int64" && ext_gpu=BENCH_NVIDIA
[ "$HOST" = irene-amd-ccrt ]  && benchs="cpu"             && mpis_cpu="64 512 4096 32768" && int="_int64" && ext_cpu=BENCH_CPU
[ "$1" = "-disable_mpi_gpu_aware" ] && unset TRUST_USE_MPI_GPU_AWARE
export TRUST_USE_MPI_GPU_AWARE
env_gpu=$TRUST_ROOT/env_TRUST.sh
env_cpu=$local/trust/tma$int/env_TRUST.sh
cas=`pwd`
cas=`basename $cas .data`
jdd_cpu=$cas && [ "$ext_cpu" != "" ] && jdd_cpu=$jdd_cpu"_"$ext_cpu$ext.data
jdd_gpu=$cas && [ "$ext_gpu" != "" ] && jdd_gpu=$jdd_gpu"_"$ext_gpu$ext.data
[ ! -f $env_cpu ] && jdd_cpu=""

mkdir -p weak_scaling$ext && cd weak_scaling$ext
echo -e "Host     \tDOF \t\tConfig       \tTime/dt[s]   \tWith Ax=B[s] \tWith B[s]    \tMDOF/s \tIters \tLoadImb Energy[J] \tGPU Direct\t DRAM[Gb] \t ms/Iter"
for bench in $benchs
do
   [ $bench = gpu ] && mpis=$mpis_gpu && source $env_gpu 1>/dev/null
   [ $bench = cpu ] && mpis=$mpis_cpu && source $env_cpu 1>/dev/null
   smallest_mpi=1000000
   for mpi in $mpis
   do
      [ $mpi -lt $smallest_mpi ] && smallest_mpi=$mpi
   done
   for mpi in $mpis
   do
      #gpus=`nvidia-smi --list-gpus | wc -l` && mpi=1 && [ $bench = cpu ] && gpus=0 && mpi=`echo $N $TRUST_NB_PHYSICAL_CORES | awk '{mpi=2^$1;print (mpi>$2?$2:mpi)}'`
      #a=`echo $N | awk '{print 2^(0.3333*$1)}'`
      #size=`echo $N | awk -v a=$a '{print int(9*a+1.5)" "int(9*a+1.5)" "int(2*a+1.5)}'`
      #dof=`echo $size | awk '{print ($1-1)*($2-1)*($3-1)*40*8}'`
      jdd=$mpi"C" && [ "$bench" = gpu ] && jdd=$jdd"_"$mpi"G" 
      mkdir -p $jdd && cd $jdd
      # Run ?
      run=1 && [ -f PAR_$jdd.TU ] && [ PAR_$jdd.TU -nt PAR_$jdd.data ] && run=0
      #[ $bench = cpu ] && run=0
      [ $run = 1 ] && echo "$jdd ..."
      # Creation data
      [ $bench = gpu ] && cp ../../$jdd_gpu.data $jdd.data
      [ $bench = cpu ] && cp ../../$jdd_cpu.data $jdd.data
      #sed -i "1,$ s?Nombre_de_Noeuds 31 31 10?Nombre_de_Noeuds $size?g" $jdd.data
      # Decoupage
      [ $run = 1 ] && [ $mpi != 1 ] && make_PAR.data $jdd $mpi 1>/dev/null 2>&1
      [ $mpi != 1 ] && jdd=PAR_$jdd
      # Modification du jeu de donnees (raffinement parallele) pour weak scaling:
      if [ $run = 1 ]
      then
         DOM="DOM"
         N=1
         NMAX=`echo $mpi/$smallest_mpi | bc`
         while [ $N -lt $NMAX ] 
         do
            N=`echo 8*$N | bc`
            NEW_DOM="DOMx$N"
            sed -i "1,$ s?Scatter $DOM.Zones dom?Raffiner_isotrope_parallele { name_of_initial_zones $DOM name_of_new_zones $NEW_DOM }\nScatter $NEW_DOM.Zones dom?g" $jdd.data
            DOM=$NEW_DOM
         done
      fi   
      # Calcul
      if [ $run = 1 ]
      then
         trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1
	 err=$?
	 # On desactive GPU_DIRECT si active et on relance...
	 if [ $err != 0 ] && [ "$TRUST_USE_MPI_GPU_AWARE" != "" ]
	 then
	    echo "Error with GPU Direct. See $jdd.out_err_GPU_DIRECT. Trying without..."
	    mv -f $jdd.out_err $jdd.out_err_GPU_DIRECT
	    unset TRUST_USE_MPI_GPU_AWARE
	    trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1
	    err=$?
	 fi
	 [ $err != 0 ] && rm -f *.TU && echo "Error:See "`pwd`/$jdd.out_err
      fi	 
      # Analyse
      [ $run = 1 ] && awk '/RAM taken/ {if ($1>RAM) RAM=$1} END {print "RAM= "RAM}' $jdd.out_err >> $jdd.TU
      # Load balancing
      load_imbalance=`awk '/Load imbalance/ {print $NF}' $jdd.out_err | tail -1`
      dof=`awk '/Total number of elements/ {print $NF}' $jdd.out_err | tail -1`
      dram=`awk '/allocated on a GPU/ {print $1}' $jdd.out_err | tail -1`
      its=`awk '/Iterations/ && /solveur/ {print $NF}' $jdd.TU`
      gpu="\t" && [ $bench = gpu ] && gpu="+"$mpi"GPU"
      direct="Off" && [ "`grep 'Enabling GPU' $jdd.out_err`" != "" ] && direct="On"
      kj=`grep -l $jdd myjob.* 2>/dev/null | tail -1 | awk -F. '{print $2}' | xargs -I {} sacct --format=JobID,ElapsedRaw,ConsumedEnergyRaw,NodeList --jobs={} 2>/dev/null | awk '/\.batch/ {print $3}'`
      awk -v host=$HOST -v mpi=$mpi"MPI" -v gpu=$gpu -v dof=$dof -v lib=$load_imbalance -v its=$its -v direct=$direct -v kj=$kj -v dram=$dram '/Secondes/ && /pas de temps/ {dt=$NF} /Dont solveurs/ {s=$4;b=dt-s} END {print host" \t"dof" \t"mpi""gpu"\t"dt" \t"s" \t"b" \t"int(dof/dt*0.001*0.001)" \t"int(its)" \t"lib" \t"kj" \t\t"direct" \t\t"dram" \t\t"1000*s/its}' $jdd.TU
      cd - 1>/dev/null 2>&1
   done    
done
