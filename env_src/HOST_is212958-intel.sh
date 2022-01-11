#!/bin/bash
#############################################
# PC configured with advanced GNU compilers #
#############################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   echo "module purge" >> $env
   echo "module load compilers/intel/2019_update3 mpi/intelmpi/2019_update3 # Load mpi compilers" >> $env
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
