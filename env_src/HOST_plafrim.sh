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
   echo "# Module $module detected and loaded on $HOST."
   # Initialisation de l environnement module $MODULE_PATH 
   #echo "source /cm/local/apps/environment-modules/3.2.10/init/bash" >> $env
   # module="mpi/openmpi/4.0.1  compiler/gcc/9.2.0" sur la nouvelle plafrim ?
   # avec mpi/openmpi/gcc/4.0.0 et mpi/openmpi/gcc/4.0.1 le mpif90 ne marche pas
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      module="compiler/cuda/11.2 compiler/gcc/9.3.0 mpi/openmpi/4.0.3"
   else
      module="slurm compiler/gcc/4.8.4 mpi/openmpi/gcc/3.0.0"
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env     
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
# $ sinfo
# PARTITION AVAIL  TIMELIMIT  NODES  STATE NODELIST
# routage*     up 3-00:00:00     20 drain* miriel[005,008,016-017,019-020,022,024,027,032,038,043-044,062,068,073,075,081,083,086]
# routage*     up 3-00:00:00      3  drain sirocco[07,10,13]
# routage*     up 3-00:00:00      8    mix bora040,sirocco[08-09,11,18-20],souris
# routage*     up 3-00:00:00     79  alloc bora[003,005,008-010,012,015-024,027,029,034,037,039,042-044],diablo04,miriel[001-004,006,009-015,018,021,023,025-026,028-031,033-037,039-042,045,048,050-053,056-058,060,063-064,067,069-071,076,078-079,084-085,087-088],sirocco14
# routage*     up 3-00:00:00     61   idle arm01,bora[001-002,004,006-007,011,013-014,025-026,028,030-033,035-036,038,041],brise,diablo[01-03,05],kona[01-04],sirocco[01-05,12,15-17,21],visu01,zonda[01-21]
# preempt      up 3-00:00:00     20 drain* miriel[005,008,016-017,019-020,022,024,027,032,038,043-044,062,068,073,075,081,083,086]
# preempt      up 3-00:00:00     53  alloc miriel[001-004,006,009-015,018,021,023,025-026,028-031,033-037,039-042,045,048,050-053,056-058,060,063-064,067,069-071,076,078-079,084-085,087-088]
# preempt      up 3-00:00:00     21   idle zonda[01-21]
# testing      up 2-00:00:00      3   idle mistral[02-03,06]
# unstable     up 3-00:00:00      3  down* miriel[007,066,072]
# unstable     up 3-00:00:00      1   idle miriel061
   soumission=2 && [ "$prod" = 1 ] && soumission=1
   queue=routage && constraint=bora && [ "$gpu"  = 1 ] && soumission=1 && constraint=sirocco
   cpu=30 && [ "$prod" = 1 ] && cpu=120 # 30 minutes or 1 day
   node=1 # --exclusive
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}
