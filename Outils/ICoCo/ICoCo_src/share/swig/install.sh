#!/bin/bash

rm -rf build
rm -rf install

mkdir -p build


BUILD_MODE=Release
MC_ENV=$TRUST_MEDCOUPLING_ROOT/env.sh

if [ "x$BUILD_TYPE" != "x" ]
then
   BUILD_MODE=$BUILD_TYPE && OPT=""
   if [ "$BUILD_TYPE" = "semi_opt" ]
   then
      OPT="_"$BUILD_TYPE
   elif [ "$BUILD_TYPE" = "Debug" ] # Debug mode, we load env_debug.sh of MEDCoupling
   then
      MC_ENV=$TRUST_MEDCOUPLING_ROOT/env_debug.sh
   fi
fi


echo Sourcing MEDCoupling
. $MC_ENV

cd build
CXXFLAGS="-Wno-error" cmake .. -DCMAKE_BUILD_TYPE=$BUILD_MODE -DCMAKE_INSTALL_PREFIX=../install

make -j install || exit -1

cd ..

