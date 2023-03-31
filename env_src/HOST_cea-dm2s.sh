
#!/bin/bash
##########################################
# PC CEA/DM2S avec modules (Ubuntu/Centos) 
##########################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   echo "source /etc/profile.d/modules.sh" >> $env
   echo "module purge" >> $env
   # PL: on revient a C++11 donc pas de chargement de module gcc par defaut
   #if [ "$TRUST_USE_CUDA" = 1 ] # || [ "`gcc -v 2>&1 | grep 4.8`" != "" ] # Bye-bye C++ 11 sur Centos7:
   #then
   #   echo "module load gcccore/.8.3.0 gcc/8.3.0 || exit -1" >> $env
   #fi
   if [ "$TRUST_USE_CUDA" = 1 ]
   then
      # Surtout ne pas charger le module (car Cuda, MPI,...), uniquement PATH vers les compilateurs
      PATH_COMPILER=`source /etc/profile;module show nvhpc-nompi 2>&1 | awk '/compilers/ && ($2=="PATH") {print $3}'`
      # Mettre dans cet ordre, pour que nvcc soit detecte avant:
      echo "export PATH=\$PATH:$PATH_COMPILER" >> $env
   fi
   #echo "module load texlive/20220321 || exit -1" >> $env
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
