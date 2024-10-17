#!/bin/bash
set -e

# Check all datasets with trustify

org=$TRUST_ROOT/Outils/trustify
# Python version:
py_vers=`python -c "import sys; print('%d.%d' % (sys.version_info.major, sys.version_info.minor))"` 

if [ "$project_directory" = "" ] || [ "$TRUSTIFY_FROM_TRUST" = 1 ]; then  # from TRUST
    generated_py=$org/install/generated/trustify_gen.py
else
    generated_py=$project_directory/build/trustify/generated/trustify_gen.py
fi

# Checking module has been generated
if [ ! -f "$generated_py" ]; then
  echo "Could not find $generated_py!!"
  echo "Is the 'trustify' tool correctly installed? And have you run 'make trustify' in your baltik?"
  exit -1
fi

# Sourcing
source $org/env.sh

# Executing!
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Testing all TRUST datasets for read/write with the generated Python module"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
env GEN_MOD=$generated_py python $org/test/test_rw_all_datasets.py
