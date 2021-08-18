#!/bin/bash

#env_med=${TRUST_MEDCOUPLING_ROOT}/env.sh
#[ -f $env_med ] && . $env_med
export MED_COUPLING_ROOT=$TRUST_MEDCOUPLING_ROOT
ORG=`pwd`
# Fixed by use of cmake 3.0.1 so commented (furthermore, killall cmake was a bad idea: what if another build during the same time?)
# recursive cmake killed:
#if [ "`ps -efl | grep cmake | grep -v grep`" != "" ] && [ "`ps -efl | grep new_build.sh | grep -v grep`" != "" ]
#then
#   killall cmake
#   rm -r -f build
#fi
# Something changed -> we delete all
# Sometime TRUST_CC changes and produces recursive cmake so the last condition IS important
Build=$TRUST_ROOT/build/Outils/lata2dx
if [ new_build.sh -nt $Build ] || [ CMakeLists.txt -nt $Build ] || [ "`grep $TRUST_CC $Build/CMakeCache.txt`" = "" ]
then
   rm -r -f $Build
fi
mkdir -p $Build
#mkdir -p $TRUST_ROOT/exec/lata2dx
cd $Build
# GF ajout de -DCMAKE_CXX_COMPILER pour bien prendre le compilo choisi par trio 
#cmake $ORG -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/lata2dx_install -DCMAKE_C_COMPILER=$TRUST_cc -DCMAKE_CXX_COMPILER=$TRUST_CC -DSWIG_EXECUTABLE=$TRUST_ROOT/exec/python/bin/swig
# PL inutile de specifier swig (dans PATH normalement):
cmake $ORG -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/lata2dx_install -DCMAKE_C_COMPILER=$TRUST_cc -DCMAKE_CXX_COMPILER=$TRUST_CC
make -j  $TRUST_NB_PROCS  install || exit -1
cd ..
rm -rf lata2dx
##
## Test de fonctionnement
##
#source $Build/env.sh
#python -c "import LataLoader" 
#if [ $? -eq 0 ]
#then
#    echo "LataLoader library OK"
#else
#    echo "LataLoader library KO"
#    exit -1
#fi
