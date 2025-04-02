#!/bin/bash

# MEDCoupling charge par defaut est l'optim, possible de charger $TRUST_MEDCOUPLING_ROOT/env_debug.sh
. $TRUST_MEDCOUPLING_ROOT/env.sh

export PYTHONPATH=$TRUST_ROOT/Outils/ICoCo/ICoCo_src/share/swig/install/lib:$PYTHONPATH
#export LD_LIBRARY_PATH=$TRUST_ROOT/exec/python/lib:$LD_LIBRARY_PATH  # Not needed anymore ?

