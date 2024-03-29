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
   #module="craype-x86-rome craype-network-ofi PrgEnv-cray/8.1.0 rocm/rocm craype-accel-amd-gfx908 libfabric"
   module="nvhpc/21.7"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   #echo "PATH=\$CRAY_MPICH_PREFIX/bin:\$PATH"  >> $env # Pour trouver mpicxx
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
   project=""
   queue=CINES
   #qos=qos_cpu-t3 && cpu=1200 && [ "$prod" != 1 ] && qos=qos_cpu-dev && cpu=120 
   cpu=6000
   [ "`id | grep fej`" != "" ] && project="fej@cpu"
   ntasks=48 # number of cores max
   node=1 # --exclusive
   # --threads-per-core=1 important pour les perfs
   mpirun="srun -n \$SLURM_NTASKS --threads-per-core=1"
   sub=SLURM
}

