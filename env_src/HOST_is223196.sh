#!/bin/bash
#############################################
# PC configured with advanced GNU compilers #
#############################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   GNU_ENV=`ls /export/home/triou/g++-last/env.sh 2>/dev/null`
   if [ "$GNU_ENV" != "" ]
   then
      ligne_supp="source $GNU_ENV # Last GNU compilers"
      echo $ligne_supp  >> $env
      source $GNU_ENV  
      if [ "$TRUST_FORCE_PROVIDED_OPENMPI" != 1 ] && [ "$TRUST_FORCE_LATEST_OPENMPI" != 1 ] && [ "$TRUST_FORCE_PROVIDED_MPICH" != 1 ]
      then
        echo "You must use provided mpi on this host"
        exit -1
      fi
   else
      echo "No GNU environment found under /export/home/triou"
      echo "Contact TRUST support."
      exit -1
   fi
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=999
   sub=""
}
