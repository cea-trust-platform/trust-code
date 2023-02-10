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
   echo 
   echo "module load gcc/6.5.0 iccifort/2019.5.281 impi/icc_2019.5.281/2019.5.281 || exit -1 # Load mpi compilers" >> $env
   #echo "module load gcc/6.5.0 intel-compilers/2021.4.0 imkl/2021.4.0 ucx/1.11.2 impi/intel-compilers_2021.4.0/2021.4.0 || exit -1 # Load mpi compilers" >> $env
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
