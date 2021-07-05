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
   # qstat inexistente sur les dernieres machines du CCRT/TGCC
   echo "Command qstat created on $HOST"
   cp $TRUST_ROOT/bin/KSH/qstat_wrapper $TRUST_ROOT/bin/KSH/qstat
   # Load modules
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      echo "Not supported yet"
      exit
      module=""
   else
      module="gnu/11.1.0 mpi/openmpi/4.0.5"
      module="intel/20.0.4 mpi/openmpi/4.0.5"
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
   [ "$sw" != "" ] && echo "module sw $sw 1>/dev/null" >> $env
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   #soumission=2
   soumission=1 && USE_MPIRUN=1 # Pour eviter un segfault dans PETSC_Finalize il faut lancer meme en sequentiel en batch avec srun ....
   [ "$prod" = 1 ] && soumission=1
   [ "$gpu"  = 1 ] && soumission=1
   ntasks=128 # AMD Milan 128 cores/node
   # ccc_mqinfo :
# Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
# -------  ---------  --------  -------  -------  --------  ------  ------  ----------
# -------  ---------  --------  -------  -------  --------  ------  ------  ----------
# long          a100        18                                         100  3-00:00:00
# long             *        20     2064     2064                       100  3-00:00:00
# normal           *        20                                         300  1-00:00:00
# normal        a100        20                                         100  1-00:00:00
# test          a100        40                           2               2    00:30:00
# test         milan        40      768      768         6               2    00:30:00
   cpu=1800 && [ "$prod" = 1 ] && cpu=86400 # 30 minutes or 1 day
   # Priorite superieure avec test si pas plus de 7-_ coeurs  
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt 768 ]
   then
      qos=normal
      [ "$prod" = 1 ] && cpu=86400
   else
      qos=test
      cpu=1800
   fi
   # ccc_mpinfo : 
#                       --------------CPUS------------  -------------NODES------------
# PARTITION    STATUS   TOTAL   DOWN    USED    FREE    TOTAL   DOWN    USED    FREE     MpC   CpN SpN CpS TpC
# ---------    ------   ------  ------  ------  ------  ------  ------  ------  ------   ----- --- --- --- ---
# milan        up       108544    3584       0  104960     848      28       0     820    2000  128   2  64   1
   queue=milan
   [ "$bigmem" = 1 ] && queue=xlarge
   [ "$gpu" = 1 ]    && queue=a100  # && [ "`sinfo | grep $queue | grep idle`" = "" ] && [ $qos = test ] && queue=hybrid # Pour tester si v100 pas libre, on prend hybrid&& [ "`sinfo | grep $queue | grep idle`" = "" ] && [ $qos = test ] && queue=hybrid # Pour tester si v100 pas libre, on prend hybrid
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
   mpirun="ccc_mprun -n \$BRIDGE_MSUB_NPROC"
   sub=CCC
   project=dendm2s && [ "`id | grep gch0504`" != "" ] && project=gch0504
}
