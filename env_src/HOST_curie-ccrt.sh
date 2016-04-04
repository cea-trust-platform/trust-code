#!/bin/bash
###########################################
#   _______  _______(_)__                 #
#  / ___/ / / / ___/ / _ \                #
# / /__/ /_/ / /  / /  __/                #
# \___/\__,_/_/  /_/\___/                 #
#                                         #
#   Hotline: hotline.tgcc@cea.fr          #
#            +33 17757 4242               #
#                                         #
#      Help: $ curie.info                 #
#                                         #
#  Web site: https://www-tgcc.ccc.cea.fr/ #
#                                         #
###########################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   # Create ROMIO_HINTS file for MPI IO optimizations on Lustre file system
   echo "# ROMIO HINTS
# Select the max OST available:
striping_factor -1
# Collective comm between nodes before write:
romio_cb_write enable
# Collective comm between nodes before read:
romio_cb_read  enable
# One process on each node do the coll comm task:
cb_config_list *:1" > ROMIO_HINTS.env
   echo "export ROMIO_HINTS=\$TRUST_ROOT/env/ROMIO_HINTS.env # ROMIO HINTS" >> $env
   #
   # qstat inexistente sur les dernieres machines du CCRT/TGCC
   echo "Command qstat created on $HOST"
   cp $TRUST_ROOT/bin/KSH/qstat_wrapper $TRUST_ROOT/bin/KSH/qstat
   modulecmd=`ls /opt/Modules/bin/modulecmd.tcl /usr/bin/modulecmd.tcl 2>/dev/null`
   echo "# For $HOST cluster:
module () { 
   eval \`tclsh $modulecmd sh \$*\` 
}" >> $env
  
   # New restore/save collection of modules (faster to load) 
   # Try to generalize to other clusters...
   collection_modules=TRUST_$TRUST_VERSION-$HOST
   module savelist
   echo "Trying to restore collection of modules named $collection_modules:"
   module restore $collection_modules
   if [ $? != 0 ]
   then
      echo "So we create the collection of modules $collection_modules:"
      # Changing strategy, we load a version for each module (not the last one)
      # MPI module 1.1.16.6 1.2.8.2 1.2.8.4(default)
      module="bullxmpi/1.2.8.4"
      echo "Module $module detected and loaded on $HOST."
      module unload bullxmpi
      module load $module
      # Intel compiler (WARNING: Intel 12.x provoque plusieurs problemes (voir FA1445))
	  # Intel 13.1.3.192 14.0.3.174(default) 15.0.2.164 15.0.3.187(decommissioned)
          # 15.0.5.223 16.0.0.109 16.0.1.150
      module="intel/14.0.3.174"
      echo "Module $module detected and loaded on $HOST."
      module unload intel
      module load $module
      #
      # If libccc_user module found, load it (this module helps to know the CPU)
      module=libccc_user
      if [ "`tclsh $modulecmd sh show $module 2>&1`" != "" ]
      then
         echo "Module $module detected and loaded on $HOST."
         module load $module
      fi
      # Save the collection:
      module save $collection_modules
   fi
   # Add the restore module command:
   echo "[ ! -f ~/.module/$collection_modules ] && ln -sf $HOME/.module/$collection_modules ~/.module/$collection_modules" >> $env
   echo "module restore $collection_modules 1>/dev/null" >> $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=2
   #mpirun="mpirun -n $NB_PROCS" ne marche pas au dela de 32 procs ?
   mpirun="ccc_mprun -n $NB_PROCS"
   [ "`basename $MPI_ROOT`" = mpich ] && mpirun="$Mpirun -np $NB_PROCS" # Try support of MVAPICH...
   # On selectionne la queue la moins occupee
   # On attend de voir, certains plantages sur standard
   #queue=`ccc_mpinfo | awk '/up/ {if ($10>free) {free=$10;queue=$1}} END {print queue}'`
   # On force la queue standard (processeurs@2.27Ghz au lieu de 1.00Ghz sur la xlarge)
   # ccc_mpinfo: standard xlarge hybrid
   # ccc_mqinfo: long normal test
   queue=standard
   if [ "$prod" = 1 ]
   then
      qos=normal
      cpu=86400 # 24 heures
   elif [ "$bigmem" = 1 ]
   then
      soumission=1
      qos=normal
      cpu=86400 
      # -M ram with ram>4000 is not supported anymore (06/2014)
      # ram=64000 # 64 GB asked
      # So we use now n cores for one task to have 4*n GB per task
      core_per_task=16 # To have 64GB per task
   else
      # Priorite superieure avec test si pas plus de 8 nodes (2*8 cores)
      qos=test && [ "$NB_PROCS" -gt 128 ] && qos=normal
      cpu=1800 # 30mn
   fi
   sub=CCC 
}
