
#!/bin/bash
##########################################
# PC CEA/DM2S avec modules (Ubuntu/Centos) 
##########################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # PL: on revient a C++11 donc pas de chargement de module gcc par defaut, sauf si cuda
   if [ "$TRUST_USE_CUDA" = 1 ]
   then

      echo "source /etc/profile.d/modules.sh" >> $env
      echo "module purge" >> $env
      if [ "`gcc -v 2>&1 | grep 4.8`" != "" ] # Bye-bye C++ 11 sur Centos7:
      then
         echo "module load gcccore/.8.3.0 gcc/8.3.0 || exit -1" >> $env
      else
         echo "module purge" >> $env
	 # Aucun module nvhpc n'est bien installe...
	 # echo "module load nvhpc/23.1 || exit -1" >> $env
      fi
   elif [ "`gcc -v 2>&1 | grep 4.8`" != "" ] 
   then
      # sur pegasi2, tout OK avec gcc 8.3
      echo "source /etc/profile.d/modules.sh" >> $env
      echo "module purge" >> $env
      echo "module load gcccore/.8.3.0 gcc/8.3.0 texlive/20220321 || exit -1" >> $env
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
