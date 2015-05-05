#!/bin/bash
batch=0
. `dirname $0`/environ.sh
[ "$1" != "" ] && batch=1
if [ $batch -eq 0 ]
then
xterm -e runSalome --modules=TRIOU,VISU,MED,GEOM,SUPERV,SHELLCOMPO,DISTRIBUTOR,SMESH,DICO
else
#python `dirname $0`/batch.py
python $1
fi
