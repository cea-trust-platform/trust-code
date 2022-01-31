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
      module="gnu/8.3.0 mpi/openmpi/4.0.2 cuda/$cuda_version"
   else
      # module="intel/19.0.5.281 mpi/intelmpi/2019.0.5.281" # Desactive car performances meilleures sur grands nombre de procs avec OpenMPI vs IntelMPI 
      # module="intel/19.0.5.281 mpi/openmpi/4.0.2 feature/openmpi/io/collective_buffering" # openmpi/io/collective_buffering recommendation CCRT pour IO paralleles
      # module="intel/20.0.4 feature/openmpi/net/ib/ucx-nocma mpi/openmpi/4.0.5 feature/openmpi/net/ib/ucx-nocma mpi/openmpi/4.0.2 feature/openmpi/io/collective_buffering" # Recommendations CCRT debut 2021 (bcp de coeurs)
      # sw="feature/hcoll/multicast/disable" # Pour supprimer un warning aleatoire au demarrage de GAMELAN (ne gene pas ensuite)
      module="intel/20.0.4 mpi/openmpi/4.0.2" # PL: Retour a OpenMPI/4.0.2 sans rien d'autre car "node failure" sur le calcul GAMELAN a 50K coeurs avec les conseils precedents 
   fi
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null || exit -1" >> $env
   [ "$sw" != "" ] && echo "module sw $sw 1>/dev/null" >> $env
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
   ntasks=128
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
      qos=normal
      cpu=1800
   fi
   # ccc_mpinfo : 
   #PARTITION    STATUS TOT_CPUS TOT_NODES    MpC  CpN SpN CpS TpC
   #skylake      up        74256      1547    3750  48   2  24   1
   #knl          up        38284       563    1411  68   1  68   1
   #rome         up       269056      2102    1875  128  8  16   1
   queue=rome
   [ "$bigmem" = 1 ] && queue=knl && ntasks=68
   # Partition v100 (4 cartes v100 par noeud): 1 GPU par 10 coeurs alloues donc si moins de 10 coeurs, on fixe a 10:
   [ "$gpu" = 1 ]    && queue=v100 && ntasks=80 && [ $NB_PROCS -lt 10 ] && cpus_per_task=10
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
   # PL: 01/2021 On desactive le binding (recommendation CCRT) suivant:
   # binding="-e '-m block:block --cpu-bind=rank'" # Optimisation des perfs en // comme sur orcus (+30%)
   mpirun="ccc_mprun $binding -n \$BRIDGE_MSUB_NPROC"
   sub=CCC
   espacedir="work,scratch"
   project="dendm2s" && [ "$gpu" = 1 ] && [ "`id | grep gen13364`" != "" ] && project="gen13364" # 10000h sur gen13364
   [ "$project" = "" ] && project=`PYTHONPATH=/usr/lib64/python2.7/site-packages ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
