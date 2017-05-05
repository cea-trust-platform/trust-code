#!/bin/bash
[ ${#TRUST_ROOT} = 0 ] && echo "TRUST environment not initialized. Go to the directory of TRUST installation and run: source ./env_TRUST.sh" && exit

Build=$TRUST_ROOT/build/bin/gestion_externalpackages/Test
[ ! -d ${Build} ] && echo Creating ${Build} directory && mkdir -p ${Build}
cd $Build

$TRUST_ROOT/bin/gestion_externalpackages/get_files.sh > cmds
sh cmds


md5sum -c $TRUST_ROOT/bin/gestion_externalpackages/md5.ref | grep -v OK 

exit
for f in `awk '{print $2}' $TRUST_ROOT/bin/gestion_externalpackages/md5.ref `
do

res=`md5sum -c $TRUST_ROOT/bin/gestion_externalpackages/md5.ref $f`
if [ $? -ne 0 ] 
then
echo $f Pb ?
fi
done
