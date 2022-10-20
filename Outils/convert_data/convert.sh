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

sth_done=0
conv_mil=$TRUST_ROOT/Outils/convert_data/convert_milieu.py
conv_interf=$TRUST_ROOT/Outils/convert_data/convert_interf.py
conv_liremed=$TRUST_ROOT/Outils/convert_data/convert_liremed.py
conv_chf=$TRUST_ROOT/Outils/convert_data/convert_ch_fonc_med.py

ze_d=`mktemp_ -d`

if [ "`file $1 | grep ISO-8859`" != "" ]
then
   echo "*** Converting datafile to utf-8"
   tmpf=`mktemp_`
   iconv -f ISO-8859-15 -t UTF-8//TRANSLIT -o $1 $tmpf || exit -1
   mv -f $tmpf $1
   echo ""
   echo ""
fi

echo "*** Converting 'milieu' ..."
python $conv_mil "$1" $ze_d/after_mil.data || ( cp "$1" $ze_d/after_mil.data && sth_done=1 )

echo ""
echo ""
echo "*** Converting 'interfaces' ..."
python $conv_interf $ze_d/after_mil.data $ze_d/after_interf.data  || ( cp $ze_d/after_mil.data $ze_d/after_interf.data && sth_done=1 )

echo ""
echo ""
echo "*** Converting 'read_med' ..."
python $conv_liremed $ze_d/after_interf.data $ze_d/after_liremed.data  || ( cp $ze_d/after_interf.data $ze_d/after_liremed.data && sth_done=1 )

echo ""
echo ""
echo "*** Converting 'champ_fonc_med' and 'champ_fonc_med_tabule' ..."
python $conv_chf $ze_d/after_liremed.data $ze_d/after_chf.data  || ( cp $ze_d/after_liremed.data $ze_d/after_chf.data && sth_done=1 )

echo ""
echo ""

if [ "$sth_done" = "1" ]; then
    cp $ze_d/after_chf.data "$1".new
    echo "=> All done! File '$1.new' has been written."
else
    echo "=> No change applied. Nothing written."
fi

rm -rf $ze_d

