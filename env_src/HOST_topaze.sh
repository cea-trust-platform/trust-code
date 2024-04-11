#!/bin/bash
##########################################
#          ┌─────────────────┐           #
#          │▀▛▘              │           #
#          │ ▌▞▀▖▛▀▖▝▀▖▀▜▘▞▀▖│           #
#          │ ▌▌ ▌▙▄▘▞▀▌▗▘ ▛▀ │           #
#          │ ▘▝▀ ▌  ▝▀▘▀▀▘▝▀▘│           #
#          └─────────────────┘           #
#  Hotline: hotline.tgcc@cea.fr          #
#           +33 17757 4242               #
#                                        #
#     Help: $ machine.info               #
#                                        #
# Web site: https://www-tgcc.ccc.cea.fr/ #
#                                        #
##########################################

##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # qstat inexistente sur les dernieres machines du CCRT/TGCC
   echo "Command qstat created on $HOST"
   cp $TRUST_ROOT/bin/KSH/qstat_wrapper $TRUST_ROOT/bin/KSH/qstat
   # Load modules
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      if [ "$TRUST_USE_OPENMP" = 1 ]
      then
         module="gnu/8.3.0 mpi/openmpi/4.0.5 cuda/11.0 nvhpc/22.7" # SDK recent pour bug use_device_ptr avec Nvidia (SDK<21.7)
         module="gnu/8.3.0 cuda/11.0 nvhpc/22.7 mpi/openmpi/4.1.4" # OpenMPI 4.1.4 suite recommendation support. OK. GPU-Direct marchait deja avec 4.0.5
	 module="gnu/8.3.0 cuda/11.7 nvhpc/22.7 mpi/openmpi/4.1.4" # Passage a Cuda 11.7 OK pour AmgX 2.4.0 (qui ne compile pas avec <Cuda 11.2)
	 # Tentative de passage a 23.7 (qui impose Cuda 11.8 sur topaze risque) pour bug long (contourne entre temps) mais pb link: 
	 # nvlink fatal   : Input file '/ccc/scratch/cont002/den/ledacp/trust/amgx_openmp_int64/lib/TRUST_mpi_opt.a:Discretisation_tools.cpp.o' newer than toolkit (122 vs 118)
         # module="gnu/8.3.0 flavor/cuda/nvhpc-237 cuda/11.8 nvhpc/23.7 mpi/openmpi/4.1.4" # Passage a 23.7
	 #echo "export NVHPC_CUDA_HOME=\$CUDA_HOME;unset CUDA_HOME" >> $env # Pour desactiver des warnings a la compilation
      else
         #module="gnu/8.3.0 mpi/openmpi/4.0.5 cuda/11.3" # Non, cela crashe en multi-gpu
         module="gnu/8.3.0 mpi/openmpi/4.0.5 cuda/11.2"      
      fi
      echo "export TRUST_CUDA_CC=80 # A100, Cuda Compute Capability" >> $env
   else
      #module="intel/20.0.4 mpi/openmpi/4.0.5"
      # passage a gnu pour v1.9.2
      module="gnu/11 mpi/openmpi/4.0.5"
      module="gnu/11 mpi/openmpi/4.0.5 mkl/20.0.0" # Regression perf 1.9.1-1.9.2 a cause de Lapack dans OpenBlas plus lent que Lapack de Mkl
   fi
   module="python3/3.8.10 swig/4.0.2 texlive cmake/3.22.2 "$module # cmake 3.22 important pour AmgX et Nvidia-HPC
   #
   # Ajout pour charger l'espace disque a la place de SCRATCHDIR pas encore disponible sur topaze:
   #[ "`id | grep gch0504`" != "" ] && sw=dfldatadir/gch0504
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null 2>&1" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
   #[ "$sw" != "" ] && echo "module sw $sw 1>/dev/null" >> $env  # fait planter soumission de jobs pour utilisateur qui n'ont pas ce projet
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=1 && USE_MPIRUN=1 # Pour eviter un segfault dans PETSC_Finalize il faut lancer meme en sequentiel en batch avec srun ....
   ntasks=128 # AMD Milan 128 cores/node
   [ "$NB_PROCS" = 1 ] && cpus_per_task=$ntasks # Pour avoir le max de memoire (celle du noeud) lors du decoupage
# ccc_mqinfo :
# Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
# -------  ---------  --------  -------  -------  --------  ------  ------  ----------
# long          a100        18      256      256         2             100  3-00:00:00
# long        xlarge        18      128      128                        32  3-00:00:00
# long             *        20     2048     2048                       100  3-00:00:00
# normal        a100        20              7680                       100  1-00:00:00
# normal      xlarge        20               128                 5     100  1-00:00:00
# normal           *        20                                         300  1-00:00:00
# inter         a100        40                           1               1    12:00:00
# inter       xlarge        40                           1               1    12:00:00
# inter            *        40                           0               0    12:00:00
# test          a100        40                           2               2    00:30:00
# test         milan        40      768      768         6               2    00:30:00
# test        xlarge        40      128      128         1               2    00:30:00
   if [ "$prod" = 1 ]
   then
      qos=normal && cpu=86400 # 1 day
   else
      qos=test   && cpu=1800 # 30 mn
   fi
   if [ "$gpu" = 1 ]
   then
      # Partition a100 (4 cartes a100 par noeud, 48 noeuds au total soit 192 GPU au total):
      queue=a100 && cpus_per_task=32 # 1 GPU/MPI (OpenMP choix par default)
      [ $NB_PROCS -gt 8 ] && qos=normal # 2 nodes
   elif [ "$bigmem" = 1 ]
   then
      queue=xlarge
      [ $NB_PROCS -gt 128 ] && qos=normal # 1 node
   else
      queue=milan
      [ $NB_PROCS -gt 768 ] && qos=normal # 6 nodes
   fi
# ccc_mpinfo :
#                       --------------CPUS------------  -------------NODES------------
# PARTITION    STATUS   TOTAL   DOWN    USED    FREE    TOTAL   DOWN    USED    FREE     MpC   CpN SpN CpS TpC
# ---------    ------   ------  ------  ------  ------  ------  ------  ------  ------   ----- --- --- --- ---
# milan        up       108544    3584       0  104960     848      28       0     820    2000  128   2  64   1
# xlarge       up          512     128       0     384       4       1       0       3   31250  128   2  64   1
# a100         up         6016       0       0    6016      47       0       0      47    4000  128   2  64   1
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt $ntasks ]
   then
      node=1
      if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
      then
         echo "=================================================================================================================="
         echo "Warning: try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks on $queue partition."
         echo "=================================================================================================================="
      fi
   else
      node=0
   fi
   mpirun="ccc_mprun -n \$BRIDGE_MSUB_NPROC"
   sub=CCC
   espacedir="work,scratch"
   project="dendm2s" #&& [ "`id | grep gch0504`" != "" ] && project=gch0504
}
