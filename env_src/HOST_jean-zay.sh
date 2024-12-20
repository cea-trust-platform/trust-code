#!/bin/bash
#######################################################################
# Jean-Zay2                                                           #
#######################################################################


##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # Initialisation de l environnement module $MODULE_PATH si pas disponible:
   module -v 2>/dev/null || echo $echo "source /etc/profile" >> $env
   #
   # Load modules
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      if [ "$TRUST_USE_OPENMP" = 1 ]
      then
         # On utilise desormais le CUDA de NVHPC:
         if [ "$TRUST_FORCE_MPI_GPU_AWARE" = 1 ]
         then
            # MPI GPU-Aware fait diverger plusieurs preconditionneurs PETSc 3.20 dont boomeramg le plus performant... Support contacte
            module="gcc/8.5.0 openmpi/4.1.5-cuda"
         else
            module="gcc/8.5.0 openmpi/4.1.5" # On telecharge le NVHPC 23.5 pour pouvoir compiler Kokkos 4.5 (23.1 trop vieux)
         fi
      else
         echo "Not supported any more." && exit -1
      fi
      [ "$TRUST_CUDA_CC" = "" ] && TRUST_CUDA_CC=70           # V100
      [ "$TRUST_CUDA_CC" = 80 ] && module="arch/a100 "$module # A100
      [ "$TRUST_CUDA_CC" = 90 ] && module="arch/h100 "$module # H100
   else
      # 21/06/2023 : ND : passage a gcc
      module="gcc/8.5.0 openmpi/4.0.5"
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
   echo "export TRUST_DISABLE_STRUMPACK=1" >> $env
   echo "export TRUST_DISABLE_SUPERLU_DIST=1" >> $env
   . $env
   # Creation wrapper qstat -> squeue
   echo "#!/bin/bash
squeue" > $TRUST_ROOT/bin/qstat
   chmod +x $TRUST_ROOT/bin/qstat
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=2
   [ "$prod" = 1 ] && soumission=1
   [ "$gpu"  = 1 ] && soumission=1
   # http://www.idris.fr/eng/jean-zay/gpu/jean-zay-gpu-exec_partition_slurm-eng.html Une seule partition gpu_p13
   project=""
   if [ "$gpu" = 1 ]
   then
      # See http://www.idris.fr/jean-zay/gpu/jean-zay-gpu-exec_partition_slurm.html 
      if [ "$TRUST_CUDA_CC" = 70 ]
      then
         ntasks=40 # number of cores max
         [ "$gpus_per_node" = "" ] && gpus_per_node=4
         cpus_per_task=10
         queue=gpu_p13 && constraint=v100-16g # 351x4=1404 GPU
         queue=gpu_p13 && constraint=v100-32g # 261x4=1044 GPU (Pour etre plus confort avec c-amg ?)
         arch="v100"
      elif [ "$TRUST_CUDA_CC" = 80 ]
      then
         ntasks=64 # number of cores max
         [ "$gpus_per_node" = "" ] && gpus_per_node=8
         cpus_per_task=8
         queue=gpu_p5 && constraint=a100
         arch="a100"
      elif [ "$TRUST_CUDA_CC" = 90 ]
      then
         ntasks=96 # number of cores max
         [ "$gpus_per_node" = "" ] && gpus_per_node=4
         cpus_per_task=24
         queue=gpu_p6 && constraint=h100
         arch="h100"
      fi
      # See http://www.idris.fr/jean-zay/gpu/jean-zay-gpu-exec_partition_slurm.html#les_qos_disponibles
      q="" && [ $arch != v100 ] && q="_"$arch
      qos=qos_gpu$q-t3 && cpu=1200 && [ "$prod" != 1 ] && [ $NB_PROCS -le 32 ] && qos=qos_gpu$q-dev && cpu=120 
      [ "`id | grep aih`" != "" ] && project="aih@$arch" # GENDEN
   else
      ntasks=40 # number of cores max
      queue=cpu_p1
      qos=qos_cpu-t3 && cpu=1200 && [ "$prod" != 1 ] && qos=qos_cpu-dev && cpu=120 
      #qos=qos_cpu-t4 && cpu=6000
      [ "`id | grep aih`" != "" ] && project="aih@cpu" # GENDEN
   fi
   hintnomultithread=1
   node=1 # --exclusive
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}

