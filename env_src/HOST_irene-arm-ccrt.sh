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
   module="arm-compiler/21.0.0 mpi/openmpi/4.0.5 swig/4.0.2" 
   module="fujitsu/1.1.0 mpi/openmpi/4.0.5 swig/4.0.2" 
   module="gnu/11.1.0 mpi/openmpi/4.0.5 swig/4.0.2" # Mis en premier car le plus robuste
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
   soumission=2 && [ "$prod" = 1 ] && soumission=1
# ccc_mqinfo : 
# Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
# -------  ---------  --------  -------  -------  --------  ------  ------  ----------
# long         a64fx        18     1024     1024                       100  3-00:00:00
# normal           *        20                                         300  1-00:00:00
# test             *        40                           2               2    00:30:00
   cpu=1800 && qos=test && [ "$prod" = 1 ] && cpu=86400 && qos=normal # 30 minutes or 1 days or 3 days
# ccc_mpinfo : 
# --------------CPUS------------  -------------NODES------------
# PARTITION    STATUS   TOTAL   DOWN    USED    FREE    TOTAL   DOWN    USED    FREE     MpC   CpN SpN CpS TpC
#---------    ------   ------  ------  ------  ------  ------  ------  ------  ------   ----- --- --- --- ---
#rome         up       288512     128  287769     615    2254       1    2253       0    1875  128   8  16   1
#v100         up         1280       0    1210      70      32       0      32       0    4500  40   2  20   1
#v100l-os     up         1044       0      36    1008      29       0       1      28   10000  36   2  18   1
#v100l        up         1044       0      36    1008      29       0       1      28   10000  36   2  18   1
#v100xl       up          144       0       0     144       2       0       0       2   41666  72   4  18   1
#a64fx        up         3408      48       0    3360      71       1       0      70     666  48   1  48   1
   queue=a64fx
   ntasks=48
   [ "$bigmem" = 1 ] && cpus_per_task=$ntasks && soumission=1 # Max RAM: 0.666*48~30Go
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
   espacedir="work,scratch"
   project="tgcc9004"
}
