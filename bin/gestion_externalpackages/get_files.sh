
DIR=`dirname $0`
DIR=`(cd $DIR;pwd)`
fichier="$DIR/liste_dep.csv"



sed "s/General/./" $fichier |awk -F, '{print "mkdir -p",$1}'  | grep -v "mkdir dir" | sort -u
 

sed "s/General/./" $fichier | awk -F, '{print "[ ! -f "$1"/"$2" ]  &&  wget "$7" -O " $1"/"$2";touch "$1"/"$2 }'  
