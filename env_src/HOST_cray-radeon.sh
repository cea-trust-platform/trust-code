#!/bin/bash
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
      module="craype-x86-rome craype-network-ofi PrgEnv-cray/8.1.0"
   else
      module="craype-x86-rome craype-network-ofi PrgEnv-cray/8.1.0 rocm/rocm craype-accel-amd-gfx908"
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   echo "PATH=\$CRAY_MPICH_PREFIX/bin:\$PATH"  >> $env # Pour trouver mpicxx
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
   soumission=128 # Provisoire
   [ "$prod" = 1 ] && soumission=1
   [ "$gpu"  = 1 ] && soumission=1
   project=""
   if [ "$gpu" = 1 ]
   then
      exit -1 # ToDo
      # See http://www.idris.fr/jean-zay/gpu/jean-zay-gpu-exec_partition_slurm.html 
      queue=gpu_p13 && constraint=v100-16g # pour gpu_p3 ou constraint=v100-32g (gpu_p1)        
      gpus_per_node=4 # Si on ne reserve qu'1 GPU plantage memoire possible...
      qos=qos_gpu-t3 && cpu=1200 && [ "$prod" != 1 ] && qos=qos_gpu-dev && cpu=120 
      #qos=qos_gpu-t4 && cpu=6000
      [ "`id | grep eia`" != "" ] && project="eia@gpu"
   else
      queue=cpu_p1
      qos=qos_cpu-t3 && cpu=1200 && [ "$prod" != 1 ] && qos=qos_cpu-dev && cpu=120 
      #qos=qos_cpu-t4 && cpu=6000
      [ "`id | grep fej`" != "" ] && project="fej@cpu"
   fi
   ntasks=128 # number of cores max
   node=1 # --exclusive
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}

