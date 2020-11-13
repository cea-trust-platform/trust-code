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
      cuda_version=10.2
      module="gcc/8.3.1 openmpi/4.0.2-cuda cuda/$cuda_version"
   else
      # avec intel/intelmpi 19.0.2, les calculs bloquent
      #module="intel-compilers/19.0.2 intel-mpi/19.0.2 intel-mkl/19.0.2"
      # avec intel/intelmpi 19.0.5, impossible de compiler TrioCFD ou autre BALTIK (meme vide)
      #module="intel-compilers/19.0.5 intel-mkl/19.0.5 intel-mpi/19.0.5"
      # avec intel 19.0.5 et openmpi 3.1.4, trust et TrioCFD compilent
      module="intel-compilers/19.0.5 intel-mkl/19.0.5 openmpi/3.1.4"
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
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
   [ "$gpu"  = 1 ] && soumission=1 && qos=qos_gpu-dev
   # sinfo :
   #PARTITION AVAIL  TIMELIMIT  NODES  STATE 
   #cpu_p1*      up   20:00:00    1528 mix
   #gpu_p1       up   20:00:00     261 mix
   #visu         up    1:00:00      5   idle 
   #archive      up   20:00:00      7    mix 
   #prepost      up   20:00:00      4    mix 


   queue=cpu_p1 && [ "$gpu" = 1 ] && queue=gpu_p3 && [ "`sinfo | grep $queue | grep idle`" = "" ] && queue=gpu_p1
   # sacctmgr list qos format=Name,Priority,MaxSubmit,MaxWall,MaxNodes :      
   #   Name   Priority MaxSubmit     MaxWall MaxNodes 
   #---------- ---------- --------- ----------- -------- 
   #normal          0                                
   #qos_cpu-d+         80     10000    02:00:00          
   #qos_gpu-d+         80     10000    02:00:00          
   #qos_cpu-gc         50     25000    20:00:00          
   #qos_gpu-gc         50     25000    20:00:00          
   #qos_cpu-t3         50     10000    20:00:00          
   #qos_gpu-t3         50     10000    20:00:00          
   #qos_cpu-ex          0     10000    20:00:00          
   #qos_gpu-ex          0     10000    20:00:00          
   #qos_cpu-d+         50     10000                      
   #qos_gpu-d+         50     10000                      
   #qos_cpu-cp         50     10000                      
   #qos_gpu-cp         50     10000                      
   #qos_prepo+          0     10000                  
   project=""
   if [ "$gpu" = 1 ]
      [ "`id | grep ikp`" != "" ] && project="ikp@gpu" && gpus_per_node=4 # Si on ne reserve qu'1 GPU plantage memoire possible...
   else
      [ "`id | grep fej`" != "" ] && project="fej@cpu"
      cpus_per_task=40
   fi
   hintnomultithread=1
   cpu=30 && [ "$prod" = 1 ] && cpu=110 # 30 minutes or 1 day
   ntasks=20 # number of cores ?
   node=1 # --exclusive
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}

