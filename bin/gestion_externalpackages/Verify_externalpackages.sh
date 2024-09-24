#!/bin/bash
# 

# on regarde ce que contient externalpackages sans tenir compte du RELEASE_NOTES
(cd $TRUST_ROOT/externalpackages; md5sum * */* */*/* */*/*/* 2>/dev/null | sort -k 2 | grep -v RELEASE_NOTES | grep -v conda_list_ref | grep -v README_Miniforge | grep -v channeldata | grep -v repodata.json | grep -v index.html | grep -v repodata_from_packages.json | sed "s/ \*/  /") > md5.check

echo "Control of md5 sum of externalpackages"
diff md5.ref md5.check
ko=$?
#echo
echo Control of liste_dep.csv
# on verifie que les fichiers sont bien indiques dans liste_dep.csv
for f in `awk '{print $2}' md5.check`
 do
 [ "`grep $(basename $f) liste_dep.csv`" = "" ] && echo $f not in  liste_dep.csv
done




sed "s/General/./g;s/MiniforgeLocChannLinux/Miniforge\/local_channel\/linux-64/g; s/MiniforgeLocChannNoarch/Miniforge\/local_channel\/noarch/g;s/MiniforgeLocChann/Miniforge\/local_channel/g"  liste_dep.csv  | awk -F, '{print "[ ! -f $TRUST_ROOT/externalpackages/"$1"/"$2" ]  && echo "$2" not in externalpackages " }' | grep -v "dir/paquet" > cmd_test
sh cmd_test

	


for f in `awk '{print $2}' md5.check`
 do
 [ "`grep $(basename $f) liste_dep.csv`" = "" ] && echo $f not in  liste_dep.csv
done	
rm cmd_test 
[ $ko -eq 0 ] && rm md5.check
