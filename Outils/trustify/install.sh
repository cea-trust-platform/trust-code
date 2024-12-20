#!/bin/bash

# Exit on error:
set -e

org=$TRUST_ROOT/Outils/trustify
install_dir=$org/install
PYTHON_VERSION=`python -c "import sys; print(str(sys.version_info.major)+'.'+str(sys.version_info.minor))"`
full_install_dir=$install_dir/lib/python$PYTHON_VERSION/site-packages

# Are we invoked from a baltik, or from TRUST?
if [ "$project_directory" = "" ] || [ "$TRUSTIFY_FROM_TRUST" = 1 ]; then  # from TRUST
    generated_TRAD2=$install_dir/generated/TRAD2_trustify
    generated_pyd=$install_dir/generated/trustify_gen_pyd.py
    generated_pars=$install_dir/generated/trustify_gen.py

    mkdir -p $install_dir
    cd $install_dir

    [ ! -f $archive ] && echo $archive no such file && exit 1

    from_TRUST=1
else   # From a baltik
    build_dir=$project_directory/build/trustify
    rm -rf $build_dir
    mkdir -p $build_dir $build_dir/generated
    generated_TRAD2=$build_dir/generated/TRAD2_trustify
    generated_pyd=$build_dir/generated/trustify_gen_pyd.py
    generated_pars=$build_dir/generated/trustify_gen.py
    agg_trad2=$build_dir/generated/agg_TRAD_2.org
    from_TRUST=0
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Sourcing environment"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
source $org/env.sh

echo "OK"

if [ "$from_TRUST" = "1" ]; then
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    echo "@@@ Installing trustify Python module"
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    pip install .. --prefix $install_dir
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

# Set Python path properly:
export PYTHONPATH=$full_install_dir:$PYTHONPATH

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating TRAD2 (from (agg)TRAD2.org files and C++ sources)"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
mkdir -p $(dirname $generated_TRAD2)
python -c "import trustify.trad2_utilities as t2u; t2u.do_main()" $generated_TRAD2
echo "Done."
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating Python modules (pydantic and parser) from TRAD2"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
python -c "import trustify.trad2_pydantic as t2p; t2p.do_main()" $generated_TRAD2 $generated_pyd $generated_pars
echo "Done."
