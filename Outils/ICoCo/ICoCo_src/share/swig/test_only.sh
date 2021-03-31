#!/bin/bash
ze_dir=`readlink -f $(dirname $0)`

source $TRUST_MEDCOUPLING_ROOT/env.sh

export PYTHONPATH=$ze_dir/install/lib:$PYTHONPATH

cd build/test
echo "Testing ICoCo (version 2) ..."
python test_trusticoco.py 1>test_trusticoco.log 2>&1
if ! [ $? -eq 0 ]; then
    echo Failed!
    exit 255
fi

echo All OK.

