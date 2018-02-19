#!/bin/bash


DIR=`dirname $0`
DIR=`cd $DIR; pwd`
mkdir provdir
BALTIK=$TRUST_ROOT/bin/baltik
$BALTIK/share/baltik/bin/baltik_make_distrib -compute_dependencies -directory_for_tar `pwd`/provdir

org=`pwd`
Arc=`basename $org` 
cp -r $DIR/* provdir
cd provdir/
cp $Arc.tar.gz sto
rm -f ${Arc}*.tar.gz
mv sto $Arc.tar.gz
cp -ar  $BALTIK .
rm -rf baltik/templates
chmod -R +w .
tar hcf ../$Arc.tar *
cd $org

rm -rf provdir 
echo fin creation de $Arc.tar
