#!/bin/bash

rm -rf build
rm -rf install

mkdir -p build

echo Sourcing MEDCoupling
. $TRUST_MEDCOUPLING_ROOT/env.sh

cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install -DSWIG_EXECUTABLE=$TRUST_ROOT/exec/python/bin/swig

make -j install

cd ..
rm -rf build

