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
   qos=normal
   cpu=30 && [ "$prod" = 1 ] && cpu=1440 # 30 minutes or 1 day
   # sinfo :
   #PARTITION    AVAIL  TIMELIMIT  NODES  STATE
   #prod*           up   infinite     31    mix
   #prod_intel      up   infinite      5    mix
   #prod_amd_12c    up   infinite     20    mix
   #prod_amd_48c    up   infinite      6    mix
   #mono            up   infinite      1    mix
   queue=prod
   # Slurm srun support
   #mpirun="srun -n \$SLURM_NTASKS"
   mpirun="mpirun -np \$SLURM_NTASKS"
   sub=SLURM
}
