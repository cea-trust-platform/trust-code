#!/bin/bash
######################################
# PC configured with NVidia compiler #
######################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   echo "source /etc/profile" >> $env
   echo "module purge" >> $env
   echo "module load gcc/6.3.0 nvhpc/22.1 || exit -1" >> $env
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
