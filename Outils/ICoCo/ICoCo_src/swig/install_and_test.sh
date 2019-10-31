#!/bin/bash

rm -rf build
rm -rf install

mkdir -p build

echo Sourcing MEDCoupling
. $TRUST_MEDCOUPLING_ROOT/env.sh

cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install

make -j install
ctest 

cd ..

