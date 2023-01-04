
#!/bin/bash
##########################################
# PC CEA/DM2S avec modules (Ubuntu/Centos) 
##########################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   # Bye-bye C++ 11 sur Centos7:
   if [ "`gcc -v 2>&1 | grep 4.8`" != "" ]
   then
      echo "source /etc/profile" >> $env
      echo "module purge" >> $env
      #echo "module load gcc/6.5.0 mpich/gcc_6.5.0/3.2.1 || exit -1" >> $env
      echo "module load gcc/6.5.0 || exit -1" >> $env
   fi
   if [ $TRUST_USE_CUDA = 1 ]
   then
      # Surtout ne pas charger le module (car Cuda, MPI,...), uniquement PATH vers les compilateurs
      PATH_COMPILER=`source /etc/profile;module show nvhpc-nompi 2>&1 | awk '/compilers/ && ($2=="PATH") {print $3}'`
      # Mettre dans cet ordre, pour que nvcc soit detecte avant:
      echo "export PATH=\$PATH:$PATH_COMPILER" >> $env
   fi
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
