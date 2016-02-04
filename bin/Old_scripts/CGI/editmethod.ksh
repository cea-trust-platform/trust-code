#!/bin/bash
[ "$UTILISE_APPACHE" = "1" ] && . $HOME/HTML/recharger_env
ORDRE=$1
ORDRE=`echo $ORDRE | sed "s/WX/ /g"| sed "s?:? :?"| sed "s?(? ( ?"|sed "s?)? ) ?"`
ficb=`echo $ORDRE | awk '{for(i=2;i<NF+1;i++) {if ($i=="::") {print ($(i-1))}}}'`".html"
fic2=`extrait_nom doc/$ficb`

str="\.cpp"
[ "`echo $ORDRE | grep inline`" != "" ] && str="\.h"
files=`grep Edit.cg $fic2 | grep $str | awk -F "Edit.cg" '{print $2}' | awk -F\" '{print $1}' | sed "s/\?//" | sed "s/Dipharoot//"`
#exit
#files=`grep Edit.cg $fic2 | grep $str | awk '{print ($NF)}'`
for file in $files
do
file2=`extrait_nom $file`
#echo on cherche $ORDRE dans $file2
#grep "$ORDRE"  $file2
name="grep -n \"::\" $file2"

for str in $ORDRE
do
name=$name" | grep \""$str"\""
done
ORDRE2=`echo $ORDRE | sed "s/ //g"`
name="grep -n \"::\" $file2| sed \"s/ //g\" | grep \""$ORDRE2"\""
res=`eval $name `
#echo ok $res
n=0
n=`eval $name | wc -l | awk '{print $1}'`
[ $n -ge 2 ] && res=`eval $name | grep -v const` && n=`eval $name | grep -v const| wc -l | awk '{print $1}'`
[ $n == 1 ] && break
[ $n -ge 2 ] && xterm -name "pb plusieurs choix $res"  && exit
done
ligne=`echo $res | awk -F: '{print $1}'`
cmd="vi  +"$ligne" "$file2
ires=`xterm  -e $cmd `
