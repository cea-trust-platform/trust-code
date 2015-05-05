#!/bin/bash

env_med=${TRUST_MEDCOUPLING_ROOT}/env.sh
[ -f $env_med ] && . $env_med

# Fixed by use of cmake 3.0.1 so commented (furthermore, killall cmake was a bad idea: what if another build during the same time?)
# recursive cmake killed:
#if [ "`ps -efl | grep cmake | grep -v grep`" != "" ] && [ "`ps -efl | grep new_build.sh | grep -v grep`" != "" ]
#then
#   killall cmake
#   rm -r -f build
#fi
# Something changed -> we delete all
# Sometime TRUST_CC changes and produces recursive cmake so the last condition IS important
if [ new_build.sh -nt build ] || [ CMakeLists.txt -nt build ] || [ "`grep $TRUST_CC build/CMakeCache.txt`" = "" ]
then
   rm -r -f build
fi
mkdir build
cd build
# GF ajout de -DCMAKE_CXX_COMPILER pour bien prendre le compilo choisi par trio 
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PWD/../install -DCMAKE_CXX_COMPILER=$TRUST_CC
make -j  $TRUST_NB_PROCS  install
