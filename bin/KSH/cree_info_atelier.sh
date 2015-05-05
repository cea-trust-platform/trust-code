#!/bin/bash
res=`mktemp_`
dest=info_atelier.cpp
files=`\ls $rep_dev/*.cpp $rep_dev/*.f $rep_dev/*.c $rep_dev/*.h 2>/dev/null`
echo "//////////////////////////////////////////////////////////////////////////////
//
// File:        info_atelier.cpp
// Directory:   \$TRUST_ROOT/src/MAIN
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

// provisoire fichier genere par cree_info_atelier.sh 

#include <info_atelier.h>
#include <Sortie.h>
#include <EntreeSortie.h>
int info_atelier(Sortie& os)
{" >$res
echo "if (0) os<<\"date of generation of this file: `date`\"<<finl;" >> $res
nb=0
nbnew=0
for f in $files
  do
  ff=`basename $f`
  ftrio=`Find $ff`
  if [ "$ftrio" != "" ]
      then
      nb=`echo $nb+1|bc`
      echo "os<<\" $f modified the standard version \"<< finl;" >> $res
      else
      nbnew=`echo ${nbnew}+1|bc`
  fi
done
echo "os<<finl<< \" $nb file(s) modified \" <<finl<< finl;" >>$res
echo "os<<finl<< \" $nbnew file(s) created \" <<finl<< finl;" >>$res

echo "return "$nb";
}
// Description: permet de stocker l'atelier et de le reecrire
void restore_atelier(Sortie& os)
{
}" >> $res


if [ ! -f $dest ]
then
    cat $res > $dest
    echo $dest updated.
else
    p1=`mktemp_`
    p2=`mktemp_`
    grep -v "date of generation" $res > $p1
    grep -v "date of generation" $dest > $p2
    if [ "`diff $p1 $p2`" != "" ]
    then
	cp -f $res $dest
	echo $dest updated.
    fi
    rm -f $p1 $p2
fi
rm -f $res
exit 0
