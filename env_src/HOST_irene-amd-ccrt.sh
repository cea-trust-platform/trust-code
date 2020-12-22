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
   # qstat inexistente sur les dernieres machines du CCRT/TGCC
   echo "Command qstat created on $HOST"
   cp $TRUST_ROOT/bin/KSH/qstat_wrapper $TRUST_ROOT/bin/KSH/qstat
   # Load modules
   intel="intel/19.0.5.281"
   mpi="mpi/wi4mpi" 			# Pour pouvoir ensuite switcher entre intelmpi et openmpi...
   mpi="mpi/intelmpi/2019.0.5.281"
   mpi="mpi/openmpi/4.0.2" 		# Performances meilleures sur grands nombre de procs avec OpenMPI vs IntelMPI
   romio_hints="feature/openmpi/io/collective_buffering"
   module="$intel $mpi $romio_hints"
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
   ntasks=128
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
      qos=normal
      cpu=1800
   fi
   # ccc_mpinfo : 
   #PARTITION    STATUS TOT_CPUS TOT_NODES    MpC  CpN SpN CpS TpC
   #skylake      up        74256      1547    3750  48   2  24   1
   #knl          up        38284       563    1411  68   1  68   1
   #rome         up       269056      2102    1875  128  8  16   1
   queue=rome
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
   binding="-e '-m block:block --cpu-bind=rank'" # Optimisation des perfs en // comme sur orcus (+30%)
   mpirun="ccc_mprun $binding -n \$BRIDGE_MSUB_NPROC"
   sub=CCC
   espacedir="work,scratch"
   project="dendm2s" #&& [ "`id | grep gch0406`" != "" ] && project="gch0406"
   [ "$project" = "" ] && project=`PYTHONPATH=/usr/lib64/python2.7/site-packages ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
