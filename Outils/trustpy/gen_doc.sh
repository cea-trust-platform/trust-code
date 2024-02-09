#!/bin/sh
# Exit on error:
set -e

org=$TRUST_ROOT/Outils/trustpy

if [ "$project_directory" = "" ]; then  # from TRUST
    install_dir=$org/install
else  # from a BALTIK
    install_dir=$project_directory/build/trustpy
fi

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Sourcing environment"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
source $org/env.sh
echo $PACKAGESPY_ROOT_DIR
echo "OK"

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "@@@ Generating doc"
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
mod=$install_dir/generated/trustpy_gen.py
doc_dir=$install_dir/doc

rm -rf $doc_dir
mkdir -p $doc_dir

echo "Using module '$mod'"
$org/src/gen_doc.py $mod $doc_dir 
echo "OK, generated in '$doc_dir'"
