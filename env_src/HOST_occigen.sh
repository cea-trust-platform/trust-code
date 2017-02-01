#!/bin/bash
#######################################################################
#            Bienvenue au CINES  sur  OCCIGEN                         #
#                                                                     #
# Cluster BULLx DLC : 2106 noeuds B720, 50544 coeurs,  202 To RAM     #
#                                                                     #
#  -1053 noeuds B720 : 24 coeurs Intel E5-2690 @2.6GHz,  64Go RAM     #
#  -1053 noeuds B720 : 24 coeurs Intel E5-2690 @2.6GHz, 128Go RAM     #
#  -Espace temporaire /scratch(Lustre) : 5 Po  @  ~120Go/s            #
#                                                                     #
# Red Hat 6.7  (kernel :  2.6.32-573.3.1.el6.x86_64)                  #
#                                                                     #
#  - Votre mot de passe doit contenir 12 caracteres minimum           #
#  - Pour acceder a vos espaces de travail, utilisez les variables    #
#    $HOMEDIR $SCRATCHDIR $STOREDIR $SHAREDHOMEDIR $SHAREDSCRATCHDIR  #
#    Exemple d'utilisation : cd $SCRATCHDIR                           #
#                                                                     #
#  Si vous rencontrez le moindre probleme, n'hesitez pas a contacter  #
#  nos equipes support au 04 67 14 14 99 ou par mail svp@cines.fr     #
#                                                                     #
#######################################################################
#######################################################################
#              Bienvenue au CINES  sur  OCCIGEN2                      #
#                                                                     #
#                     Cluster BULLx SCS5                              #
#                                                                     #
#                       Red Hat 7                                     #
#          kernel : 3.10.0-514.el7.x86_64                             #
#                                                                     #
#######################################################################


##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   # Create ROMIO_HINTS file for MPI IO optimizations on Lustre file system
   echo "# ROMIO HINTS
# Select the max OST available:
striping_factor -1
# Collective comm between nodes before write:
romio_cb_write enable
# Collective comm between nodes before read:
romio_cb_read  enable
# One process on each node do the coll comm task:
cb_config_list *:1" > ROMIO_HINTS.env
   echo "export ROMIO_HINTS=\$TRUST_ROOT/env/ROMIO_HINTS.env # ROMIO HINTS" >> $env
   #
   # Load modules
   # intel 15.6.233 16.4.258 17.0
   intel="intel/17.0"
   # intelmpi 2017.0.098 5.1.3.258
   module="$intel intelmpi/2017.0.098" # license is not available now
   # openmpi 2.0.0 2.0.1
   module="$intel openmpi/intel/2.0.1" # error mpif-sizeof.h
   module="openmpi/gnu/2.0.1"
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
   cpu=00:30:00 && [ "$prod" = 1 ] && cpu=24:00:00 # 30 minutes or 1 day
   #ram=60GB # Memory nodes with 64Go (small) or 120Go (large) of RAM
   [ "$bigmem" = 1 ] && ram=120GB && soumission=1
   project="BDW28" # project="HSW24"
   ntasks=28       # 28 cores per node for Broadwell (24 for Haswell)
   # Priorite superieure avec test si pas plus de 8 nodes (24 cores)
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt 192 ]
   then
      [ "$prod" = 1 ] && cpu=24:00:00
   else
      cpu=00:30:00
   fi
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt $ntasks ]
   then
      node=1
      if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
      then
         echo "=================================================================================================================="
         echo "Warning: the allocated nodes of $ntasks cores will not be shared with other jobs (--exclusive option used)"
         echo "so please try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks."
         echo "=================================================================================================================="
      fi
   else
     node=0
   fi
   noeuds=`echo "$NB_PROCS/$ntasks+1" | bc`
   [ `echo "$NB_PROCS%$ntasks" | bc -l` = 0 ] && noeuds=`echo "$NB_PROCS/$ntasks" | bc`
   [ "$noeuds" = 0 ] && noeuds=1
   # srun Run a parallel job on cluster managed by SLURM
   # option --mpi Identify the type of MPI to be used : pmi2 load the library lib/slurm/mpi_pmi2.so
   # option -K = --kill-on-bad-exit 0|1
   # option --resv-ports Reserve communication ports for this job. Used for OpenMPI. 
   mpirun="srun --mpi=pmi2 -K1 --resv-ports -n \$SLURM_NTASKS"
   sub=SLURM
}
