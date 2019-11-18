#!/bin/bash
#######################################################################
# Jean-Zay2                                                                    #
#######################################################################


##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   # Load modules
   # trust compile avec intel 19.0.2
   module="intel-compilers/19.0.2 intel-mpi/19.0.2"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
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
   # sinfo :
   #PARTITION AVAIL  TIMELIMIT  NODES  STATE 
   #cpu_p1*      up   20:00:00    1528 mix
   #gpu_p1       up   20:00:00     261 mix
   #visu         up    1:00:00      5   idle 
   #archive      up   20:00:00      7    mix 
   #prepost      up   20:00:00      4    mix 


   queue=cpu_p1 # && [ "$bigmem" = 1 ] && queue=large && soumission=1
   # sacctmgr list qos format=Name,Priority,MaxSubmit,MaxWall,MaxNodes :      
   #   Name   Priority MaxSubmit     MaxWall MaxNodes 
   #---------- ---------- --------- ----------- -------- 
   #normal          0                                
   #qos_cpu-d+         80     10000    02:00:00          
   #qos_gpu-d+         80     10000    02:00:00          
   #qos_cpu-gc         50     25000    20:00:00          
   #qos_gpu-gc         50     25000    20:00:00          
   #qos_cpu-t3         50     10000    20:00:00          
   #qos_gpu-t3         50     10000    20:00:00          
   #qos_cpu-ex          0     10000    20:00:00          
   #qos_gpu-ex          0     10000    20:00:00          
   #qos_cpu-d+         50     10000                      
   #qos_gpu-d+         50     10000                      
   #qos_cpu-cp         50     10000                      
   #qos_gpu-cp         50     10000                      
   #qos_prepo+          0     10000                  
   cpu=30 && [ "$prod" = 1 ] && cpu=110 # 30 minutes or 1 day
   ntasks=20 # 20 cores per node for slim or large queue (24 for fat, 8 for eris and 12 for pluton)
   node=0
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}

