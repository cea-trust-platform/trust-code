#!/bin/bash
########################################################
# Summer Cluster                                       #
########################################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # Initialisation de l environnement module $MODULE_PATH
   echo "source /etc/profile.d/modules.sh " >> $env
   # Load modules
   module="gcc ompi"
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   . $env
}


##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   # pas trouve comment soumettre un job, je met soumission=9 pour autoriser les tests sur frontale
   soumission=9
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

