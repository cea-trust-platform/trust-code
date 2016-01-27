#!/bin/bash


DIR=`dirname $0`
DIR=`(cd $DIR;pwd)`
export PLOT2D_ROOT_DIR=$DIR/
export PYTHONPATH=$PLOT2D_ROOT_DIR/lib/python2.7/site-packages/salome:$PYTHONPATH

python $PLOT2D_ROOT_DIR/bin/salome/tests/DynamicPlot.py $*
