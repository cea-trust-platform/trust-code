#!/bin/bash
################################################################################################
# User Commands: PBS/Torque#                                                                   #
# Script directive: #PBS                                                                       #
#    #PBS -q $queue        (Controls which partition is used for the job)                      #
#    #PBS -l qos=$qos      (Controls which Quality of Service is specified)                    #
#    #PBS -l walltime=$cpu (Controls the total run time)                                       #
#    #PBS -l ppn=$taches   (Controls the number of tasks/cores to be created for the job)      #
#    #PBS -l nodes=$noeuds (Controls the minimum/maximum number of nodes allocated to the job) #
#    #PBS -l mem=$ram      (Real memory required per node)                                     #
################################################################################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   # Load modules
   # intel 
   #intel=""
   # mpi
   #module="$intel "
   #
   #echo "# Module $module detected and loaded on $HOST."
   #echo "module purge 1>/dev/null" >> $env
   #echo "module unload mpich openmpi mvapich mvapich2 intel 1>/dev/null" >> $env
   #echo "module load $module 1>/dev/null" >> $env     
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
   cpu=00:30:00 && [ "$prod" = 1 ] && cpu=24:00:00 # 30 minutes or 1 day
   #ram=64000 && [ "$bigmem" = 1 ] && ram=128000 # 64 GB or 128 GB
   # partition
   qos=""
   queue=""
   # specific data cluster
   taches=$NB_PROCS
   noeuds=1
   #ntasks=12
   #noeuds=`echo "$NB_PROCS/$ntasks+1" | bc`
   #[ `echo "$NB_PROCS%$ntasks" | bc -l` = 0 ] && noeuds=`echo "$NB_PROCS/$ntasks" | bc`
   #[ "$noeuds" = 0 ] && noeuds=1
   #mpirun="mpirun -np $NB_PROCS"
   sub=PBS
}
