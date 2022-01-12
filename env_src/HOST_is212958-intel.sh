#!/bin/bash
#############################################
# PC configured with advanced GNU compilers #
#############################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   echo "source /etc/profile" >> $env
   echo "module purge" >> $env
   echo "module load compilers/intel/2019_update5 mpi/intelmpi/2019_update5 || exit -1 # Load mpi compilers" >> $env
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=999
   sub=""
}
