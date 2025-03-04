#!/bin/bash

rm -rf build
rm -rf install

mkdir -p build

echo Sourcing MEDCoupling
. $TRUST_MEDCOUPLING_ROOT/env.sh

cd build

CXXFLAGS="-Wno-error" cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install

#VERBOSE=1 make -j install || exit -1
make -j install || exit -1
ctest --output-on-failure || exit -1

cd ..
rm -rf build

