#!/bin/bash
# Is the file $1 is managed by an SCM ?
err=0
if [ "$1" = "" ]
then
   err=1
else
   [ ! -f $1 ] && [ ! -d $1 ] && err=1
fi
[ $err = 1 ] && echo "Erreur! Usage: `basename $0` path_to_element" && exit -1

SCM=""
if [ -d $TRUST_ROOT/.git ]
then
   # Git
   rep=`dirname $1`
   if [ "`cd $rep;git-ls-files $1 2>/dev/null`" != "" ]
   then
      SCM=git
   fi
fi
echo $SCM
