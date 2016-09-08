#!/bin/bash
#######################################################
#    _____ ______ _____  ______  _____ ___            #
#   / ____|  ____|  __ \|  ____|/ ____|__ \           #
#  | |    | |__  | |__) | |__  | (___    ) |          #
#  | |    |  __| |  _  /|  __|  \___ \  / /           #
#  | |____| |____| | \ \| |____ ____) |/ /_           #
#   \_____|______|_|  \_\______|_____/|____|          #
#                                                     #
# Description stockage Utilisateur :                  #
# /home/USER : backup                                 #
# /home/USER/archive : backup                         #
# /home/USER/scratch : NO backup                      #
# /home/USER/stockage : NO backup                     #
#                                                     #
# Url acces Wiki : https://www-linuxmar.intra.cea.fr/ #
#                                                     #
#######################################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # Load modules
   #
   # GNU:
   # MPI module openmpi-1.8.3-gcc.env openmpi-1.8.5-gcc.env
   # echo "source /softs/_environnement/openmpi-1.8.5-gcc.env" >> $env
   #
   # INTEL: 2016.1.2
   #echo "source /softs/_environnement/intel.env" >> $env
   echo "source /softs/intel/bin/compilervars.sh intel64" >> $env
   # MPI module impi-5.0.3.048 impi-5.1.3.181
   #echo "source /softs/_environnement/intel_mpi.env" >> $env
   echo "source /softs/intel/impi/5.1.3.181/bin64/mpivars.sh" >> $env
   # MPI module openmpi-1.8.3-intel.env openmpi-1.8.5-intel.env
   # echo "source /softs/_environnement/openmpi-1.8.5-intel.env" >> $env
   #
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
   # sacctmgr list qos format=Name,Priority,MaxSubmit,MaxWall,MaxNodes :
   #      Name   Priority MaxSubmit     MaxWall MaxNodes
   #---------- ---------- --------- ----------- --------
   #    normal          0
   #      visu          0
   #      lila          0
   qos=normal
   cpu=30 && [ "$prod" = 1 ] && cpu=1440 # 30 minutes or 1 day
   ntasks=12 # 12 cores per node for prod_X5675 queue (16 for prod_E5-2670 and 20 for prod_E5-2680v2)
   # sinfo :
   #PARTITION      AVAIL  TIMELIMIT  NODES  STATE
   #prod*             up   infinite      7    mix
   #prod_X5675        up   infinite      1    mix
   #prod_E5-2680v2    up   infinite      3    mix
   #prod_E5-2670      up   infinite      3    mix
   #lila              up   infinite      2    mix
   #queue=prod
   queue=prod_X5675
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt $ntasks ]
   then
      if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
      then
         echo "=================================================================================================================="
         echo "Warning: try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks on $queue partition."
         echo "=================================================================================================================="
      fi
   fi
   #if [ "`basename $Mpirun`" = srun ]
   #then
   #   # Slurm srun support
   #   mpirun="srun -n $NB_PROCS" # A revoir car tres tres long !!!
   #else
   #   mpirun="mpirun -np $NB_PROCS"
   #fi
   mpirun="mpirun -np \$SLURM_NTASKS"
   sub=SLURM
}
