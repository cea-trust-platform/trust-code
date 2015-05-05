#!/bin/bash
ETUDE_TRUST=`cat $HOME/.ETUDE_TRUST`
export ETUDE_TRUST
# Ne marche pas bien:
# export ETUDE_TRUST=`cat $HOME/.ETUDE_TRUST` sur SGI 5.3 !!
# ETUDE_TRUST=... not a indentifier
cd $ETUDE_TRUST
if [ ${MA_VUE:=vide} != "vide" ]
then
   $Dtterm -title "Vue: $MA_VUE"
else
   $Dtterm -title "Terminal TRUST" &
fi
