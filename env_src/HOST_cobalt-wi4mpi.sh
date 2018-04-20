#!/bin/bash
###########################################
#    ______      __          ____         #
#   / ____/___  / /_  ____ _/ / /_        #
#  / /   / __ \/ __ \/ __ `/ / __/        #
# / /___/ /_/ / /_/ / /_/ / / /_          #
# \____/\____/_.___/\__,_/_/\__/          #
#                                         #
#   Hotline: hotline.tgcc@cea.fr          #
#            +33 17757 4242               #
#                                         #
#      Help: $ machine.info               #
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
   #
   # Load modules
   # intel 14.0.3.174 16.0.3.210(default) intel/17.0.4.196
   intel="intel/17.0.4.196"
   #Vgcc # gnu 4.8.4 4.8.5(default) 4.9.3 5.4.0 6.1.0 6.3.0 7.1.0
   #Vgcc intel="gnu/4.8.5"
   # openmpi mpi/openmpi/1.8.4 mpi/openmpi/1.8.8(default) mpi/openmpi/2.0.2 
   # bibliotheque MellanoX Messaging (MXM) pour optimisation des communications MPI mpiprofile/openmpi/mxm
   #openmpi="mpi/openmpi/1.8.8 mpiprofile/openmpi/mxm"
   #openmpi="mpi/openmpi/2.0.2"
   #Vgcc openmpi="feature/openmpi/gnu"
   # intelmpi mpi/intelmpi/5.1.3.210  mpi/intelmpi/2017.0.4.196(default) # 2016: You are currently not allowed to use this software
   #openmpi="mpi/intelmpi/2017.0.4.196"
   # wi4mpi mpi/wi4mpi/3.0.5 mpi/wi4mpi/3.0.6(default) mpi/wi4mpi/3.1.0 mpi/wi4mpi/3.1.2
   openmpi="mpi/wi4mpi/3.0.6"
   #Python 2.7.5 python/2.7.8  python/2.7.12(default)  python/2.7.13  python/3.5.2 
   #cmake version 2.8.12.2 cmake/3.5.2  cmake/3.9.1
   #module="$intel $openmpi"
   # newenv/2016-09  newenv/2018-03(default)
   module="newenv/2018-03"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   #Vgcc module="flavor/openmpi/cea flavor/openmpi/gcc"
   #Vgcc echo "module switch $module 1>/dev/null" >> $env
   #
   # If libccc_user module found, load it (this module helps to know the CPU)
   module=libccc_user
   if [ "`tclsh $modulecmd sh show $module 2>&1`" != "" ]
   then
      echo "Module $module detected and loaded on $HOST."
      module load $module  >> $env
   fi
   #
   . $env
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
   ntasks=28 # 28 cores per node for broadwell or hybrid queue (64 for xlarge)
   # ccc_mqinfo : 
   #Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
   #-------  ---------  --------  -------  -------  --------  ------  ------  ----------
   #long             *        20     2048     2048                        32  3-00:00:00
   #normal           *        20                                         128  1-00:00:00
   #test     broadwell        40      280      280        10               2    00:30:00
   cpu=1800 && [ "$prod" = 1 ] && cpu=86400 # 30 minutes or 1 day
   # Priorite superieure avec test si pas plus de 8 nodes (28 cores)
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt 224 ]
   then
      qos=normal
      [ "$prod" = 1 ] && cpu=86400
   else
      qos=test
      cpu=1800
   fi
   # ccc_mpinfo : 
   #PARTITION    STATUS TOT_CPUS TOT_NODES    MpC  CpN SpN CpS TpC
   #broadwell    up        39256      7597    4392  28   4   7   1
   #xlarge       up          192       192   48000  64   4  16   1
   queue=broadwell && [ "$bigmem" = 1 ] && queue=xlarge && ntasks=64
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
   #mpirun="mpirun -np \$BRIDGE_MSUB_NPROC"
   mpirun="ccc_mprun -n \$BRIDGE_MSUB_NPROC"
   sub=CCC
   #project="gch0202"
   #project="den"
   #Your account : 'user' is not attached to an existant project
   #[ "$project" = "" ] && project=`ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
