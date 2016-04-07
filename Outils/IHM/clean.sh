#!/bin/bash

ORG=`pwd`
for rep in PLOT2D TRUST_PLOT2D TRUST_WIZARD
do
  BUILD=$TRUST_TMP/build/$ORG/build_$rep
  rm -rf $BUILD
done
