#!/bin/bash
set -e

# Check a single dataset with the trustpy tool

echo 'Usage: check_single.sh </the/dataset/path/jdd.data>'
echo ""
echo '(to debug the check process itself, you can use: '
echo ">  TRUSTIFY_DEBUG=1 check_single.sh </the/dataset/path/jdd.data>"
echo ""
echo ""

org=$TRUST_ROOT/Outils/trustify

if [ "$project_directory" = "" ] || [ "$TRUSTIFY_FROM_TRUST" = 1 ]; then  # from TRUST
    generated_py=$org/install/generated/trustify_gen.py
else
    generated_py=$project_directory/build/trustify/generated/trustify_gen.py
fi

# Checking module has been generated
if [ ! -f "$generated_py" ]; then
  echo "Could not find $generated_py!!"
  echo "Is the 'trustify' tool correctly installed? Or have you run 'make trustify'?"
  exit -1
fi

# Sourcing
source $org/env.sh

# Executing!
env GEN_MOD=$generated_py python $org/test/test_rw_all_datasets.py $1
