#!/bin/bash
[ "$*" = "" ] && echo usage $0 files && exit -1

DIR=`dirname $0`
DIR=`cd $DIR;pwd`

for f in $*
do
   chmod +w $f
   cp $f $f.sa
   sed  -f $DIR/sedfile_prm $f.sa > $f

   echo "Modifications on $f" 
   diff $f.sa $f
   rm -f $f.sa
done

