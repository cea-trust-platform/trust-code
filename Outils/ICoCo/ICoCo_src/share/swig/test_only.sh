#!/bin/bash
ze_dir=`readlink -f $(dirname $0)`

source $TRUST_MEDCOUPLING_ROOT/env.sh

export PYTHONPATH=$ze_dir/install/lib:$PYTHONPATH

cd build/test
echo "Testing Version 1 ..."
python test_trusticoco.py 1>v1.log 2>&1
if ! [ $? -eq 0 ]; then
    echo Failed!
    exit 255
fi

#echo "Testing Version 2 ..."
#python test_trusticoco_V2.py 1>v2.log 2>&1
#if ! [ $? -eq 0 ]; then
#    echo Failed!
#    exit 255
#fi

echo All OK.

