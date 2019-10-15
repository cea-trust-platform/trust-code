#!/bin/bash
################################################################################################################################
# User Commands: Slurm                                                                                                         #
# Script directive: #SBATCH                                                                                                    #
#    #SBATCH -p, --partition=$queue     (Controls which partition is used for the job)                                         #
#    #SBATCH --qos=$qos                 (Controls which Quality of Service is specified)                                       #
#    #SBATCH -t, --time=$cpu            (Controls the total run time)                                                          #
#    #SBATCH -n, --ntasks=$NB_PROCS     (Controls the number of tasks/cores to be created for the job)                         #
#    #SBATCH -N, --nodes=$noeuds        (Controls the minimum/maximum number of nodes allocated to the job)                    #
#    #SBATCH -c, --cpus-per-task=$ncpus (Controls the number of CPUs allocated per task)                                       #
#    #SBATCH --ntasks-per-node=$ntasks  (Controls the maximum number of tasks/cores per allocated node)                        #
#    #SBATCH --exclusive                (Prevents sharing of allocated nodes with other jobs. Suballocates CPUs to job steps.) #
#    #SBATCH --mem=$ram                 (Real memory required per node)                                                        #
################################################################################################################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   module="slurm intel/compiler intel/mkl openmpi/intel" # openmpi/gcc bloque
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   echo "TRUST_ATELIER_CMAKE=0 && export TRUST_ATELIER_CMAKE # To speedup Baltik build on beefgs file system" >> $env  
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
   cpu=30 && [ "$prod" = 1 ] && cpu=1440 # 30 minutes or 1 day
   #ram=64G && [ "$bigmem" = 1 ] && ram=128G # 64 GB or 128 GB
   # sacctmgr list qos format=Name,Priority,MaxSubmit,MaxWall,MaxNodes :
   qos=""
   # sinfo :
   queue=""
   # specific data cluster
   #ntasks=20
   #if [ "$prod" = 1 ] || [ $NB_PROCS -gt $ntasks ]
   #then
   #   node=1
   #   if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
   #   then
   #      echo "=================================================================================================================="
   #      echo "Warning: the allocated nodes of $ntasks cores will not be shared with other jobs (--exclusive option used)"
   #      echo "so please try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks."
   #      echo "=================================================================================================================="
   #   fi
   #else
   #  node=0
   #fi
   #noeuds=`echo "$NB_PROCS/$ntasks+1" | bc`
   #[ `echo "$NB_PROCS%$ntasks" | bc -l` = 0 ] && noeuds=`echo "$NB_PROCS/$ntasks" | bc`
   #[ "$noeuds" = 0 ] && noeuds=1
   # Slurm srun support
   mpirun="srun -n \$SLURM_NTASKS"
   #mpirun="mpirun -np \$SLURM_NTASKS"
   sub=SLURM
}
