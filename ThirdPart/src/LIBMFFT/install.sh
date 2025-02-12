#!/bin/bash

#
# Installation script for LIBMFFT
#

set -e # Exit on error

org=$TRUST_ROOT/ThirdPart/src/LIBMFFT
src=$org/src
build=$TRUST_ROOT/build/lib/LIBMFFT

rm -rf $build
mkdir -p $build
cp -a $src $build/

cd $build/src

if [ "$TRUST_INT64" = "1" ] && [ "$TRUST_INT64_NEW" = "0" ]; then
    echo "Patching for 64b ..."
    cp $org/patch/* $build/src
fi

$TRUST_MAKE   # This will also copy the .a file to teh final directory

# Clean build
rm -rf $build

