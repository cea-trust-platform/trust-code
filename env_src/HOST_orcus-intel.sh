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

   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      module="compilers/gcc/9.1.0 mpi/openmpi/gcc/9.1.0/3.1.4 nvidia_hpc_sdk/21.2 texlive/2020"
   else
      #module="slurm compilers/intel/2019_update3 mpi/openmpi/intel/2019_update3/4.0.1" # OpenMPI plante TRUST de facon bizarre sur cette machine...
      #module="slurm compilers/intel/2019_update3 mpi/intelmpi/2019_update3 texlive/2020"
      # La version Intel sur orcus provoque des ennuis divers (performance/plantages) avec MUMPS. Ne pas utiliser! 
      # A priori, on utilise encore Intel au CCRT (mais surement des versions plus a jour, moins buggees), mais la tendance est de petit à petit ne plus utiliser sauf exception. cf bt#195561
      #module="slurm compilers/gcc/9.1.0 mpi/openmpi/gcc/9.1.0/3.1.4 texlive/2020" # Calculs plus stables avec MUMPS (teste fin 2022). SegFault avec la precedente ligne parfois au bout d'un certain temps
      # 02/10/2023 : Ajout module qt/5.14 pour VisIt
      #module="slurm gcc/11.2.0 openmpi/gcc_11.2.0/4.1.4 texlive/2020 qt5/gcc_9.3.0/5.14.2" # passage a COS7.9, mpi/openmpi/gcc/9.1.0/3.1.4 plus supporte
      module="gcc/11.4.0 openmpi/gcc_11.4.0/4.1.6 texlive/20240312"
   fi
   echo "# Module $module detected and loaded on $HOST." 
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
   [ "`echo $module | grep intelmpi`" != "" ] && echo "source mpivars.sh release -ofi_internal" >> $env # TRES IMPORTANT pour intelmpi car sinon plantage sur plusieurs noeuds avec MLX5_SINGLE_THREAD
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
   queue=intelq_6226 # Plus rapide en sequentiel (frequence+, cache+) mais scale moins bien (bizarre)
   queue=intelq_5118
   [ "$gpu" = 1 ] && queue=gpuq_5118 && [ "`sinfo | grep $queue | grep idle`" = "" ] && queue=gpuq_5218

   # sacctmgr list qos
   # qos	prority		walltime	ntasks_max
   # normal 	20 		2 jours
   # test   	40		1 heure		40
   # long 	10		14 jours	200
   # visu	20		18 heures
   # 1jour      30              1 jour
   # 2jour      20              2 jours
   # 7jour      15              7 jours
   # 14jour     10              14 jours
   # dev        80              20 minutes
   # debug      70              4 heures
   if [ "$prod" = 1 ] || [ "$NB_PROCS" -gt 40 ]
   then
      qos=normal && cpu=2880 && node=1 # exclusive
   else
      qos=test	 && cpu=60   && node=1 # exclusive too
   fi
   binding="-m block:block --cpu-bind=rank" # Ameliore un peu les performances sur INTEL
   mpirun="srun --mpi=pmix $binding -n \$SLURM_NTASKS"
   sub=SLURM
}
