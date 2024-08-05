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
      if [ "$TRUST_USE_OPENMP" = 1 ]
      then
         module="gnu/8.3.0 mpi/openmpi/4.0.5 nvhpc/21.3" # TRUST 1.9.2. Cuda 11.2 mais c-amg crash (sur topaze aussi...)
         module="gnu/8.3.0 mpi/openmpi/4.0.5 cuda/11.0 nvhpc/22.7" # Comme sur topaze
         module="gnu/8.3.0 cuda/11.7 nvhpc/22.7 mpi/openmpi/4.1.4" # Comme sur topaze (passage a 11.7)
         #module="gnu/8.3.0 cuda/11.8 nvhpc/22.11 mpi/openmpi/4.1.4" # cuda/11.7 nvhpc/22.7 plus dispo suite maj 04/2024
         module="gnu/8.3.0 nvhpc/23.7 mpi/openmpi/4.1.4" # Passage a Cuda 11.8 et NVHPC 23.7 OK
      else
         echo "Not supported anymore" && exit -1
      fi
      [ "$TRUST_CUDA_CC" = "" ] && TRUST_CUDA_CC=70 # V100
   else
      # module="intel/19.0.5.281 mpi/intelmpi/2019.0.5.281" # Desactive car performances meilleures sur grands nombre de procs avec OpenMPI vs IntelMPI 
      # module="intel/19.0.5.281 mpi/openmpi/4.0.2 feature/openmpi/io/collective_buffering" # openmpi/io/collective_buffering recommendation CCRT pour IO paralleles
      # module="intel/20.0.4 feature/openmpi/net/ib/ucx-nocma mpi/openmpi/4.0.5 feature/openmpi/net/ib/ucx-nocma mpi/openmpi/4.0.2 feature/openmpi/io/collective_buffering" # Recommendations CCRT debut 2021 (bcp de coeurs)
      # sw="feature/hcoll/multicast/disable" # Pour supprimer un warning aleatoire au demarrage de GAMELAN (ne gene pas ensuite)
      # 09/02/23 : ND : ajout de gnu/7.3.0 car gcc 4.8.5 natif rhel7 ne supporte pas c++14
      #module="intel/20.0.4 gnu/7.3.0 mpi/openmpi/4.0.2" # PL: Retour a OpenMPI/4.0.2 sans rien d'autre car "node failure" sur le calcul GAMELAN a 50K coeurs avec les conseils precedents 
      # passage a redhat 8.4, openmpi 4.0.2 n'existe plus, remplace par 4.0.5
      #module="intel/20.0.0 mpi/openmpi/4.0.5"
      # passage a gnu pour v1.9.2
      module="gnu/11 mpi/openmpi/4.0.5 mkl/20.0.0"
   fi
   # depuis v1.9.2, install sans conda, on prend python+swig+cmake module
   # 2023/10/02: libgd utile pour installer gnuplot
   module="python3/3.8.10 swig/4.0.2 cmake/3.22.2 $module"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null 2>/dev/null" >> $env
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
      qos=test && cpu=1800
      [ "$gpu" = 1 ] && [ $NB_PROCS -gt 8 ] && qos=normal && cpu=1800
   fi
   # ccc_mpinfo : 
   #PARTITION    STATUS   TOTAL      DOWN    USED    FREE    TOTAL   DOWN    USED    FREE     MpC    CpN  SpN CpS  TpC GpN GPU Type
   # rome         up       292736       0  290219    2517    2287       0    2270      17     1781  128   8   16   2   0  
   # skylake      up        79200       0   62069   17131    1650       0    1333     317     3687   48   2   24   1   0  
   # a64fx        up         3696      48      48    3600      77       1       1      75      666   48   1   48   1   0  
   # v100         up         1240       0     930     310      31       0      24       7     4375   40   2   20   2   4  nvidia
   # v100l-os     up         1080       0    1044      36      30       0      29       1     9861   36   2   18   2   1  nvidia
   # v100l        up         1080       0    1044      36      30       0      29       1     9861   36   2   18   2   1  nvidia
   # hybrid       up          960       0      48     912      20       0       1      19     3750   48   2   24   1   1  nvidia
   # xlarge       up          448       0     239     209       4       0       3       1    26803  112   4   28   1   1  nvidia
   # v100xl       up          144       0       0     144       2       0       0       2    40277   72   4   18   2   1  nvidia
   queue=rome && [ "$bigmem" = 1 ] && queue=knl && ntasks=68
   # Partition v100 (4 cartes v100 par noeud)
   [ "$gpu" = 1 ] && queue=v100 && ntasks=40 && cpus_per_task=10 # 1 GPU/MPI (OpenMP choix par default)
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
   project="dendm2s" # meme pour gpu, je reste sur dendm2s car plus d'heure sur gen13364?
   #[ "$gpu" = 1 ] && [ "`id | grep gen13364`" != "" ] && project="gen13364" # 10000h sur gen13364
   [ "$project" = "" ] && project=`PYTHONPATH=/usr/lib64/python2.7/site-packages ccc_myproject 2>/dev/null | $TRUST_Awk '/project/ {print $4;exit}'` # Add project
}
