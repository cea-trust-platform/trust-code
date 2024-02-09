#!/bin/bash

# Check a single dataset with the trustpy tool

echo 'Usage: check_single.sh </the/dataset/path/jdd.data>'

org=$TRUST_ROOT/Outils/trustpy
generated_py=$org/install/generated/trustpy_gen.py

# Checking module has been generated
if [ ! -f "$generated_py" ]; then
  echo "Could not find $generated_py!! Is the 'trustpy' tool correctly installed?"
  exit -1
fi

# Checking dataset existence
jdd=$1
if [ ! -f $jdd ]; then
  echo "Could not find $jdd!!"
  exit -1
fi

# Sourcing
source $org/env.sh

# Executing!
env GEN_MOD=$generated_py python $org/src/tst_rw_one_dataset.py $1 || exit -1
