#!/bin/bash
#################################################################################################################
# User Commands: SGE                                                                                            #
# Script directive: #$                                                                                          #
#    #$ -q $queue        (Controls which partition is used for the job)                                         #
#    #$ -l h_rt=$cpu     (Controls the total run time)                                                          #
#    #$ -pe $taches      (Controls the number of tasks/cores to be created for the job)                         #
#    #$ -l exclusive     (Prevents sharing of allocated nodes with other jobs. Suballocates CPUs to job steps.) #
#    #$ -l mem_free=$ram (Real memory required per node in MegaBytes)                                           #
#################################################################################################################

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
   cpu=1800 && [ "$prod" = 1 ] && cpu=86400 # 30 minutes or 1 day
   #ram=64G && [ "$bigmem" = 1 ] && ram=128G # 64 GB or 128 GB
   # partition
   queue=""
   # specific data cluster
   taches=$NB_PROCS
   #mpirun="mpirun -np $NB_PROCS"
   sub=SGE
}
