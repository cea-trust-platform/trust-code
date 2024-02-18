#!/bin/bash
set -e

# Check all datasets trustpy tool

org=$TRUST_ROOT/Outils/trustpy

if [ "$project_directory" = "" ] || [ "$TRUSTPY_FROM_TRUST" = 1 ]; then  # from TRUST
    generated_py=$org/install/generated/trustpy_gen.py
else
    generated_py=$project_directory/build/trustpy/generated/trustpy_gen.py
fi

# Checking module has been generated
if [ ! -f "$generated_py" ]; then
  echo "Could not find $generated_py!! Is the 'trustpy' tool correctly installed? Or have you run 'make trustpy'?"
  exit -1
fi

# Sourcing
source $org/env.sh

# Executing!
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Testing all TRUST datasets for read/write with the generated Python module"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
env GEN_MOD=$generated_py python $org/install/trustpy/tst_rw_all_datasets.py
