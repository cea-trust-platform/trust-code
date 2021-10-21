
DIR=`dirname $0`
DIR=`(cd $DIR;pwd)`
fichier="$DIR/liste_dep.csv"



sed "s/General/./g;s/MinicondaLocChannLinux/Miniconda\/local_channel\/linux-64/g; s/MinicondaLocChannNoarch/Miniconda\/local_channel\/noarch/g;s/MinicondaLocChann/Miniconda\/local_channel/g" $fichier |awk -F, '{print "mkdir -p",$1}'  | grep -v "mkdir dir" | sort -u
 

sed "s/General/./g;s/MinicondaLocChannLinux/Miniconda\/local_channel\/linux-64/g; s/MinicondaLocChannNoarch/Miniconda\/local_channel\/noarch/g;s/MinicondaLocChann/Miniconda\/local_channel/g" $fichier | awk -F, '{print "[ ! -f "$1"/"$2" ]  &&  wget "$7" --no-netrc --no-check-certificate -O " $1"/"$2";touch "$1"/"$2 }' | grep -v "dir/paquet"
#sed "s/General/./" $fichier | awk -F, '{print "[ ! -f "$1"/"$2" ]  &&  curl -L "$7" -o " $1"/"$2";touch "$1"/"$2 }'  
