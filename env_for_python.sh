#!/bin/bash
# TRUST python tools initialization script

err=0
# check if TRUST env is sourced. Otherwise source it !
if [ ! -d "${TRUST_ROOT}" ]; then
  DIR=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)
  source $DIR/env_TRUST.sh || err=1
fi

[ $err = 1 ] && echo "Error: cannot load env_for_python.sh. TRUST not yet installed!" && return 1

# Chargement TRUST ICoCo swig
if [ "`echo $PYTHONPATH | grep -i Outils/ICoCo/ICoCo_src/share/swig/install/lib`" = "" ] && [ "$TRUST_DISABLE_MPI" -eq 0 ]
then
  if [ -f $TRUST_ROOT/Outils/ICoCo/ICoCo_src/share/swig/install/lib/_trusticoco.so ]
  then
     source $TRUST_ROOT/Outils/ICoCo/ICoCo_src/share/swig/env.sh
  else
     echo "Error: cannot load env_for_python.sh. trusticoco not yet installed!"
     return 1
  fi
fi

# Chargement MEDCoupling
if [ "`echo $PYTHONPATH | grep -i LIBMEDCOUPLING/install/bin`" = "" ]
then
  if [ -f $TRUST_MEDCOUPLING_ROOT/env.sh ]
  then
     source $TRUST_MEDCOUPLING_ROOT/env.sh
  else
     echo "Error: cannot load env_for_python.sh. MEDCoupling not yet installed!"
     return 1
 fi
fi

echo "env_for_python.sh successfully loaded."
