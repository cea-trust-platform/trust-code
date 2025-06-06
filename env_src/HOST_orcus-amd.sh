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
      #module="nvidia_hpc_sdk/nvhpc-nompi/22.1 compilers/gcc/9.1.0 mpi/openmpi/gcc/9.1.0/3.1.4 texlive/2020"
      module="gcc/11.4.0 openmpi/gcc_11.4.0/4.1.6 texlive/20240312" # On telecharge desormais le meme SDK que sur PC
      module="gcc/11.4.0 texlive/20240312" # On utilise le OpenMPI du SDK telecharge car le OpenMPI orcus n'est pas GPU-Aware...
      echo "export USE_NVHPC_MPI=1" >> $env
      CUDA_VERSION=12.1 # Kokkos prend par defaut 12.1 et au link nvlink prend 11.0 donc on met tout au meme niveau...
      [ "$TRUST_CUDA_CC" = "" ] && TRUST_CUDA_CC=80 # A100
   elif [ $gnu = 1 ]
   then
      # Compilateur Intel + MPI IntelMPI
      #module="slurm compilers/intel/2019_update3 mpi/openmpi/intel/2019_update3/4.0.1" # Marche pas attention (crashes TRUST)
      #module="slurm compilers/intel/2019_update3 mpi/intelmpi/2019_update3 texlive/2020" # Recommande par AG
      #source="source mpivars.sh release -ofi_internal" # TRES IMPORTANT pour intelmpi car sinon plantage sur plusieurs noeuds avec MLX5_SINGLE_THREAD
      # La version Intel sur orcus provoque des ennuis divers (performance/plantages) avec MUMPS. Ne pas utiliser! 
      # A priori, on utilise encore Intel au CCRT (mais surement des versions plus a jour, moins buggees), mais la tendance est de petit à petit ne plus utiliser sauf exception
      #module="slurm compilers/gcc/9.1.0 mpi/openmpi/gcc/9.1.0/3.1.4 texlive/2020" # cf bt#195561
      # 02/10/2023 : Ajout module qt/5.14 pour VisIt
      #module="slurm gcc/11.2.0 openmpi/gcc_11.2.0/4.1.4 texlive/2020 qt5/gcc_9.3.0/5.14.2" # passage a COS7.9, mpi/openmpi/gcc/9.1.0/3.1.4 plus supporte
      #module="slurm gcc/11.4.0 openmpi/gcc_11.4.0/4.1.6 texlive/20240312"
      module="openmpi/gcc_13.3.0/4.1.6 texlive/20240312" # Mise a jour des modules Orcus-AMD en 08/2024
   else
      # Compilateur : AOCC (AMD) et librairie MPI : HPC-X (Mellanox)
      module="aocl/aocc/2.1 compilers/aocc/2.1.0 mpi/hpcx/aocc/2.1.0/2.6.0 texlive/2020"
      echo "module purge 1>/dev/null" >> $env
      echo "module load $module 1>/dev/null || exit -1" >> $env
      # echo ". /scratch2/rnrna/aocc/setenv_AOCC.sh" >> $env
      echo "export TRUST_FORCE_CC=clang++; " >> $env
      echo "export TRUST_FORCE_cc=clang; " >> $env
      echo "export TRUST_FORCE_F77=flang; " >> $env
      # echo "export UCX_NET_DEVICES=mlx5_0:1; " >> $env #pour eviter de prendre la mauvaise carte IB sur n0[37-48]
   fi
   echo "# Module $module detected and loaded on $HOST." 
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module" >> $env
   echo "[ \$? != 0 ] && echo \"Error: $module not found; we exit...\" && echo \"Contat TRUST support team or system administrator\" && exit -1" >> $env
   [ "$CUDA_VERSION" != "" ] && echo "export CUDA_VERSION=$CUDA_VERSION" >> $env # Prendre desormais le CUDA de NVHPC
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
   queue=amdq_naples && [ "`grep 'Rocky Linux 9.1' /etc/os-release 1>/dev/null 2>&1 ; echo $?`" = "0" ] && queue=amdq_milan
   if [ "$gpu" = 1 ]
   then
      [ "$TRUST_CUDA_CC" = 90 ] && queue=gpuq_h100 && gpus_per_node=`echo $NB_PROCS | awk '{print $1<4?$1:4}'` && noeuds=`echo "1+($NB_PROCS-1)/4" | bc` # 4GPUs/node
      [ "$TRUST_CUDA_CC" = 80 ] && queue=gpuq_a100 && gpus="" && noeuds=`echo "1+($NB_PROCS-1)/2" | bc` # 2GPUs/node
      [ "$TRUST_CUDA_CC" = 70 ] && queue=gpuq_v100 && gpus="" && noeuds=`echo "1+($NB_PROCS-1)/2" | bc` # 2GPUs/node
   fi
   # sacctmgr list qos
   # qos	prority		walltime	ntasks_max
   # test   	40		1 heure		40
   # visu	20		18 heures
   # 1jour      30              1 jour
   # 2jours      20              2 jours
   # 7jours      15              7 jours
   # 14jours     10              14 jours
   # dev        80              20 minutes
   # debug      70              4 heures
   if [ "$prod" = 1 ] || [ "$NB_PROCS" -gt 40 ]
   then
      qos=2jours && cpu=2880
      [ "$gpu" != 1 ] && node=1 # exclusif uniquement sur cpu
   else
      qos=test	&& cpu=60   && node=0 
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
   else
      mpirun="srun --mpi=pmix -n \$SLURM_NTASKS"
   fi
   sub=SLURM
}
