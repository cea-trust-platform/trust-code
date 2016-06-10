#!/bin/bash

[ ! -d CURVEPLOT ] && tar zxf $TRUST_ROOT/externalpackages/CURVEPLOT.tgz

STANDALONE=ON
if [ "$KERNEL_ROOT_DIR" = "" ]
then
# verification of modules
(cd $TRUST_ROOT/Outils/Python_modules&& make ) || exit -1
. $TRUST_ROOT/exec/python_modules/env.sh
else
#STANDALONE=OFF
export PYTHON_INCLUDE_DIR=$PYTHON_INCLUDE
export PYTHONLIBS_ROOT_DIR=$PYTHON_ROOT_DIR
fi
export MED_ROOT_DIR=$TRUST_MEDCOUPLING_ROOT
export MEDCOUPLING_ROOT_DIR=$TRUST_MEDCOUPLING_ROOT
export MAKELEVEL=0


ORG=`pwd`
for rep in CURVEPLOT TRUST_PLOT2D TRUST_WIZARD
do
  BUILD=$TRUST_TMP/build/$ORG/build_$rep
  BUILD=build_$rep
  mkdir -p $BUILD
  cd $BUILD
  # [ "$rep" != "CURVEPLOT" ] && [ ! -d adm_local ] && ln -sf $CURVEPLOT_ROOT_DIR/adm_local .
  cmake $ORG/$rep -DSALOME_${rep}_STANDALONE=$STANDALONE -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/$rep || exit -1
  make || exit -1
  make install | grep -v Up-to-date || exit -1
  export CURVEPLOT_ROOT_DIR=$TRUST_ROOT/exec/CURVEPLOT
  cd -
done
