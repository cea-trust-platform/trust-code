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
   # intel/15.0.6.233  intel/16.0.3.210  intel/17.0.4.196  intel/17.0.6.256(default)  intel/18.0.1.163
   intel="intel/17.0.6.256"
   # gnu/4.8.5  gnu/4.9.2  gnu/5.4.0  gnu/6.3.0  gnu/6.4.0  gnu/7.1.0  gnu/7.3.0(default)
   #intel="gnu/7.3.0"
   # openmpi mpi/openmpi/1.8.8 mpi/openmpi/2.0.2 mpi/openmpi/2.0.4(default) mpi/wi4mpi/3.1.5
   #openmpi="mpi/openmpi/2.0.4"
   # intelmpi mpi/intelmpi/2017.0.4.196 mpi/intelmpi/2017.0.6.256 mpi/intelmpi/2018.0.1.163(default)
   #openmpi="mpi/intelmpi/2018.0.1.163"
   # wi4mpi mpi/wi4mpi/3.1.5 mpi/wi4mpi/3.2.0 mpi/wi4mpi/3.2.1(default)
   openmpi="mpi/wi4mpi/3.2.1"
   module="$intel $openmpi"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
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
