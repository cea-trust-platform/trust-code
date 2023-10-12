#!/bin/bash
##########################################
#      (_)_______  ____  ___             #
#     / / ___/ _ \/ __ \/ _ \            #
#    / / /  /  __/ / / /  __/            #
#   /_/_/   \___/_/ /_/\___/             #
#                                        #
#                                        #
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
      cuda_version=10.2.89
      module="gnu/8.3.0 mpi/openmpi/4.1.4 cuda/$cuda_version"
   elif [ "$TRUST_INT64" = 1 ]
   then
      # module="intel/19.0.5.281 mpi/intelmpi/2019.0.5.281" # Desactive car performances meilleures sur grands nombre de procs avec OpenMPI vs IntelMPI 
      # module="intel/19.0.5.281 mpi/openmpi/4.0.2 feature/openmpi/io/collective_buffering" # openmpi/io/collective_buffering recommendation CCRT pour IO paralleles
      # module="intel/20.0.4 feature/openmpi/net/ib/ucx-nocma mpi/openmpi/4.0.5 feature/openmpi/net/ib/ucx-nocma mpi/openmpi/4.0.2 feature/openmpi/io/collective_buffering" # Recommendations CCRT debut 2021 (bcp de coeurs)
      # sw="feature/hcoll/multicast/disable" # Pour supprimer un warning aleatoire au demarrage de GAMELAN (ne gene pas ensuite)
      # 09/02/23 : ND : ajout de gnu/7.3.0 car gcc 4.8.5 natif rhel7 ne supporte pas c++14
      # module="intel/20.0.4 mpi/openmpi/4.0.2" # PL: Retour a OpenMPI/4.0.2 sans rien d'autre car "node failure" sur le calcul GAMELAN a 50K coeurs avec les conseils precedents 
      # ND : avec intel/20.0.4, le cas test Cx plante en // dans petsc, je tente avec intel/19.*
      # module="intel/19.0.5.281 gnu/7.3.0 mpi/openmpi/4.0.2"
      # passage a redhat 8: openmpi 4.0.5 fait une erreur de segmentation lors de l'appel a PetscFinalize(). 4.1.4 ne le fait pas
      #module="intel/20.0.0 mpi/openmpi/4.1.4"
      # passage a gnu pour v1.9.2
      # Ajout de mkl car regression de perf sur irene-skl 191 -> 192
      module="gnu/11 mpi/openmpi/4.1.4 mkl/20.0.0"
   else
      # 09/02/23 : ND : ajout de gnu/7.3.0 car gcc 4.8.5 natif rhel7 ne supporte pas c++14
      #intel="intel/18.0.3.222 gnu/7.3.0" 
      romio_hints="feature/openmpi/io/collective_buffering"
      #mpi="mpi/intelmpi/2018.0.3.222"       # 1.8.2
      #mpi="mpi/openmpi/2.0.4 $romio_hints"  # 1.8.3 (car crash intelmpi sur grands nbrs de procs)
      #mpi="$romio_hints mpi/openmpi/2.0.4" # suite maintenance 1.8.5b charger openmpi en dernier
      #module="$intel $mpi"
      # passage a redhat 8.4, anciens modules n'existent plus
      #module="$romio_hints intel/20.0.0 mpi/openmpi/4.1.4"
      # passage a gnu pour v1.9.2
      # Ajout de mkl car regression de perf sur irene-skl 191 -> 192
      module="$romio_hints gnu/11 mpi/openmpi/4.1.4 mkl/20.0.0"
   fi
   # 2023/10/02: libgd utile pour installer gnuplot
   module="python3/3.8.10 swig/4.0.2 $module libgd/2.3.2"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null 2>/dev/null" >> $env
   echo "module load $module 1>/dev/null 2>/dev/null || exit -1" >> $env
   echo "ml unload --force --no-auto cuda" >> $env
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=2
   [ "$prod" = 1 ] && soumission=1
   [ "$gpu"  = 1 ] && soumission=1
   # ram=16000 # 16 GB asked
   # So we use now n cores for one task to have 4*n GB per task
   [ "$bigmem" = 1 ] && cpus_per_task=4 && soumission=1 # To have 16GB per task
   # 48 cores per node for skylake queue (68 for KNL queue), 128 for AMD rome
   ntasks=48 
   # ccc_mqinfo : 
   #Name     Partition  Priority  MaxCPUs  SumCPUs  MaxNodes  MaxRun  MaxSub     MaxTime
   #-------  ---------  --------  -------  -------  --------  ------  ------  ----------
   #long             *        18     2064     2064                        32  3-00:00:00
   #normal           *        20                                         300  1-00:00:00
   #test       skylake        40     1344     1344        28               2    00:30:00
   cpu=1800 && [ "$prod" = 1 ] && cpu=86400 # 30 minutes or 1 day
   # Priorite superieure avec test si pas plus de 28 nodes (48 cores)
   if [ "$prod" = 1 ] || [ $NB_PROCS -gt 1344 ]
   then
      qos=normal
      [ "$prod" = 1 ] && cpu=86400
   else
      qos=test 
      cpu=1800
   fi
   # ccc_mpinfo : 
   #PARTITION    STATUS TOT_CPUS TOT_NODES    MpC  CpN SpN CpS TpC
   #skylake      up        74256      1547    3750  48   2  24   1
   #knl          up        38284       563    1411  68   1  68   1
   #rome         up       269056      2102    1875  128  8  16   1
   queue=skylake
   [ "$bigmem" = 1 ] && queue=knl && ntasks=68
   [ "$gpu" = 1 ]    && queue=hybrid && ntasks=96
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
   binding=""
   mpirun="ccc_mprun $binding -n \$BRIDGE_MSUB_NPROC"
   sub=CCC
   espacedir="work,scratch"
   project="dendm2s" 
   [ "$project" = "" ] && project=`PYTHONPATH=/usr/lib64/python2.7/site-packages ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
