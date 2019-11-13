#!/bin/bash
##################################################################################
#                            Cluster PlaFRIM !                                   #
##################################################################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # Load modules
   module=slurm
   echo "# Module $module detected and loaded on $HOST."
   # Initialisation de l environnement module $MODULE_PATH 
   #echo "source /cm/local/apps/environment-modules/3.2.10/init/bash" >> $env
   echo "module load $module 1>/dev/null" >> $env
   #
   # module="mpi/openmpi/4.0.1  compiler/gcc/9.2.0" sur la nouvelle plafrim ?
   # avec mpi/openmpi/gcc/4.0.0 et mpi/openmpi/gcc/4.0.1 le mpif90 ne marche pas
   module="compiler/gcc/4.8.4 mpi/openmpi/gcc/3.0.0"
   #
   echo "# Module $module detected and loaded on $HOST."
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
   #PARTITION     AVAIL  TIMELIMIT  NODES  STATE 
   #defq*            up    2:00:00     46 mix
   #longq            up 3-00:00:00     56 mix
   #court            up    4:00:00     56 mix
   #multiPart        up    1:00:00     57 mix
   #special          up      30:00     56 mix
   #court_souris     up    4:00:00      1 alloc 
   #court_sirocco    up    4:00:00     16 mix
   #long_sirocco     up 3-00:00:00      7 mix
   #kona             up 1-00:00:00      4 idle
   #visu             up    8:00:00      1 idle 
   #court_brise      up 3-00:00:00      1 idle
   #hack             up 1-00:00:00      2 idle 
   #arm              up 3-00:00:00      1 idle 

   queue=court #&& [ "$bigmem" = 1 ] && queue=large && soumission=1
   # sacctmgr list qos format=Name,Priority,MaxSubmit,MaxWall,MaxNodes :      
   #  Name      Priority  MaxSubmit     MaxWall        MaxNodes 
   #---------- ---------- --------- ----------- -------- 
   # normal          0                                
   # multi           0         2                      
   # longq           0        10       3-00:00:00       16 
   # court           0        10         04:00:00          
   # special         0        20                      
   # verylong        0                                
   # sirocco         0                                
   # mistral         0                                
   # qoscompute      0                                
   # qoscrt_mi+      0        10                      
   # qoscrt_si+      0         4                      
   # qoslg_mis+      0         5                      
   # qoslg_sir+      0         2                      
   # qoscrt_mi+      0        16                      
   # qoslg_mir+      0        16                      
   # qos_souris   1000                                
   # defq            0  
   cpu=30 && [ "$prod" = 1 ] && cpu=120 # 30 minutes or 1 day
   ntasks=20 # 20 cores per node for slim or large queue (24 for fat, 8 for eris and 12 for pluton)
   node=0
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt $ntasks ]
   then
      #node=1
      #if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
      #then
      #   echo "=================================================================================================================="
      #   echo "Warning: the allocated nodes of $ntasks cores will not be shared with other jobs (--exclusive option used)"
      #   echo "so please try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks on $queue partition."
      #   echo "=================================================================================================================="
      #fi
      qos=court
      #[ "$prod" = 1 ] && cpu=2880 # 2 days
      [ "$prod" = 1 ] && cpu=120 # 1 day
   else
      #node=0
      queue=court
      qos=court
      [ "$prod" = 1 ] && cpu=60 # 1 hour
   fi
   mpirun="mpirun -np \$SLURM_NTASKS"
   sub=SLURM
}
