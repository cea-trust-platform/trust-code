#!/bin/bash

rm -rf build
rm -rf install

mkdir -p build

echo Sourcing MEDCoupling
. $TRUST_MEDCOUPLING_ROOT/env.sh

cd build

cmake .. -DCMAKE_CXX_FLAGS_RELEASE="" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install 

make -j install || exit -1

cd ..
rm -rf build

