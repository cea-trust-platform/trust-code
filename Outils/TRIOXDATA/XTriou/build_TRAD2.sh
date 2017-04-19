#!/bin/bash
DIR=`pwd`
ROOT=`(cd $DIR/..;pwd)`
ROOT=`basename $ROOT`
if [ "$ROOT" = "TRIOXDATA" ]
    then 
    mv -f TRAD_2 TRAD_2.sa
    python   Extract_xdata.py --src=$TRUST_ROOT/src 
    if [ "`diff TRAD_2 TRAD_2.sa 2>&1`" != "" ]
    then
      echo TRAD_2 updated
      rm -f TRAD_2.sa
    else
      # pour la date
      mv -f TRAD_2.sa TRAD_2
      # mais non
      touch TRAD_2
    fi
else
    python   Extract_xdata.py --src=$project_directory/build/src
fi
