#!/bin/bash
# Exit on error:
set -e

org=$TRUST_ROOT/Outils/trustify

if [ "$project_directory" = "" ] || [ "$TRUSTIFY_FROM_TRUST" = 1 ]; then  # from TRUST
    install_dir=$org/install
else  # from a BALTIK
    install_dir=$project_directory/build/trustify
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Sourcing environment"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
source $org/env.sh
echo "OK"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating doc"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
mod=$install_dir/generated/trustify_gen.py
#mod=/export/home/adrien/Projets/TRUST/TRUST_LOCAL_fourth/Outils/trustify/test/generated/TRAD_2_adr_simple_pars.py
doc_dir=$install_dir/doc

rm -rf $doc_dir
mkdir -p $doc_dir

echo "Using module '$mod'"
$org/tools/gen_doc.py $mod $doc_dir
echo "OK, generated in '$doc_dir'"
