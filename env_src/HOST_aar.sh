#!/bin/bash
########################################################
# AAR Cluster configured with non native GNU compilers #
########################################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   GNU_ENV=`ls /home/prog/trust/env_gcc_5-5-0_ompi.sh 2>/dev/null`
   if [ "$GNU_ENV" != "" ]
   then
      ligne_supp="source $GNU_ENV # GNU compiler and openmpi"
      echo $ligne_supp  >> $env
      echo "
# link to add qstat on aar for submission BEGIN
PATH=/home/prog/torque/torque-2.5.12/bin:/home/prog/torque/maui-3.3.1/bin:\$PATH
LS_COLORS=''
PBS_SERVER=aar
export PATH LS_COLORS PBS_SERVER
# link to add qstat on aar for submission END"  >> $env
      source $GNU_ENV
   else
      echo "No GNU environment found under /home/prog/trust/"
      echo "Contact TRUST support."
      exit -1
   fi
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=1
   cpu=48:00:00 # 2 days
   qos=""
   queue="default"
   ntasks=1
   ram="16g"
   if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" = 0 ]
   then
      # NB_PROCS multiple de $ntasks
      noeuds=`echo $NB_PROCS | awk -v n=$ntasks '{print $1/n}'`
      taches=$ntasks             # Nb coeurs
   else
      if [ $NB_PROCS -lt $ntasks ]
      then
         noeuds=1                # 1 noeud
         taches=$NB_PROCS        # NB_PROCS coeurs
      else
         noeuds=`echo "$NB_PROCS/$ntasks+1" | bc`
         [ `echo "$NB_PROCS%$ntasks" | bc -l` = 0 ] && noeuds=`echo "$NB_PROCS/$ntasks" | bc`
         [ "$noeuds" = 0 ] && noeuds=1
         taches=$ntasks          # Nb coeurs
         cpus_per_task=1         # pour message erreur
      fi
   fi

   mpirun="mpirun -np $NB_PROCS"

   sub=PBS
}

