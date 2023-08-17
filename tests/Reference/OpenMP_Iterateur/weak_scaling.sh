#!/bin/bash
# Weak scaling on nodes:
# Selon machines:
benchs="cpu gpu"              && mpis_gpu="2 16"          && mpis_cpu="8 64"              && int=""       && jdd_gpu=AmgX             && jdd_cpu=PETSc
[ "$HOST" = adastra ]         && mpis_gpu="2 16 128 1024" && mpis_cpu="64 512 4096 32768" && int="_int64" && jdd_gpu=BENCH_rocALUTION && jdd_cpu=BENCH_PETSc
[ "$HOST" = topaze ]          && mpis_gpu="2 16 128 1024" && mpis_cpu="64 512 4096 32768" && int=""       && jdd_gpu=BENCH_AmgX       && jdd_cpu=BENCH_PETSc
[ "$HOST" = irene-amd-ccrt ]  && benchs="cpu"             && mpis_cpu="64 512 4096 32768" && int="_int64"                             && jdd_cpu=BENCH_PETSc
env_gpu=$local/trust/amgx_openmp$int/env_TRUST.sh
env_cpu=$local/trust/tma$int/env_TRUST.sh         
jdd_cpu=OpenMP_Iterateur_$jdd_cpu.data
jdd_gpu=OpenMP_Iterateur_$jdd_gpu.data

mkdir -p weak_scaling && cd weak_scaling
echo -e "Host     \tDOF \tConfig     \tTime/dt[s]   \tWith Ax=B[s] \tWith B[s]    \tMDOF/s  \tIters    \tLoadImbalance"
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
      [ $bench = gpu ] && cp ../../$jdd_gpu $jdd.data
      [ $bench = cpu ] && cp ../../$jdd_cpu $jdd.data
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
      [ $run = 1 ] && (trust $jdd $mpi -journal=0 1>$jdd.out_err 2>&1 || (rm -f *.TU;echo "Error:See "`pwd`/$jdd.out_err))
      # Analyse
      [ $run = 1 ] && awk '/RAM taken/ {if ($1>RAM) RAM=$1} END {print "RAM= "RAM}' $jdd.out_err >> $jdd.TU
      # Load balancing
      load_imbalance=`awk '/Load imbalance/ {print $NF}' $jdd.out_err | tail -1`
      dof=`awk '/Total number of elements/ {print $NF}' $jdd.out_err | tail -1`
      its=`awk '/Iterations/ && /solveur/ {print $NF}' $jdd.TU`
      gpu="\t" && [ $bench = gpu ] && gpu="+"$mpi"GPU"
      awk -v host=$HOST -v mpi=$mpi"MPI" -v gpu=$gpu -v dof=$dof -v lib=$load_imbalance -v its=$its '/Secondes/ && /pas de temps/ {dt=$NF} /Dont solveurs/ {s=$4;b=dt-s} END {print host" \t"dof" \t"mpi""gpu" \t"dt" \t"s" \t"b" \t"dof/dt*0.001*0.001" \t"its" \t"lib}' $jdd.TU
      cd - 1>/dev/null 2>&1
   done    
done
