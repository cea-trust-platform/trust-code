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
   #module="intel-compilers/18.0.1 intel-mpi/18.0.1" # petsc a plante
   # 14/11/2019 : modules intel-mpi et intel-compilers par defaut sont 19.0.4, je les prends pour la compilation
   #module="intel-compilers/19.0.4 intel-mpi/19.0.4" # yanshu a deja utilise intel-mpi/19-* pour une compilation de trust 176
   # avec les 2019 j ai eu l erreur :
   # [100%] Linking CXX executable /gpfswork/rech/ise/usd69jp/TRUST/TRUST-1.7.9/exec/TRUST_mpi_opt
   #/usr/bin/ld: /gpfswork/rech/ise/usd69jp/TRUST/TRUST-1.7.9/lib/src/LIBMED/MED/lib/libhdf5.a(H5Ztrans.o): undefined reference to symbol '__svml_udiv4'
   #/gpfslocalsys/intel/parallel_studio_xe_2019_update4_cluster_edition/compilers_and_libraries_2019.4.243/linux/compiler/lib/intel64/libsvml.so: error adding symbols: DSO missing from command line
   #collect2: error: ld returned 1 exit status
   #make[2]: *** [/gpfswork/rech/ise/usd69jp/TRUST/TRUST-1.7.9/exec/TRUST_mpi_opt] Error 1
   #make[1]: *** [CMakeFiles/TRUST_mpi_opt.dir/all] Error 2
   #make: *** [all] Error 2

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

