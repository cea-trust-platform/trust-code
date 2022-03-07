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
      module="gcc/8.3.1 openmpi/4.0.5-cuda cuda/11.2" # Fonctionne sur gpu_p13 et gpu_p4
      #module=$module" opa-psm2/11.2.204nccl_patched-cuda" # Patch pour corriger un plantage lors des IO (sondes/xyz)
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
   [ "$gpu"  = 1 ] && soumission=1
   # http://www.idris.fr/eng/jean-zay/gpu/jean-zay-gpu-exec_partition_slurm-eng.html Une seule partition gpu_p13
   project=""
   if [ "$gpu" = 1 ]
   then
      # See http://www.idris.fr/jean-zay/gpu/jean-zay-gpu-exec_partition_slurm.html 
      queue=gpu_p13 && constraint=v100-16g # pour gpu_p3 ou constraint=v100-32g (gpu_p1)  
      #queue=gpu_p4 # Partition A100      
      [ "$gpus_per_node" = "" ] && gpus_per_node=4 # Si on ne reserve qu'1 GPU plantage memoire possible... Donc le max par defaut
      qos=qos_gpu-t3 && cpu=1200 && [ "$prod" != 1 ] && qos=qos_gpu-dev && cpu=120 
      #qos=qos_gpu-t4 && cpu=6000
      [ "`id | grep aih`" != "" ] && project="aih@gpu" # GENDEN
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

