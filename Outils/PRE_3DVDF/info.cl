#!/bin/bash
echo $*
cas=`basename $2 .prep`
f2=$1/noms_des_bords$cas
if [ -f $f2 ]
then 
   echo "WARNING "$f2" is used to define boundaries name"
   echo "only lines which containes \"bord \" are useful (with cap b) "
   echo "the fields must be separeted by ,"
   echo "previous last (avant dernier) corresponding to the boundary condiction index (ex -1000)"
   echo "the last corresponding to the boundary condition name (20 first caracters are retained)"
   cat $f2
else 
   listind="`grep "INDMAT=" $1/appel.h | awk -F= '{if ($2 <0)  {print ($2)}}'|sort -n -u`"
   listind="$listind 0"
   listind=`echo $listind | sed "s/ 0 0/ 0/"`
   #echo $listind
   i=1
   nlavtdec=`grep -n "SEPDEC 1" $1/$2|awk -F: '{print $1}'`
   if [ "$nlavtdec" != "" ]
   then
      head -$nlavtdec $1/$2 > $1/prov.dfdf
      #echo $nlavtdec
   else
      cat $1/$2 > $1/prov.dfdf
   fi
   for cl in $listind
   do
      res=`grep " $cl " $1/prov.dfdf | awk '{if ($2!="0") {for (i=3+$2;i<=NF;i++) {printf (" "$i)}}}' `
      if [ $cl -eq 0 ]
      then 
         res="$res defaut"
      fi
      res2=`echo $res | sed "s/ /_/g" | sed "s/\,//"`
      echo "Bord $i :$res, $cl,$res2"
      i="`expr $i + 1`"
   done
   rm $1/prov.dfdf
fi
