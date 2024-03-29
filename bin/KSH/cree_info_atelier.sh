#!/bin/bash
res=`mktemp_`
dest=info_atelier.cpp
files=`\ls $rep_dev/*.cpp $rep_dev/*.f $rep_dev/*.c $rep_dev/*.h $rep_dev/*.tpp 2>/dev/null`
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

py_script="import os,sys
ffs = os.getenv('ze_list').split()
ffs = [os.path.split(p)[1] for p in ffs]
lst = []
tr = os.path.join(os.getenv('TRUST_ROOT'), 'src')
for dirpath, dirnames, filenames in os.walk(tr):
      if filenames:
        lst.extend(filenames)
inter = set(lst).intersection(set(ffs))
print(' '.join(list(inter)))
"

modif_files=`env ze_list="$files" python -c "$py_script"`

for f in $modif_files; do
    nb=`echo $nb+1|bc`
    echo "os<<\"File '$f' overrides the standard TRUST version \"<< finl;" >> $res
done

echo "os<<finl<< \" $nb file(s) modified \" <<finl<< finl;" >>$res
#echo "os<<finl<< \" $nbnew file(s) created \" <<finl<< finl;" >>$res

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
