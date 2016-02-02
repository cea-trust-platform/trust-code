#!/bin/bash
# 

# on regarde ce que contient externalpackages sans tenir compte du RELEASE_NOTES
(cd $TRUST_ROOT/externalpackages; md5sum * */* 2>/dev/null | sort -k 2 | grep -v RELEASE_NOTES | sed "s/ \*/  /") > md5.check

echo "Controm md5 of externalpackages"
diff md5.ref md5.check
ko=$?
echo
echo Control of liste_dep.csv
# on verifie que les fichiers sont bien indiqus dans liste_dep.csv
for f in `awk '{print $2}' md5.check`
 do
 [ "`grep $(basename $f) liste_dep.csv`" = "" ] && echo $f not in  liste_dep.csv
done




sed "s/General/./"  liste_dep.csv  | awk -F, '{print "[ ! -f $TRUST_ROOT/externalpackages/"$1"/"$2" ]  && echo "$2" not in externalpackages " }' | grep -v "dir/paquet" > cmd_test
sh cmd_test

	


for f in `awk '{print $2}' md5.check`
 do
 [ "`grep $(basename $f) liste_dep.csv`" = "" ] && echo $f not in  liste_dep.csv
done	
rm cmd_test 
[ $ko -eq 0 ] && rm md5.check
