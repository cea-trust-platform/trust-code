#!/bin/bash
##################################################################################
#                            BIENVENUE SUR ORCUS !                            #
##################################################################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # Initialisation de l environnement module $MODULE_PATH
   echo "source /etc/profile.d/modules.sh " >> $env
   # Load modules
   gnu=1 # On continue a garder intel/intelmpi meme si aocc/hpcx plus scalable (voir avec AG)
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      # Il est critique de mettre nvidia_hpc_sdk en premier car sinon undefined reference __pgi car dans LD_LIBRARY_PATH le lib de nvidia avant celui de gcc
      module="slurm nvidia_hpc_sdk/nvhpc-nompi/22.1 compilers/gcc/9.1.0 mpi/openmpi/gcc/9.1.0/3.1.4 texlive/2020" # Cuda 11.5 mais plante cuSolver (on teste sur altair)
      module="slurm nvidia_hpc_sdk/21.2             compilers/gcc/9.1.0 mpi/openmpi/gcc/9.1.0/3.1.4 texlive/2020" # Cuda 11.2
      #module=$module" cmake/3.22.0"
   elif [ $gnu = 1 ]
   then
      # Compilateur Intel + MPI IntelMPI
      module="slurm compilers/intel/2019_update3 mpi/openmpi/intel/2019_update3/4.0.1" # Marche pas attention (crashes TRUST)
      module="slurm compilers/intel/2019_update3 mpi/intelmpi/2019_update3 texlive/2020" # Recommande par AG
      #source="source mpivars.sh release -ofi_internal" # TRES IMPORTANT pour intelmpi car sinon plantage sur plusieurs noeuds avec MLX5_SINGLE_THREAD
      # La version Intel sur orcus provoque des ennuis divers (performance/plantages) avec MUMPS. Ne pas utiliser! 
      # A priori, on utilise encore Intel au CCRT (mais surement des versions plus a jour, moins buggees), mais la tendance est de petit à petit ne plus utiliser sauf exception
      module="slurm compilers/gcc/9.1.0 mpi/openmpi/gcc/9.1.0/3.1.4 texlive/2020" # cf bt#195561
      module="slurm gcc/11.2.0 openmpi/gcc_11.2.0/4.1.4 texlive/2020" # passage a COS7.9, mpi/openmpi/gcc/9.1.0/3.1.4 plus supporte
   else
      # Compilateur : AOCC (AMD) et librairie MPI : HPC-X (Mellanox)
      module="slurm aocl/aocc/2.1 compilers/aocc/2.1.0 mpi/hpcx/aocc/2.1.0/2.6.0 texlive/2020"
      echo "module purge 1>/dev/null" >> $env
      echo "module load $module 1>/dev/null" >> $env
      # echo ". /scratch2/rnrna/aocc/setenv_AOCC.sh" >> $env
      echo "export TRUST_FORCE_CC=clang++; " >> $env
      echo "export TRUST_FORCE_cc=clang; " >> $env
      echo "export TRUST_FORCE_F77=flang; " >> $env
      # echo "export UCX_NET_DEVICES=mlx5_0:1; " >> $env #pour eviter de prendre la mauvaise carte IB sur n0[37-48]
   fi
   echo "# Module $module detected and loaded on $HOST."   
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   echo $source >> $env
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
   # sinfo :
   # PARTITION AVAIL  TIMELIMIT  NODES  STATE NODELIST
   # amdq*         up   infinite     34  alloc n[002-035]
   # intelq*      up   infinite      4  alloc n[101-104]
   # intelq*      up   infinite     11   idle n[105-115]
   # gpuq         up   infinite      1   idle gpu01

   # On se base sur la frontale pour selectionner la queue par defaut:
   queue=amdq_naples && [ "$gpu" = 1 ] && queue=gpuq_5118 && [ "`sinfo | grep $queue | grep idle`" = "" ] && queue=gpuq_5218

   # sacctmgr list qos
   # qos	prority		walltime	ntasks_max
   # normal 	20 		2 jours
   # test   	40		1 heure		40
   # long 	10		14 jours	200
   # visu	20		18 heures
   if [ "$prod" = 1 ] || [ "$NB_PROCS" -gt 40 ]
   then
      qos=normal && cpu=2880 && node=1 # exclusive
   else
      qos=test	 && cpu=60   && node=0 
   fi
   # Le binding ameliore fortement les performances sur AMD quelque soit MPI:
   if [ "$I_MPI_ROOT" != "" ] # IntelMPI
   then
      binding="-m block:block --cpu-bind=rank"
      mpirun="srun $binding -n \$SLURM_NTASKS"
   elif  [ "$HPCX_DIR" != "" ] # HPC-X
   then 
      binding="--map-by numa --bind-to core"
      mpirun="mpirun $binding -n \$SLURM_NTASKS"
   fi   
   sub=SLURM
}
