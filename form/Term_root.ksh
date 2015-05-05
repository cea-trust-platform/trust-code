#!/bin/bash
if [ ! -z $DIPHA_ROOT ]
then
   cd $DIPHA_ROOT
else
   cd $TRUST_ROOT
fi
# Bizarre sur une Sun, xterm depuis netscape ne marche pas
# a cause du test sur MA_VUE...
# Sur DEC aussi ! ${#MA_VUE} remplace par $MA_VUE
if [ ${MA_VUE:=vide} != "vide" ]
then
   $Dtterm $DroitCentre_geometrie -title "Vue: $MA_VUE"
else
   $Dtterm $DroitCentre_geometrie -title "Terminal TRUST" &
fi
