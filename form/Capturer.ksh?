#!/bin/bash
ETUDE_TRUST=`QUELLE_ETUDE.ihm`
export ETUDE_TRUST
cd $ETUDE_TRUST
NOM=`cat $ETUDE_TRUST/.NOM_DATA`
export NOM
meshtvfile=$NOM.meshtv
$Xterm $HautGauche_geometrie -title "Create mpeg movie" -e movie_generate $meshtvfile mpeg
