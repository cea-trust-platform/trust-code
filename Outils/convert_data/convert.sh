#!/bin/bash

if [ "$TRUST_ROOT" = "" ]; then 
    echo "TRUST env not loaded!"
    exit -1
fi

if [ ! -f "$1" ]; then 
    echo "File "$1" not found!"
    exit -1
fi

echo "Converting dataset '$1' to 1.9.1 syntax ..."
echo ""
echo ""

conv_mil=$TRUST_ROOT/Outils/convert_data/convert_milieu.py
conv_interf=$TRUST_ROOT/Outils/convert_data/convert_interf.py
conv_liremed=$TRUST_ROOT/Outils/convert_data/convert_liremed.py

ze_d=`mktemp_ -d`
echo "*** Converting 'milieu' ..."
python $conv_mil "$1" $ze_d/after_mil.data || cp "$1" $ze_d/after_mil.data

echo ""
echo ""
echo "*** Converting 'interfaces' ..."
python $conv_interf $ze_d/after_mil.data $ze_d/after_interf.data  || cp $ze_d/after_mil.data $ze_d/after_interf.data

echo ""
echo ""
echo "*** Converting 'read_med' ..."

python $conv_liremed $ze_d/after_interf.data $ze_d/afer_liremed.data  || cp $ze_d/after_interf.data $ze_d/after_liremed.data 

cp $ze_d/afer_liremed.data "$1".new
echo ""
echo ""
echo "=> All done! File '$1.new' has been written."

rm -rf $ze_d

