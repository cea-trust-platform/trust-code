#!/bin/bash
rep=`cat $HOME/.ETUDE_TRUST`
cd $rep
$Dtterm -title "Xdata" -e EditData `cat .NOM_DATA`.data
