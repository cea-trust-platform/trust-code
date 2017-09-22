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
   modulecmd=`ls /opt/Modules/bin/modulecmd.tcl /usr/bin/modulecmd.tcl /usr/share/modules-tcl/libexec/modulecmd.tcl 2>/dev/null`
   echo "# For $HOST cluster:
module () { 
   eval \`tclsh $modulecmd sh \$*\` 
}" >> $env
  
   # New restore/save collection of modules (faster to load) 
   # Try to generalize to other clusters...
   collection_modules=TRUST-$TRUST_VERSION-$HOST_BUILD
   module savelist
   echo "Trying to restore collection of modules named $collection_modules:"
   module restore $collection_modules
   if [ $? != 0 ]
   then
      echo "So we create the collection of modules $collection_modules:"
      # Changing strategy, we load a version for each module (not the last one)
      # Intel compiler (WARNING: Intel 12.x provoque plusieurs problemes (voir FA1445))
	  # Intel 14.0.3.174(default) 16.0.3.210
      module="intel/16.0.3.210"
      echo "Module $module detected and loaded on $HOST."
      module unload intel
      module load $module
      # bullxmpi 1.2.7.2 1.2.8.2 1.2.8.4(default) 1.2.9.2
      #module="mpi/bullxmpi/1.2.8.4"
      #echo "Module $module detected and loaded on $HOST."
      #module unload mpi
      #module load $module
      # openmpi 1.8.8(default) 2.0.2
      module="mpi/openmpi/1.8.8"
      echo "Module $module detected and loaded on $HOST."
      module unload mpi
      module load $module
      #Python 2.6.6 python/2.7.3  python/2.7.8(default)  python/2.7.12  python/3.3.2
      module="python/2.7.8"
      echo "Module $module detected and loaded on $HOST."
      module unload python
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
   soumission=2 && [ "$prod" = 1 ] && soumission=1
   # -M ram with ram>4000 is not supported anymore (06/2014)
   # ram=64000 # 64 GB asked
   # So we use now n cores for one task to have 4*n GB per task
   [ "$bigmem" = 1 ] && cpus_per_task=16 && soumission=1 # To have 64GB per task
   ntasks=16 # 16 cores per node for standard queue (8 for hybrid, 128 for xlarge)
   # ccc_mqinfo : 
   #Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
   #-------  ---------  --------  -------  -------  --------  ------  ------  ----------
   #long             *        18     2048     4096                        32  3-00:00:00
   #normal           *        20                                         300  1-00:00:00
   #test        hybrid        40       64       64         8               2    00:30:00
   #test      standard        40      512      512        32               2    00:30:00
   #test        xlarge        40      128      128         1               2    00:30:00
   cpu=1800 && [ "$prod" = 1 ] && cpu=86400 # 30 minutes or 1 day
   # Priorite superieure avec test si pas plus de 8 nodes (2*8 cores)
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt 128 ]
   then
      qos=normal
      [ "$prod" = 1 ] && cpu=86400
   else
      qos=test
      cpu=1800
   fi
   # ccc_mpinfo : 
   #PARTITION    STATUS TOT_CPUS TOT_NODES    MpC  CpN SpN CpS TpC
   #standard     up        80368      5023    4000  16   2   8   1
   #hybrid       up          272        34    2900   8   2   4   1
   # On selectionne la queue la moins occupee
   # On attend de voir, certains plantages sur standard
   #queue=`ccc_mpinfo | awk '/up/ {if ($10>free) {free=$10;queue=$1}} END {print queue}'`
   # On force la queue standard
   queue=standard
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt $ntasks ]
   then
      node=1
      if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
      then
         echo "=================================================================================================================="
         echo "Warning: try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks on $queue partition."
         echo "=================================================================================================================="
      fi
   else
      node=0
   fi
   #mpirun="mpirun -np \$BRIDGE_MSUB_NPROC" ne marche pas au dela de 32 procs ?
   mpirun="ccc_mprun -n \$BRIDGE_MSUB_NPROC"
   [ "`basename $MPI_ROOT`" = mpich ] && mpirun="$Mpirun -np \$BRIDGE_MSUB_NPROC" # Try support of MVAPICH...
   sub=CCC
   #project="genden"
   [ "$project" = "" ] && project=`ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
