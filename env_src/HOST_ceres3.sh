#!/bin/bash
##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # Load modules
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      module="gnu3/9.3.0 openmpi/3.1.6-gcc-slurm-cuda cuda/11.1.0"
   else
      module="gnu9/9.3.0 openmpi/3.1.6-gcc"
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
# sinfo:
# PARTITION     AVAIL  TIMELIMIT  NODES  STATE NODELIST 
# cpu-6230R        up   infinite      6    mix ceres3-dc[01-03,05-07] 
# cpu-6230R        up   infinite      2   idle ceres3-dc[04,08] 
# gpu-v100s        up   infinite      2   idle ceres3-dg[01-02] 
# gpu-a100         up   infinite      1   idle ceres3-dg03 
# visu             up   infinite      1   idle ceres3-visu 
# cpu-E5-2680v2    up   infinite      5  alloc ceres3-bc[02-03,07-08,10] 
# cpu-E5-2680v2    up   infinite      3   idle ceres3-bc[01,05,09] 
# cpu-E5-2690v4    up   infinite     10    mix ceres3-bc[11,13-14,16-18,20,23,25,27] 
# cpu-E5-2690v4    up   infinite      3  alloc ceres3-bc[15,22,26] 
# cpu-E5-2690v4    up   infinite      5   idle ceres3-bc[12,19,21,24,28]
   if [ "$gpu"  = 1 ]
   then
      soumission=1
      queue="gpu-a100"   
   else
      soumission=2 && [ "$prod" = 1 ] && soumission=1
      queue="cpu-E5-2690v4"
   fi   
   mpirun="mpirun -np \$SLURM_NTASKS"
   sub=SLURM
}
