#!/bin/bash
##############################################################################################################################
# User Commands: LoadLeveler                                                                                                 #
# Script directive: #@                                                                                                       #
#    #@ class=$queue                  (Controls which partition is used for the job)                                         #
#    #@ wall_clock_limit=$cpu         (Controls the total run time)                                                          #
#    #@ total_tasks=$NB_PROCS         (Controls the number of tasks/cores to be created for the job)                         #
#    #@ node=$noeuds                  (Controls the minimum/maximum number of nodes allocated to the job)                    #
#    #@ tasks_per_node=$ntasks        (Controls the maximum number of tasks/cores per allocated node)                        #
#    #@ node_usage=not_shared         (Prevents sharing of allocated nodes with other jobs. Suballocates CPUs to job steps.) #
#    #@ requirements=(Memory >= $ram) (Real memory required per node)                                                        #
##############################################################################################################################

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
   queue=""
   # specific data cluster
   noeuds=1
   #ntasks=12
   #noeuds=`echo "$NB_PROCS/$ntasks+1" | bc`
   #[ `echo "$NB_PROCS%$ntasks" | bc -l` = 0 ] && noeuds=`echo "$NB_PROCS/$ntasks" | bc`
   #mpirun="mpirun -np $NB_PROCS"
   sub=POE
}
