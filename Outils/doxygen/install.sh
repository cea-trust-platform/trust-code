#!/bin/bash

#
# Installation script for doxygen from sources
#

mkdir -p $TRUST_ROOT/build/Outils/
cd $TRUST_ROOT/build/Outils/
tar xfz $TRUST_ROOT/externalpackages/doxygen-1.9.3.src.tar.gz
mkdir doxygen_build
cd doxygen_build
install_dir=$TRUST_ROOT/exec/doxygen-1.9.3

cmake  -G "Unix Makefiles" ../doxygen-1.9.3 -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/doxygen-1.9.3 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LOCAL_ONLY=ON
err=0
$TRUST_MAKE || err=1
make install || err=1
[ $err = 0 ] && rm -rf $TRUST_ROOT/build/Outils/doxygen*
exit $err
