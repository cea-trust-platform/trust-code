
#!/bin/bash
##########################################
# PC CEA/DM2S avec modules (Ubuntu/Centos) 
##########################################

define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   echo "source /etc/profile" >> $env
   echo "module purge" >> $env
   # PL: on revient a C++11 donc pas de chargement de module gcc par defaut
   echo "module load gcccore/.8.3.0 gcc/8.3.0 || exit -1" >> $env
   echo "export PATH=$WORKDIR/texlive/2019/bin/x86_64-linux:\$PATH" >> $env
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
