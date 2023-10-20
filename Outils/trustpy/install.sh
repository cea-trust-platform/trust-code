#!/bin/bash

org=$TRUST_ROOT/Outils/trustpy
install_dir=$org/install
generated_TRAD2=$install_dir/generated/TRAD2_truspy
generated_py=$install_dir/generated/trustpy_gen.py

archive=$1

mkdir -p $install_dir
cd $install_dir

[ ! -f $archive ] && echo $archive no such file && exit 1

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Sourcing environment"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
source $org/env.sh
echo $PACKAGESPY_ROOT_DIR
echo "OK"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Extracting packagespy"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
tar zxf $archive
echo "OK"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Copying Python stuff from trustpy"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
cp -a $org/src $install_dir/trustpy || exit -1  
echo "OK"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating TRAD2 (from TRAD2.org files and C++ sources)"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
mkdir -p $(dirname $generated_TRAD2)
python $install_dir/trustpy/trustpy/trad2_utilities.py $generated_TRAD2 || exit -1

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating Python module from TRAD2"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
python $install_dir/trustpy/trustpy/trad2_code_gen.py $generated_TRAD2 $generated_py || exit -1

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Testing all TRUST datasets for read/write with the generated Python module"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
env GEN_MOD=$generated_py $org/src/tst_rw_all_datasets.py || exit -1

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ All done! Bye folks."
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

