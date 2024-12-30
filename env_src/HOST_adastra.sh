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
   if [ "$TRUST_USE_ROCM" = 1 ]
   then
      # Compilateur crayCC
      module="craype-x86-trento craype-network-ofi PrgEnv-cray/8.5.0 rocm/5.7.1 craype-accel-amd-gfx90a libfabric gcc-mixed/11.2.0"
      #module=$module" cce/15.0.1" # 16.0.0 has an issue with crayftn (ToDo contact support)
      # Compilateur amdclang
      #module="craype-x86-trento craype-network-ofi PrgEnv-amd/8.5.0 rocm/5.7.1 craype-accel-amd-gfx90a libfabric gcc-mixed/11.2.0"
      # Compilateur hipcc
      module="PrgEnv-gnu/8.5.0"
      
      # ROCM_ARCH par defaut:
      if [ "$ROCM_ARCH" = "" ]
      then
         export ROCM_ARCH=gfx90a # MI250
      elif [ "$ROCM_ARCH" != gfx942 ] # MI300
      then
         echo "$ROCM_ARCH not supported on adastra!"
      fi
      module=$module" craype-accel-amd-$ROCM_ARCH"
   else
      # Compilateur GNU
      #module="craype-x86-trento craype-network-ofi PrgEnv-cray libfabric gcc/10.3.0" used for first 1.9.3 install
      # use swig module, but it needs develop GCC-CPU-3.1.0, which load gcc-native/12.1
      # gcc-native/12.1 is replaced by cce/17.0.0 when loading PrgEnv-cray, so we reload gcc-native/12.1 again
      #module="develop GCC-CPU-3.1.0 swig/4.1.1-fortran craype-x86-trento craype-network-ofi PrgEnv-cray libfabric gcc-native/12.1 cray-python/3.10.10 cmake/3.27.7"
      # ND : 30/04/2024 : ne plus utiliser develop car les modules sont assez souvent supprimes
      module="craype-x86-trento craype-network-ofi PrgEnv-cray libfabric gcc/10.3.0"
   fi
   module=$module" python/3.12.1 swig" # Pour -without-conda
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
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
   [ "`id | grep genden15`" != "" ] && project="genden15"
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
   else
      # Partition scalaire
      constraint=GENOA
      ntasks=192 # Node cores
      noeuds=`echo "1+($NB_PROCS-1)/$ntasks" | bc`
      srun_options=""
      #[ $NB_PROCS -gt ??? ] && qos=???
   fi
   node=1 # --exclusive
   ram=0 # Important pour acceder a toute la RAM du noeud
   # ToDo utiliser le binding !!!
   # https://dci.dci-gitlab.cines.fr/webextranet/porting_optimization/#proper-binding-why-and-how
   # https://dci.dci-gitlab.cines.fr/webextranet/porting_optimization/detailed_binding_script.html#adastra-detailed-binding-script
   # Attention, le verbose est important sinon crash ! voir doc
   mpirun="srun -l $srun_options --mem-bind=local --mpi=cray_shasta --cpu-bind=verbose,cores"
   sub=SLURM
}

