#!/bin/bash
#############################################################
#                                                           #
#    $$\      $$\$$$$$$$$\$$$$$$$$\$$$$$$$$\$$\             #
#    $$$\    $$$ $$  _____\____$$  $$  _____$$ |            #
#    $$$$\  $$$$ $$ |         $$  /$$ |     $$ |            #
#    $$\$$\$$ $$ $$$$$\      $$  / $$$$$\   $$ |            #
#    $$ \$$$  $$ $$  __|    $$  /  $$  __|  $$ |            #
#    $$ |\$  /$$ $$ |      $$  /   $$ |     $$ |            #
#    $$ | \_/ $$ $$$$$$$$\$$$$$$$$\$$$$$$$$\$$$$$$$$\       #
#    \__|     \__\________\________\________\________|      #
#                                                           #
#############################################################
#                        Bienvenue sur MEZEL                #
#############################################################
#                                                           #
# Informations disponibles sur                              #
# https://mezel-i01/                                        #
#                                                           #
#############################################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   module="gnu9/9.4.0 openmpi4/4.1.1"
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
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
   # sacctmgr list qos format=Name,Priority,MaxSubmit,MaxWall,MaxNodes
   #      Name   Priority MaxSubmit     MaxWall MaxNodes 
   #---------- ---------- --------- ----------- -------- 
   #    normal          0
   #    qos_max_1+      0
   qos=normal
   cpu=30 && [ "$prod" = 1 ] && cpu=1440 # 30 minutes or 1 day
   ntasks=12 # 12 cores per node for prod_amd_12c queue (48 for prod_amd_48c and 20 for prod_intel)
   # sinfo :
   #PARTITION   AVAIL  TIMELIMIT    NODES  STATE
   # longq        up   30-00:00:00
   # mediumq      up    7-00:00:00
   # shortq*      up    1-00:00:00
   # xshortq      up    1:00:00
   # rjhq         up   10-00:00:00
   # full         up    3-00:00:00
   queue=xshortq && [ "$prod" = 1 ] && queue=mediumq
   node=0
   #if [ "$prod" = 1 ] || [ $NB_PROCS -gt $ntasks ]
   #then
   #   node=1
   #   if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
   #   then
   #      echo "=================================================================================================================="
   #      echo "Warning: try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks on $queue partition."
   #      echo "=================================================================================================================="
   #   fi
   #else
   #   node=0
   #fi
   mpirun="mpirun -np \$SLURM_NTASKS"
   sub=SLURM
}
