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
   # Load modules
   module="slurm compilers/intel/2019_update3 mpi/openmpi/intel/2019_update3/4.0.1" # Utilise par PE ?
   module="slurm compilers/intel/2019_update3 mpi/intelmpi/2019_update3" # Recommande par AG
   echo "# Module $module detected and loaded on $HOST."    
   echo "module load $module 1>/dev/null" >> $env
   echo ". mpivars.sh release -ofi_internal" >> $env # Necessaire car par defaut IntelMPI multithreade: message ERROR: multithreading violation  
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
   queue=intelq && [ "`grep AMD /proc/cpuinfo`" != "" ] && queue=amdq

   # sacctmgr list qos
   # qos	prority		walltime	ntasks_max 
   # normal 	20 		2 jours		
   # test   	40		1 heure		40
   # long 	10		14 jours	200
   # visu	20		18 heures
   qos=test && [ "$prod" = 1 ] && qos=normal
   cpu=60 && [ "$prod" = 1 ] && cpu=2880
   node=0 # exclusive ?
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}
