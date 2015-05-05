#!/bin/bash
# Script pour analyser le disque
# en terme d'espace ou de vitesse
space()
{
   # Detection espace libre
   space=`df -k . | awk '/\// && /%/ {print int($(NF-2)/1024/1024)}'`
   if [ "$space" != "" ] && [ "`echo $space $space_need | awk '{print $1<$2}'`" = 1 ]
   then
      echo "========================================================="
      echo "You need $space_need GBytes to install and compile TRUST"
      echo "and you have only $space GBytes on disc $partition."
      echo "Please, fix the problem and re-install TRUST."
      echo "========================================================="
      exit -1
   else
      [ "$silent" = 0 ] && echo "The disc $partition has $space free GBytes."
   fi
}
test_speed()
{
   # Detection vitesse disque en copiant
   bench=`ls $TRUST_ROOT/externalpackages/petsc/petsc*tar.gz | head -1 2>/dev/null`
   [ "$bench" = "" ] && echo "Error for bench variable in dd.ksh !" && exit -1
   mbytes=`cd $TRUST_ROOT;du -s -m $bench | awk '{print $1}' 2>/dev/null`
   [ "$mbytes" = "" ] && echo "Error for mbytes variable dd.ksh !" && exit -1
   #seconds=`(time -p cp $bench $location/.tmp) 2>&1 | awk '/real/ {print $2}' 2>/dev/null`
   # By default, bash time is used and is more procise than GNU time (/usr/bin/time)
   # See: http://unix.stackexchange.com/questions/70653/increase-e-precision-with-usr-bin-time-shell-command
   #seconds=`(TIMEFORMAT='%4R';time cp $bench $location/.tmp) 2>&1`
   # Improve accuracy with: http://stackoverflow.com/questions/3683434/using-time-command-in-bash-script
   # Time is measured in ns when converted in ms:
   NS="$(date +%s%N)"
   cp $bench $location/.tmp 1>/dev/null 2>&1
   NS="$(($(date +%s%N)-NS))"
   MS="$((NS/1000000))"
   rate=`echo $mbytes $MS | awk '{print int(1000*$1/$2)}' 2>/dev/null`
   [ "$rate" = "" ] && rate=0 # Sometime $rate empty, bug with time or something else ?
}
speed()
{
   rate_need=50
   let nb_test=0
   rate=0
   rm -r -f $location/.tmp
   # Fait n tests
   while [ $rate -lt $rate_need ] && [ $nb_test -lt 5 ]
   do
      let nb_test=$nb_test+1
      test_speed
   done
   rm -r -f $location/.tmp 
   if [ $rate -lt $rate_need ]
   then
      echo "====================================================="
      echo "The disc $partition is too slow to work with TRUST !"
      echo "The data transfer rate is $rate MBytes/s whereas $rate_need MBytes/s is recommended."
      [ "$silent" = 0 ] && echo "Please, fix the problem and re-install TRUST."
      echo "====================================================="
      exit -1   
   else
      [ "$silent" = 0 ] && echo "The disc $partition where is located $location has a transfer rate of $rate MBytes/s."
   fi
}
# Pa defaut 2 Go
space_need=2 	&& [ "$1" = "-space_need" ] && shift && space_need=$1 	&& shift
location=`pwd` 	&& [ "$1" = "-speed" ]      && shift && [ "$1" != "" ] && location=$1 	&& shift
silent=0	&& [ "$1" = "-silent" ]	    && silent=1			&& shift
partition=`df -k . | awk '/\// && /%/ {print $NF}'`
[ "$1" != -speed ] && space
[ "$1" != -space ] && speed
exit 0
