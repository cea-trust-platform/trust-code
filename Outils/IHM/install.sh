#!/bin/bash

# verification of modules
(cd $TRUST_ROOT/Outils/Python_modules&& make ) || exit -1
. $TRUST_ROOT/exec/python_modules/env.sh

export MED_ROOT_DIR=$TRUST_MEDCOUPLING_ROOT


ORG=`pwd`
for rep in PLOT2D TRUST_PLOT2D TRUST_WIZARD
do
  BUILD=$TRUST_TMP/build/$ORG/build_$rep
  mkdir -p $BUILD
  cd $BUILD
  cmake $ORG/$rep -DSALOME_${rep}_STANDALONE=ON -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/$rep || exit -1
  make || exit -1
  make install || exit -1
  export PLOT2D_ROOT_DIR=$TRUST_ROOT/exec/PLOT2D
done
