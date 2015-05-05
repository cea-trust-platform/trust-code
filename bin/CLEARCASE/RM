#!/bin/bash
liste=$*
for file in $liste
do
   rep=`pwd`
   cd `dirname $file`
   filename=`basename $file` 
   git tag 1>/dev/null 2>&1
   if [ $? = 0 ]
   then
      # Under Git
      git rm $filename
   fi
   cd $rep
done
rm -f $liste
