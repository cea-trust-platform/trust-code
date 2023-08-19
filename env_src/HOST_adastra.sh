#!/bin/bash
##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # qstat inexistente sur les dernieres machines du CCRT/TGCC
   echo "Command qstat created on $HOST"
   cp $TRUST_ROOT/bin/KSH/qstat_wrapper $TRUST_ROOT/bin/KSH/qstat
   # Initialisation de l environnement module $MODULE_PATH
   echo "source /etc/profile" >> $env
   #
   # Load modules
   if [ "$TRUST_USE_ROCM" = 1 ] && [ "$TRUST_USE_OPENMP" = 1 ]
   then
      # Compilateur crayCC
      module="craype-x86-trento craype-network-ofi PrgEnv-cray rocm craype-accel-amd-gfx90a libfabric gcc-mixed/11.2.0"
   else
      # Compilateur GNU
      module="craype-x86-trento craype-network-ofi PrgEnv-cray libfabric gcc/10.3.0"   
   fi   
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   echo "PATH=\$CRAY_MPICH_PREFIX/bin:\$PATH"  >> $env # Pour trouver mpicxx
   echo "export TRUST_DEVICES_PER_NODE=8" >> $env # Devices per node
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   # Documentation adastra: https://dci.dci-gitlab.cines.fr/webextranet/
   soumission=1
   if [ "$prod" = 1 ]
   then
      qos="" && cpu=1440 # 1 day
   else
      qos="" && cpu=30 # 30 mn
   fi
   if [ "$gpu" = 1 ]
   then
      # Partition mi250 (4 cartes MI250X par noeud=8 cartes MI100):
      constraint=MI250
      ntasks=64 # Node cores
      cpus_per_task=8 # 1 GPU/MPI (OpenMP choix par default)
      noeuds=`echo "1+($NB_PROCS-1)/8" | bc`
      # Important pour les performances ! le -c dans le srun est important il semble que SBATCH -c ne marche pas...
      # Attention, le verbose est important sinon crash ! voir doc
      srun_options="-c $cpus_per_task --gpus-per-task=1 --ntasks-per-node=8 --threads-per-core=1 --gpu-bind=verbose,closest"
      #[ $NB_PROCS -gt 8 ] && qos=normal # 2 nodes
      [ "`id | grep cpa2202`" != "" ] && project="cpa2202" # Projet GPU (5000h)
   else
      # Partition scalaire
      constraint=GENOA
      ntasks=192 # Node cores
      noeuds=`echo "1+($NB_PROCS-1)/$ntasks" | bc`
      srun_options=""
      #[ $NB_PROCS -gt ??? ] && qos=???
      [ "`id | grep cin3364`" != "" ] && project="cin3364" # Projet GPU (5000h), CPU (100000h)
   fi
   node=1 # --exclusive
   ram=0 # Important pour acceder a toute la RAM du noeud
   # Attention, le verbose est important sinon crash ! voir doc
   mpirun="srun -l $srun_options --mem-bind=local --mpi=cray_shasta --cpu-bind=verbose,cores"
   sub=SLURM
}

