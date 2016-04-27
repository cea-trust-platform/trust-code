#!/bin/bash
################################################################################################################
# User Commands: LSF                                                                                           #
# Script directive: #BSUB                                                                                      #
#    #BSUB -q $queue    (Controls which partition is used for the job)                                         #
#    #BSUB -W $cpu      (Controls the total run time)                                                          #
#    #BSUB -n $NB_PROCS (Controls the number of tasks/cores or nodes to be created for the job)                #
#    #BSUB -x           (Prevents sharing of allocated nodes with other jobs. Suballocates CPUs to job steps.) #
#    #BSUB -M $ram      (Real memory required per node)                                                        #
################################################################################################################

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
   # sinfo :
   queue=""
   #mpirun="mpirun -np $NB_PROCS"
   sub=LSF
}
