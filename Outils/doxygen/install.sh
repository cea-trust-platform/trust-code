#!/bin/bash

#
# Installation script for doxygen from sources
#

version=1.9.3
echo "installing doxygen-$version"
mkdir -p $TRUST_ROOT/build/Outils/doxygen_build
cd $TRUST_ROOT/build/Outils/
tar xfz $TRUST_ROOT/externalpackages/doxygen-$version.src.tar.gz
cd doxygen_build
install_dir=$TRUST_ROOT/exec/doxygen

cmake  -G "Unix Makefiles" ../doxygen-$version -DCMAKE_INSTALL_PREFIX=$install_dir -DCMAKE_BUILD_TYPE=Release
err=0
$TRUST_MAKE || err=1
make install || err=1

if [ $err = 0 ]
then
   rm -rf $TRUST_ROOT/build/Outils/doxygen*
else
   #
   # if install from sources fails (for ex. gcc 4.8.5), we install doxygen-1.7.4
   #
   version=1.7.4
   rm -rf $TRUST_ROOT/build/Outils/doxygen*
   rm -rf $TRUST_ROOT/exec/doxygen
   echo "We install doxygen-$version..."
   tar xfz $TRUST_ROOT/externalpackages/doxygen-$version.linux.bin.tar.gz -C $TRUST_ROOT/exec
   mv $TRUST_ROOT/exec/doxygen-* $TRUST_ROOT/exec/doxygen
   touch $TRUST_ROOT/exec/doxygen/bin/doxygen
fi

HTML=$TRUST_ROOT/doc/html
if [ -d $HTML ] && [ "`grep doxygen $HTML/index.html | grep $version 2>/dev/null`" = "" ]
then
   echo "Doxygen version changed, we clean html documentation of TRUST inside $HTML"
   rm -r -f $HTML
else
   exit 0
fi

