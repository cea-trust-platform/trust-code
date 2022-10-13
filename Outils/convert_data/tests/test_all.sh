#!/bin/sh

curr_dir=$(dirname $0)

rm -rf $curr_dir/build
mkdir $curr_dir/build
cd $curr_dir/build

cmake ..

ctest -j $TRUST_NB_PROCS


