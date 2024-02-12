#!/bin/bash

#
# Installation script for LIBMFFT_C
#

set -e # Exit on error

org=$TRUST_ROOT/ThirdPart/src/LIBMFFT_C
src=$org/src
build=$TRUST_ROOT/build/lib/LIBMFFT_C

rm -rf $build
mkdir -p $build
cp -a $src $build/

cd $build/src

if [ "$TRUST_INT64" = "1" ]; then
    echo "Patching for 64b ..."
    cp $org/patch/* $build/src
fi

$TRUST_MAKE   # This will also copy the .a file to teh final directory

# Clean build
rm -rf $build

