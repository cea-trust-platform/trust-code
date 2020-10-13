#!/bin/bash
##########################################
#      (_)_______  ____  ___             #
#     / / ___/ _ \/ __ \/ _ \            #
#    / / /  /  __/ / / /  __/            #
#   /_/_/   \___/_/ /_/\___/             #
#                                        #
#                                        #
#  Hotline: hotline.tgcc@cea.fr          #
#           +33 17757 4242               #
#                                        #
#     Help: $ machine.info               #
#                                        #
# Web site: https://www-tgcc.ccc.cea.fr/ #
#                                        #
##########################################

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
   # modulecmd=`ls /opt/Modules/bin/modulecmd.tcl /usr/bin/modulecmd.tcl /usr/share/modules-tcl/libexec/modulecmd.tcl 2>/dev/null`
   # echo "# For $HOST cluster:
   # module () {
   #    eval \`tclsh $modulecmd sh \$*\`
   # }" >> $env
   #
   # Load modules
   intel="intel/18.0.3.222" 
   intelmpi="mpi/intelmpi/2018.0.3.222"
   module="$intel $intelmpi"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=2 && [ "$prod" = 1 ] && soumission=1
   # ram=16000 # 16 GB asked
   # So we use now n cores for one task to have 4*n GB per task
   [ "$bigmem" = 1 ] && cpus_per_task=4 && soumission=1 # To have 16GB per task
   # 48 cores per node for skylake queue (68 for KNL queue), 128 for AMD rome
   ntasks=48 
   # ccc_mqinfo : 
   #Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
   #-------  ---------  --------  -------  -------  --------  ------  ------  ----------
   #long             *        18     2064     2064                        32  3-00:00:00
   #normal           *        20                                         300  1-00:00:00
   #test       skylake        40     1344     1344        28               2    00:30:00
   cpu=1800 && [ "$prod" = 1 ] && cpu=86400 # 30 minutes or 1 day
   # Priorite superieure avec test si pas plus de 28 nodes (48 cores)
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt 1344 ]
   then
      qos=normal
      [ "$prod" = 1 ] && cpu=86400
   else
      qos=test 
      cpu=1800
   fi
   # ccc_mpinfo : 
   #PARTITION    STATUS TOT_CPUS TOT_NODES    MpC  CpN SpN CpS TpC
   #skylake      up        74256      1547    3750  48   2  24   1
   #knl          up        38284       563    1411  68   1  68   1
   #rome         up       269056      2102    1875  128  8  16   1
   queue=skylake
   [ "$bigmem" = 1 ] && queue=knl && ntasks=68
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
   binding=""
   mpirun="ccc_mprun $binding -n \$BRIDGE_MSUB_NPROC"
   sub=CCC
   espacedir="work,scratch"
   project="dendm2s" 
   [ "$project" = "" ] && project=`PYTHONPATH=/usr/lib64/python2.7/site-packages ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
