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
   module="slurm compilers/intel/2019_update3 mpi/openmpi/intel/2019_update3/4.0.1" # OpenMPI plante TRUST de façon bizarre sur cette machine...
   module="slurm compilers/intel/2019_update3 mpi/intelmpi/2019_update3"
   echo "# Module $module detected and loaded on $HOST." 
   echo "module purge 1>/dev/null" >> $env   
   echo "module load $module 1>/dev/null" >> $env
   echo "source mpivars.sh release -ofi_internal" >> $env # TRES IMPORTANT pour intelmpi car sinon plantage sur plusieurs noeuds avec MLX5_SINGLE_THREAD
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
   soumission=2 && [ "$prod" = 1 ] && soumission=1
   # sinfo :
   # PARTITION AVAIL  TIMELIMIT  NODES  STATE NODELIST
   # amdq         up   infinite     34  alloc n[002-035]
   # amdq         up   infinite      2   idle n[001,036]
   # intelq*      up   infinite      4  alloc n[101-104]
   # intelq*      up   infinite     11   idle n[105-115]
   # gpuq         up   infinite      1   idle gpu01   
   
   # On se base sur la frontale pour selectionner la queue par defaut:  
   queue=intelq_6226 # Plus rapide en sequentiel (frequence+, cache+) mais scale moins bien (bizarre)
   queue=intelq_5118

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
   binding="-m block:block --cpu-bind=rank" # Ameliore un peu les performances sur INTEL
   mpirun="srun $binding -n \$SLURM_NTASKS"
   sub=SLURM
}
