#!/bin/bash

rm -rf build
rm -rf install

mkdir -p build


BUILD_MODE=Release
if [ "x$BUILD_TYPE" != "x" ]
then
   BUILD_MODE=$BUILD_TYPE && OPT="" && [ "$BUILD_TYPE" = "semi_opt" ] && OPT="_"$BUILD_TYPE
fi

echo Sourcing MEDCoupling
. $TRUST_MEDCOUPLING_ROOT/env.sh

cd build
CXXFLAGS="-Wno-error" cmake .. -DCMAKE_BUILD_TYPE=$BUILD_MODE -DCMAKE_INSTALL_PREFIX=../install

make -j install || exit -1

cd ..

