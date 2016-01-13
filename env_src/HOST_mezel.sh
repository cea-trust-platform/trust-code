#!/bin/bash
#########################################################################
#                                                                       #
#    $$\      $$\$$$$$$$$\$$$$$$$$\$$$$$$$$\$$\              $$$$$$\    #
#    $$$\    $$$ $$  _____\____$$  $$  _____$$ |            $$  __$$\   #
#    $$$$\  $$$$ $$ |         $$  /$$ |     $$ |            \__/  $$ |  #
#    $$\$$\$$ $$ $$$$$\      $$  / $$$$$\   $$ |             $$$$$$  |  #
#    $$ \$$$  $$ $$  __|    $$  /  $$  __|  $$ |            $$  ____/   #
#    $$ |\$  /$$ $$ |      $$  /   $$ |     $$ |            $$ |        #
#    $$ | \_/ $$ $$$$$$$$\$$$$$$$$\$$$$$$$$\$$$$$$$$\       $$$$$$$$\   #
#    \__|     \__\________\________\________\________|      \________|  #
#                                                                       #
#########################################################################
#                        Bienvenue sur MEZEL2                           #
#########################################################################
#                                                                       #
# Informations disponibles sur                                          #
# https://www-linuxcad.intra.cea.fr/dokuwiki/doku.php/cluster           #
# et                                                                    #
# https://mezel-ms0/doku.php                                            #
#                                                                       #
#########################################################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   # Load modules
   # intel 
   #intel=""
   # bullxmpi
   #module="$intel "
   #
   #echo "# Module $module detected and loaded on $HOST."
   #echo "module purge 1>/dev/null" >> $env
   #echo "module load $module 1>/dev/null" >> $env     
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
   #    normal          0            7-00:00:00          
   #      long          0           30-00:00:00 
   cpu=2880 && qos=normal       # Qos normal  2 days (?? nodes max=??? CPUs)    Priority ??
   # sinfo :
   #PARTITION    AVAIL  TIMELIMIT  NODES  STATE NODELIST
   #prod*           up   infinite     30  down* mezel[029-033,036,038-041,101-103,106-115],mezelocto100,mezelquad[100-105]
   #amd4184-12c     up   infinite      1  alloc mezel100
   #intelE5-2697v3- up   infinite      2   idle mezel[034-035]
   queue=prod
   # Slurm srun support
   #mpirun="srun -n \$SLURM_NTASKS"
   mpirun="mpirun -np \$SLURM_NTASKS"
   sub=SLURM
}
