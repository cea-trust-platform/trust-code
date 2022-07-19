#!/bin/bash

ORG=`pwd`
for rep in CURVEPLOT TRUST_PLOT2D
do
  BUILD=$TRUST_TMP/build/$ORG/build_$rep
  BUILD=build_$rep
  rm -rf $BUILD
  rm -rf $TRUST_ROOT/exec/$rep
done
rm -rf $TRUST_ROOT/build/Outils/IHM/

