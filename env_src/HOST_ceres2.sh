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
   # INTEL:
   echo "source /softs/intel/bin/compilervars.sh intel64" >> $env
   # MPI module impi-5.0.3.048 openmpi-1.8.5-intel.env
   echo "source /softs/intel/impi/5.0.3.048/bin64/mpivars.sh" >> $env
   # echo "source /softs/_environnement/openmpi-1.8.5-intel.env" >> $env
   #
   . $env
   # Creation wrapper qstat -> squeue
   echo "#!/bin/bash
squeue" > $TRUST_ROOT/bin/qstat
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
   cpu=2880 && qos=normal       # Qos normal  2 days (?? nodes max=??? CPUs)    Priority ??
   # sinfo :
   #PARTITION    AVAIL  TIMELIMIT  NODES  STATE NODELIST
   #prod*           up   infinite     10   idle ceres[231-240]
   #prod_E5-2680    up   infinite     10   idle ceres[231-240]
   #gpu             up   infinite      1   idle ceres241
   #visu            up   infinite      1   idle ceres2-visu
   queue=prod
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
