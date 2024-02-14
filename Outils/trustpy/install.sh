#!/bin/bash

# Exit on error:
set -e

org=$TRUST_ROOT/Outils/trustpy
install_dir=$org/install

# Are we invoked from a baltik, or from TRUST?
if [ "$project_directory" = "" ] || [ "$TRUSTPY_FROM_TRUST" = 1 ]; then  # from TRUST
    generated_TRAD2=$install_dir/generated/TRAD2_truspy
    generated_py=$install_dir/generated/trustpy_gen.py

    archive=$1

    mkdir -p $install_dir
    cd $install_dir

    [ ! -f $archive ] && echo $archive no such file && exit 1
    
    from_TRUST=1
else   # From a baltik
    build_dir=$project_directory/build/trustpy
    rm -rf $build_dir
    mkdir -p $build_dir $build_dir/generated
    generated_TRAD2=$build_dir/generated/TRAD2_truspy
    generated_py=$build_dir/generated/trustpy_gen.py
    agg_trad2=$build_dir/generated/agg_TRAD_2.org
    from_TRUST=0
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Sourcing environment"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
source $org/env.sh
echo $PACKAGESPY_ROOT_DIR
echo "OK"

if [ "$from_TRUST" = "1" ]; then
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo "@@@ Extracting packagespy"
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    tar zxf $archive
    echo "OK"
    
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo "@@@ Copying Python stuff from trustpy"
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    cp -a $org/src $install_dir/trustpy
    echo "OK"
else
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo "@@@ Generating aggregated TRAD2.org"
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    cat $TRUST_ROOT/Outils/TRIOXDATA/XTriou/TRAD_2.org > $agg_trad2
    ta=$project_directory/share/doc_src/TRAD2_ajout0
    if [ -f $ta ]; then
        cat $ta >> $agg_trad2
    fi
    ta=$project_directory/share/doc/TRAD2_ajout0
    if [ -f $ta ]; then
        cat $ta >> $agg_trad2
    fi
    echo "  ---> OK"
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating TRAD2 (from (agg)TRAD2.org files and C++ sources)"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
mkdir -p $(dirname $generated_TRAD2)
python $install_dir/trustpy/trustpy/trad2_utilities.py $generated_TRAD2

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating Python module from TRAD2"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
python $install_dir/trustpy/trustpy/trad2_code_gen.py $generated_TRAD2 $generated_py

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ All done! Bye folks."
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
