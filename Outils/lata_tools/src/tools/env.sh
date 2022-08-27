#!/bin/bash

LATA_TOOLS_INSTALL_DIR=$TRUST_ROOT/exec/lata_tools

#
# Source MEDCoupling si pas encore fait !
#
if [ "`echo $PYTHONPATH | grep -i LIBMEDCOUPLING/install/bin`" = "" ]
then
  if [ -f $TRUST_MEDCOUPLING_ROOT/env.sh ]
  then
     source $TRUST_MEDCOUPLING_ROOT/env.sh
  else
     echo "Error: MEDCoupling not yet installed!"
     return 1
 fi
fi

#
# On ajoute lata_tools ... Si pas encore fait ! 
#
if [ "`echo $PYTHONPATH | grep -i exec/lata_tools`" = "" ]
then
  export PYTHONPATH=$LATA_TOOLS_INSTALL_DIR/lib:${PYTHONPATH}
  export LD_LIBRARY_PATH=$LATA_TOOLS_INSTALL_DIR/lib:${LD_LIBRARY_PATH}
fi

