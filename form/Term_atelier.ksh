#!/bin/bash
cd $rep_dev
if [ ${MA_VUE:=vide} != "vide" ]
then
   $Dtterm -title "Vue: $MA_VUE"
else
   $Dtterm -title "Terminal TRUST" &
fi
