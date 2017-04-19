#!/bin/bash


# toutes les variables doivent exister
set -u

add=0
update=0
check=0
mode="auto"
APP="$*"

LICENSE_SRC=$TRUST_ROOT/License.txt
mkdir -p $TRUST_ROOT/build
LICENSE=$TRUST_ROOT/build/License.txt
cat $LICENSE_SRC > $LICENSE

[ "$1" = "--mode" ] && shift && mode=$1 && shift 

type=$1
[ "$1" = "-add" ] && add=1 && shift
[ "$1" = "-check" ] && check=1 && shift
[ "$1" = "-update" ] && update=1 && shift
let test_opt=add+update+check
if [ ${test_opt} -ne 1 ]
then
  echo Error wih $0 $APP

  echo  "Usage $0 [ --mode auto|c|py|shell] -add |-check| -update file "
  exit 1
fi
check_file()
{
file=$1

if [ $add -eq 1 ]
then
  # pas de license avant
  [ "`grep Copyright $file`" != "" ] && echo Already a license in $file && return 1
   mod=$mode
   [ ! -f  ${LICENSE}_$mod ] && echo  ${LICENSE}_$mod  inexistant && return 1
   if [ "$mod" = "py" ]
       then
       cp  $file $file.sa
       awk 'BEGIN {com=1} { if (com==1) { if (substr($0,1,1)=="#" ) { print $0} else {com=0} } }' $file.sa > $file 

       cat ${LICENSE}_$mod >> $file
       awk 'BEGIN {com=1} { if (com==1) { if (substr($0,1,1)!="#" ) { com=0}} ; if (com==0) {print $0} }  ' $file.sa >> $file 
       rm $file.sa
   else
       
       cp $file $file.sa
       cat ${LICENSE}_$mod $file.sa > $file
       rm $file.sa
   fi
else
  # il ya bien une  license pour check/update ?
  [ "`grep Copyright $file`" = "" ] && echo  No license in $file && return 1
fi
}
if  [ ! -f ${LICENSE}_c  ] || [  ${LICENSE}_c -ot  ${LICENSE} ] 
then
awk 'BEGIN {print ("/****************************************************************************")} 
    { if ($0=="") {print ("*") } else {print ("* "$0) }}
    END    {print ("*****************************************************************************/")}' ${LICENSE}  >  ${LICENSE}_c

awk 'BEGIN {print ("#****************************************************************************")} 
    { print ("# "$0)}
    END    {print ("#*****************************************************************************")}' ${LICENSE}  >  ${LICENSE}_py
fi
for f in $*
do
#echo $f
check_file $f
#echo $f
done
