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
   # Load modules
   # intel 15.0.6.233 16.0.3.210(default)
   intel="intel/16.0.3.210"
   # mpi 1.8.8(default) openmpi/1.8.8(default)
   module="$intel mpi/openmpi/1.8.8"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module unload mpi/openmpi intel 1>/dev/null" >> $env
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
   # -M ram with ram>4000 is not supported anymore (06/2014)
   # ram=64000 # 64 GB asked
   # So we use now n cores for one task to have 4*n GB per task
   [ "$bigmem" = 1 ] && cpus_per_task=16 && soumission=1 # To have 64GB per task
   # ccc_mqinfo : 
   #Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
   #-------  ---------  --------  -------  -------  --------  ------  ------  ----------
   #long             *        20     2048     2048                        32  3-00:00:00
   #normal           *        20                                         128  1-00:00:00
   #test     broadwell        40      280      280        10               2    00:30:00
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
   #broadwell    up        36036      1287    4392  28   2  14   1
   queue=broadwell
   #mpirun="mpirun -np $NB_PROCS" ne marche pas au dela de 32 procs ?
   mpirun="ccc_mprun -n $NB_PROCS"
   [ "`basename $MPI_ROOT`" = mpich ] && mpirun="$Mpirun -np $NB_PROCS" # Try support of MVAPICH...
   sub=CCC
}
