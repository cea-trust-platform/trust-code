#!/bin/bash
#############################################################################################
# User Commands: CCC                                                                        #
# Script directive: #MSUB                                                                   #
#    #MSUB -q $queue    (Controls which partition is used for the job)                      #
#    #MSUB -Q $qos      (Controls which Quality of Service is specified)                    #
#    #MSUB -T $cpu      (Controls the total run time)                                       #
#    #MSUB -n $NB_PROCS (Controls the number of tasks/cores to be created for the job)      #
#    #MSUB -N $noeuds   (Controls the minimum/maximum number of nodes allocated to the job) #
#    #MSUB -c $ncpus    (Controls the number of CPUs allocated per task)                    #
#    #MSUB -M $ram      (Real memory required per node)                                     #
#############################################################################################

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
   #ram=64000 && [ "$bigmem" = 1 ] && ram=128000 # 64 GB or 128 GB
   # ccc_mqinfo :
   qos=""
   # ccc_mpinfo :
   queue=""
   # CCC ccc_mprun support
   mpirun="ccc_mprun -n \$BRIDGE_MSUB_NPROC"
   #mpirun="mpirun -np \$BRIDGE_MSUB_NPROC"
   sub=CCC
   #project="den"
   #Your account : 'user' is not attached to an existant project
   #project=`ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
