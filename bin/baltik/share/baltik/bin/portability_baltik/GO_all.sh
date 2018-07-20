#!/bin/bash
org=`pwd`

DIR=`dirname $0`
DIR=`cd $DIR; pwd`

if [ "$1" = "" ] || [ ! -f $1 ] 
then
 echo usage $0 Run.liste 
 exit
fi
list=$1
PRO=`dirname $1`
shift
cd $PRO
tar rvf old_log.tar *log
rm -f *log


echo > cmds
rm -f f_cmd_*
while read -e line 
do

file=`echo $line | awk '{ if ($1=="-extension") {print ("cmd_"$5"_"$4)} else  {print ("cmd_"$3"_"$2)}}'| sed "s?\/?%?g"`  
touch f_$file
echo $DIR/../portability/distrib.sh $line  $* >> f_$file
# ./distrib.sh $line &
# echo sleep 3 >> f_$file
echo sh f_$file \& >> cmds
done < `basename $list` 

[ "`basename $0`" = "NOGO.sh" ] && exit
#ln -sf $org/*.tar
ln -sf $org/$line old_log.tar
touch debut

eval `sort -u cmds `
# eval `awk '{print ("./distrib.sh " $0" &" )}' Run.liste  ` 
wait 
rm cmds f_cmd_*
rm *.tar
proj=`basename $org`
echo "Begin of build_report.sh "$proj
sh $DIR/../portability/build_report.sh $proj
echo "See html log here:" `pwd`/nuit_$proj.html
