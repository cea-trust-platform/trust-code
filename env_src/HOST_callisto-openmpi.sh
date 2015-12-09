#!/bin/bash
##################################################################################
#                            BIENVENUE SUR CALLISTO !                            #
#                                                                                #
# * Documentation : evince /cm/shared/docs/callisto.pdf                          #
# * Remarque : Les noeuds des partitions slim, large et fat sont interconnectes  #
#              par un reseau InfiniBand PSM alors que ceux des partitions eris   #
#              et pluton s'appuient sur des equipements Mellanox.                #
#              Les jobs qui ont recours au module OpenMPI peuvent s'executer sur #
#              les deux partitions. Les utilisateurs de MVAPICH2 devront quant   #
#              a eux utiliser des modules specifiques a chaque type d'IB.        #
# * Astuce : Pour diminuer vos temps d'attente, il est possible de specifier     #
#            plusieurs partitions au scheduler, par exemple "-p slim,large,eris".#
#            Vos jobs seront ainsi executes sur la premiere partition a disposer #
#            des ressources necessaires.                                         #
#                                                                                #
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
   echo "source /cm/local/apps/environment-modules/3.2.10/init/bash" >> $env
   echo "module load $module 1>/dev/null" >> $env
   #
   #OPENMPI module openmpi/gcc/64/1.8 openmpi/gcc/64/1.8.3 openmpi/gcc/64/1.8.4 
   # Aero_192: 3.7s
   module="openmpi/gcc/64/1.8.3"
   #module="openmpi/gcc/64/1.8.4"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module unload mpich openmpi mvapich mvapich2 intel/compiler intel/mkl intel/tbb 1>/dev/null" >> $env
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
   # sacctmgr list qos format=Name,Priority,MaxSubmit,MaxWall,MaxNodes :      
   # Name   Priority MaxSubmit     MaxWall MaxNodes 
   #---------- ---------- --------- ----------- -------- 
   # normal         20            2-00:00:00       20 
   #   test         40         2    01:00:00        2 
   #   long         10           14-00:00:00       10 
   cpu=60 && qos=test				# Qos test   1 hour (2 nodes max=20 CPUs)	Priority 40
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt 20 ]
   then
      cores_per_node=20
      if [ "`echo $NB_PROCS | awk -v n=$cores_per_node '{print $1%n}'`" != 0 ]
      then
         echo "=================================================================================================================="
         echo "Warning: the allocated nodes of $cores_per_node cores will not be shared with other jobs (--exclusive option used)"
	 echo "so please try to fill the allocated nodes by partitioning your mesh with multiple of $cores_per_node."
	 echo "=================================================================================================================="
      fi
      if [ "$NB_PROCS" -gt 200 ]
      then
         qos=normal && cpu=2880 		# Qos normal 2 days (20 nodes max=400 CPUs)	Priority 20
      else
         qos=long && cpu=20160   		# Qos long  14 days (10 nodes max=200 CPUs)	Priority 10
      fi
      qos=normal && cpu=2880 # By default, cause qos long is a small priority
   fi
   # Partition:
   queue=slim 					# 40*2*10=800 cores  6.4Go/core, 128GO/node
   if [ "$bigmem" = 1 ]
   then
      queue=large 				# 16*2*10=320 cores 12.8Go/core, 256Go/node
      queue=fat 				#  2*4*6 =48  cores 21.3Go/core, 512Go/node
   fi
   #project=stmf
   #if [ "`basename $Mpirun`" = srun ]
   #then
   #   # Slurm srun support
   #   mpirun="srun -n $NB_PROCS"
   #else
   #   mpirun="mpirun -np $NB_PROCS"
   #fi
   # 01/07/2014: Regression when using mpirun with openmpi (very bad performance *3) so:
   # srun for everyone (even if it is slightly slower, binding ?)
   #mpirun="srun --cpu_bind=verbose,cores -n $NB_PROCS"
   # 18/08/2015: avec openmpi 1.8.4 srun ne passe pas... donc on repasse a mpirun -np
   #mpirun="mpirun -np $NB_PROCS"
   # retour en arriere openmpi 1.8.3
   mpirun="srun -n \$SLURM_NTASKS"
   sub=SLURM
}
