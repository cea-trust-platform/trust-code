#!/bin/bash
DIR=`dirname $0`
DIR=`cd $DIR;pwd`
DEST=$TRUST_ROOT/build/Outils/IHM
mkdir -p $DEST

cd  $DEST

[ ! -d CURVEPLOT ] && tar zxf $TRUST_ROOT/externalpackages/CURVEPLOT.tgz
file=CURVEPLOT/src/python/controller/utils.py.in
echo patching $file 
 [ ! -f  $file.sa ] && cp $file $file.sa
sed "s/LOG_LEVEL = 0/LOG_LEVEL = 1/g" $file.sa > $file.2
[ "`diff $file.2 $file`" != "" ] && cp $file.2 $file
diff $file $file.sa

STANDALONE=ON
export  CONFIGURATION_ROOT_DIR=$PWD/configuration
if [ "$KERNEL_ROOT_DIR" = "" ]
then
echo "verification of modules"
else
#STANDALONE=OFF
export PYTHON_INCLUDE_DIR=$PYTHON_INCLUDE
export PYTHONLIBS_ROOT_DIR=$PYTHON_ROOT_DIR
fi
export MED_ROOT_DIR=$TRUST_MEDCOUPLING_ROOT
export MEDCOUPLING_ROOT_DIR=$TRUST_MEDCOUPLING_ROOT
export MAKELEVEL=0


ICI=`pwd`
for rep in CURVEPLOT TRUST_PLOT2D TRUST_WIZARD
do
  ORG=$DIR
  [ "$rep" = "CURVEPLOT" ] && ORG=$ICI
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
