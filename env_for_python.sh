#!/bin/bash
# TRUST python tools initialization script

err=0
# check if TRUST env is sourced. Otherwise source it !
if [ ! -d "${TRUST_ROOT}" ]; then
  DIR=$(cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" && pwd)
  source $DIR/env_TRUST.sh || err=1
fi

[ $err = 1 ] && echo "Error: cannot load env_for_python.sh. TRUST not yet installed!" && return 1

ok=1

# Chargement TRUST ICoCo swig
if [ "`echo $PYTHONPATH | grep -i Outils/ICoCo/ICoCo_src/share/swig/install/lib`" = "" ] && [ "$TRUST_DISABLE_MPI" -eq 0 ]
then
  if [ -f $TRUST_ROOT/Outils/ICoCo/ICoCo_src/share/swig/install/lib/_trusticoco.so ]
  then
     source $TRUST_ROOT/Outils/ICoCo/ICoCo_src/share/swig/env.sh
  else
     echo "Error: cannot load 'trusticoco', not yet installed!"
     ok=0
  fi
fi

# Chargement MEDCoupling
if [ "`echo $PYTHONPATH | grep -i LIBMEDCOUPLING/install/bin`" = "" ]
then
  if [ -f $TRUST_MEDCOUPLING_ROOT/env.sh ]
  then
     source $TRUST_MEDCOUPLING_ROOT/env.sh
  else
     echo "Error: cannot load MEDCoupling, not yet installed!"
     ok=0
 fi
fi

# Chargement lata_tools
if [ "`echo $PYTHONPATH | grep -i exec/lata_tools`" = "" ]
then
  source $TRUST_ROOT/Outils/lata_tools/src/tools/env.sh
fi

# Chargement de trustpy
if [ "`echo $PYTHONPATH | grep -i trustpy`" = "" ]
then
  source $TRUST_ROOT/Outils/trustpy/env.sh
fi


if [ $ok = 1 ]; then 
   echo "env_for_python.sh successfully loaded."
else
   echo "env_for_python.sh only partially loaded!!"
fi

