#!/bin/bash

export MED_COUPLING_ROOT=$TRUST_MEDCOUPLING_ROOT
ORG=`pwd`
# Something changed -> we delete all
# Sometime TRUST_CC changes and produces recursive cmake so the last condition IS important
Build=$TRUST_ROOT/build/Outils/lata_tools
if [ build.sh -nt $Build ] || [ CMakeLists.txt -nt $Build ] || [ "`grep $TRUST_CC $Build/CMakeCache.txt`" = "" ]
then
   rm -r -f $Build
fi
mkdir -p $Build
cd $Build
# Better detection of SWIG on Ubuntu 16
SWIG_EXECUTABLE=`type -p swig`
# PL inutile de specifier swig (dans PATH normalement):
cmake $ORG -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/lata_tools_install -DCMAKE_C_COMPILER=$TRUST_cc -DCMAKE_CXX_COMPILER=$TRUST_CC -DSWIG_EXECUTABLE=$SWIG_EXECUTABLE
make -j  $TRUST_NB_PROCS  install || exit -1

echo "Check if compare_lata works"
tar xzf $TRUST_ROOT/Outils/lata_tools/src/tools/vdf.tar.gz
$TRUST_ROOT/exec/lata_tools_install/bin/compare_lata vdf/vdf.lata vdf/vdf.lata || exit -1
cd ..
rm -rf $Build
