#!/bin/bash

###########################################################################
# Welcome to the CSP (Centre de Simulation Palois)			  #
# ------------------------------------------------			  #
# NOTICE TO USERS: WARNING ! The use of this system is restricted	  #
# to authorized users.							  #
###########################################################################
#									  #
# Maintenance announces:						  #
#									  #
###########################################################################
#									  #
# Software releases:							  #
#									  #
###########################################################################
#									  #
# Contacts:								  #
# Thierry BERIT-DEBAT							  #
# Technical Responsible							  #
# (+33)5.40.17.64.69.							  #
# (+33)6.48.69.21.98.							  #
# thierry.berit-debat@c-s.fr						  #
# admin.csp@c-s.fr							  #
#									  #
###########################################################################
#									  #
# You are now connected to frontal-csp node on which you can only :	  #
# - launch batch jobs							  #
# - manage and edit your files						  #
#									  #
# If you want to compile/debug or interactivily execute a code,		  #
# you have to launch an interactive session on workers nodes :		  #
# - qsub -I -qInteractive						  #
#									  #
###########################################################################
#									  #
# LOGIN@172.19.10.81's password:					  #
#									  #
# Linux frontal-csp 2.6.32-33-generic #70-Ubuntu SMP Thu Jul 7 21:09:46   #
# UTC 2011 i686 GNU/Linux						  #
# Ubuntu 10.04.1 LTS							  #
#									  #
# Welcome to Ubuntu!							  #
#  * Documentation:  https://help.ubuntu.com/				  #
#									  #
# New release 'precise' available.					  #
# Run 'do-release-upgrade' to upgrade to it.				  #
#									  #
###########################################################################


##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=1 # aucun calcul meme sequentiel sur la frontale
   cpu=00:30:00 && [ "$prod" = 1 ] && cpu=24:00:00 # 30 minutes or 1 day
   ntasks=12    # 12 cores per node
   if [ $NB_PROCS -gt $ntasks ]
   then
      taches=$NB_PROCS  
      noeuds=`echo "$NB_PROCS/$ntasks+1" | bc`
      [ `echo "$NB_PROCS%$ntasks" | bc -l` = 0 ] && noeuds=`echo "$NB_PROCS/$ntasks" | bc`
      if [ "`echo $NB_PROCS | awk -v n=$ntasks '{print $1%n}'`" != 0 ]
      then
         echo "=================================================================================================================="
         echo "Warning: the allocated nodes of $ntasks cores will not be shared with other jobs (--exclusive option used)"
         echo "so please try to fill the allocated nodes by partitioning your mesh with multiple of $ntasks."
         echo "=================================================================================================================="
         cpus_per_task=1 # pour message erreur
      else
         # NB_PROCS multiple de ntasks
         noeuds=`echo $NB_PROCS | awk -v n=$ntasks '{print $1/n}'`
         taches=$ntasks
      fi
      node=1
      [ "$prod" = 1 ] && cpu=72:00:00 # 3 days
   else
      noeuds=1
      taches=$NB_PROCS
   fi
   queue=Batch
   # Ne pas prendre mpirun mais mpiexec
   export mpirun="mpiexec" # Pas installe sur la frontale
   sub=PBS
}
