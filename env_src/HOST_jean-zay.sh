#!/bin/bash
#######################################################################
# Jean-Zay2                                                                    #
#######################################################################


##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # Initialisation de l environnement module $MODULE_PATH
   echo "source /etc/profile" >> $env
   #
   # Load modules
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      if [ "$TRUST_USE_OPENMP" = 1 ]
      then
         #module="gcc/8.3.1 cuda/11.2 nvidia-compilers/22.5 openmpi/4.0.5-cuda" # Les modules sont mieux configures sur IDRIS qu'au CCRT...
         #module="gcc/8.3.1 cuda/11.2 nvidia-compilers/22.5 openmpi/4.1.5" # OK car crash au demarrage pour precedent (4.0.5-cuda -> NVHPC)
         #module="gcc/8.3.1 cuda/11.2 nvidia-compilers/23.5 openmpi/4.1.5" # Passage a 23.5 pour etre plus pres du PC de dev
         #module="gcc/8.3.1 cuda/11.2 nvidia-compilers/23.1 openmpi/4.1.5" # 23.5 a disparu...
         module="nvidia-compilers/23.1 openmpi/4.1.5-cuda" # On utilise desormais le CUDA du NVHPC et MPI GPU-Aware
         module="nvidia-compilers/23.1 openmpi/4.1.5" # MPI GPU-Aware fait diverger plusieurs preconditionneurs PETSc 3.20 dont boomeramg le plus performant... Support contacte
         CUDA_VERSION=12.0
      else
         echo "Not supported any more." && exit -1
      fi
      [ "$TRUST_CUDA_CC" = "" ] && TRUST_CUDA_CC=70 # V100
   else
      # avec intel/intelmpi 19.0.2, les calculs bloquent
      #module="intel-compilers/19.0.2 intel-mpi/19.0.2 intel-mkl/19.0.2"
      # avec intel/intelmpi 19.0.5, impossible de compiler TrioCFD ou autre BALTIK (meme vide)
      #module="intel-compilers/19.0.5 intel-mkl/19.0.5 intel-mpi/19.0.5"
      # avec intel 19.0.5 et openmpi 3.1.4, trust et TrioCFD compilent
      #module="intel-compilers/19.0.5 intel-mkl/19.0.5 openmpi/3.1.4"
      # 21/06/2023 : ND : passage a gcc
      module="gcc/8.5.0 openmpi/4.0.5"
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
   [ "$CUDA_VERSION" != "" ] && echo "export NVHPC_CUDA_HOME=\$NVHPC_ROOT/cuda/$CUDA_VERSION" >> $env # Prendre desormais le CUDA de NVHPC
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
      queue=gpu_p13 && constraint=v100-16g # 351x4=1404 GPU
      queue=gpu_p13 && constraint=v100-32g # 261x4=1044 GPU (Pour etre plus confort avec c-amg ?)
      #queue=gpu_p4 # Partition A100      
      [ "$gpus_per_node" = "" ] && gpus_per_node=4 # Si on ne reserve qu'1 GPU plantage memoire possible... Donc le max par defaut
      if [ "$TRUST_USE_OPENMP" = 1 ]
      then
         cpus_per_task=10 # 1MPI<->1GPU
      fi
      # See http://www.idris.fr/jean-zay/gpu/jean-zay-gpu-exec_partition_slurm.html#les_qos_disponibles
      qos=qos_gpu-t3 && cpu=1200 && [ "$prod" != 1 ] && [ $NB_PROCS -le 32 ] && qos=qos_gpu-dev && cpu=120 
      #qos=qos_gpu-t4 && cpu=6000
      [ "`id | grep aih`" != "" ] && project="aih@v100" # GENDEN
   else
      queue=cpu_p1
      qos=qos_cpu-t3 && cpu=1200 && [ "$prod" != 1 ] && qos=qos_cpu-dev && cpu=120 
      #qos=qos_cpu-t4 && cpu=6000
      [ "`id | grep aih`" != "" ] && project="aih@cpu" # GENDEN
   fi
   hintnomultithread=1
   ntasks=40 # number of cores max
   node=1 # --exclusive
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}

