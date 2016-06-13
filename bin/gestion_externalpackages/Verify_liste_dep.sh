#!/bin/bash


mkdir -p Test
cd Test

../get_files.sh > cmds
sh cmds


md5sum -c ../md5.ref | grep -v OK 

exit
for f in `awk '{print $2}' ../md5.ref `
do

res=`md5sum -c ../md5.ref $f`
if [ $? -ne 0 ] 
then
echo $f Pb ?
fi
done
