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
   module="slurm intel/compilers/2019.3.199 openmpi/intel/3.1.3 mkl/2019.3.199 hwloc" # openmpi/gcc bloque
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
   queue=compute && [ "$bigmem" = 1 ] && queue=compute && soumission=1
   cpu=30 && [ "$prod" = 1 ] && cpu=20160
   ntasks=20
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}
