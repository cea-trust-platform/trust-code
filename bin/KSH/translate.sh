#!/bin/bash
# Translation of data files based on dictionary:
upper()
{
   echo $1 | awk '{print toupper(substr($1,1,1))""substr($1,2,length($1)-1)}'
}
echo "Usage: `basename $0` [datafile.data]"
fix=no && [ "${1#-fix}" != "$1" ] && fix=$1 && shift
files=$*
# Create a ordonned dictionnary with only the validated translation (without XXX)
dictionary=`mktemp_`
awk -F"|" '{if ($1!="" && gsub("XXX","",$1)==0) print length($2)" "$0}' $TRUST_ROOT/doc/TRUST/Keywords.txt | sort -nr | awk '{print $2}' > $dictionary

MonoDir=$TRUST_ROOT/MonoDir_mpi_opt && [ ! -d $MonoDir ] && monodir
tmp=`mktemp_`
# Check the sources with the dictionary (we start with the longest keywords to avoid some obvious problems...)
for french in `cat $dictionary | awk -F"|" '{print $2}'`
do
   english=`awk -F"|" -v french=$french '($2==french) {print $1}' $dictionary`
   # Xdata
   #xdata=`grep -i "[ ^]$french " $TRUST_ROOT/Outils/TRIOXDATA/XTriou/TRAD_2 | wc -l`
   #if [ $xdata != 1 ]
   #then
   #   echo "No line found in TRAD_2 for keyword $french."
   #   exit
   #fi

   # Sources:
   # Replace "$french" by "$french|$english" 
   # We must take care of a.typer("$french");
   # and replace only for:
   # instanciable(...,"$french",...)
   # param.ajouter("$french");
   # if (a=="$french") ...
   #motcle='grep -i \"$french\" $MonoDir/src/*.cpp $MonoDir/include/*.h | grep -v "typer(" | grep -e "\.ajouter(" -e "if (" -e _instanciable'
   motcle='find $TRUST_ROOT/src -name '*'.cpp -or -name '*'.h | xargs  grep -i \"$french\" | grep -v "typer(" | grep -e "\.ajouter(" -e "if (" -e _instanciable'
   keyword=`find $TRUST_ROOT/src -name '*'.cpp | xargs grep -i -e \"$french\" -e \"$french\|  | grep _instanciable`
   if [ "$keyword" != "" ] || [ $english = end ]
   then
      # First letter is upper for some keywords (interpretors):
      English=`upper $english`
      sed -i "s/$english/$English/" $dictionary
      english=$English
      french=`upper $french`
   fi
   #instanciable='grep -i -e "_"$french"_" -e "_"$french\" -e \"$french"_" $MonoDir/src/*.cpp $MonoDir/include/*.h | grep -i "_instanciable"'
   instanciable=""
   nb_motcle=`eval $motcle | wc -l`
   nb_instanciable=`eval $instanciable | wc -l`
   if [ "$nb_motcle" = 0 ] && [ "$nb_instanciable" = 0 ]
   then
      echo "-> No source files found for keyword=$french" # C'est normal pour certains
   else
      echo "--------------------------------------------"
      if [ "$nb_instanciable" = 1 ]
      then
	 echo "-> Several files found for keyword=$french :"
	 for file in `eval $instanciable | awk -F":" '{print $1}' | sort -u`
	 do       
	    echo "-> File `basename $file` found for keyword=$french :"
            sed -e "s/\(_instanciable.*\)_"$french"_/\1_"$english"_/I" \
		-e "s/\(_instanciable.*\)_"$french"\"/\1_"$english"\"/I" \
		-e "s/\(_instanciable.*\)\""$french"_/\1\""$english"_/I" $file > $tmp
            diff $file $tmp 
	    [ $fix = -fix_sources ] && cat $tmp > $file
	 done
      fi
      if [ "$nb_motcle" != "" ]
      then
         echo "-> Several files found for keyword=$french :"
	 for file in `eval $motcle | awk -F":" '{print $1}' | sort -u`
	 do
	    echo `basename $file`
	    sed "s/\""$french"\"/\""$french"|"$english"\"/I" $file > $tmp
            diff $file $tmp 
	    [ $fix = -fix_sources ] && cat $tmp > $file
	 done
      fi
   fi
done
echo
echo "Return to see data files now or enter -fix_sources to fix the sources:"
read return
[ "$return" != "" ] && $0 $return
# Data files
# Il faut gerer les espaces et les tabs avec \([ \t]\) et \1 et \2 pour bien retrouver " " resp \t a la place de " " resp de \t 
# Il faut gerer les debuts et fin de lignes avec ^ et $
# Il faut gerer la casse avec I dans /old/new/I
translation=`mktemp_`
# Translate old marks to new one:
echo "s/DEBUT MAILLAGE/BEGIN MESH/" >> $translation
echo "s/FIN MAILLAGE/END MESH/" >> $translation
echo "s/DEBUT DECOUPAGE/BEGIN PARTITION/" >> $translation
echo "s/FIN DECOUPAGE/END PARTITION/" >> $translation
echo "s/DEBUT LECTURE/BEGIN SCATTER/" >> $translation
echo "s/FIN LECTURE/END SCATTER/" >> $translation
# Add translation french -> english
awk -F"|" '{print "s/\\([ \\t]\\)"$2"\\([ \\t]\\)/\\1"$1"\\2/I\ns/^"$2"\\([ \\t]\\)/"$1"\\1/I\ns/\\([ \\t]\\)"$2"$/\\1"$1"/I\ns/^"$2"$/"$1"/I"}' $dictionary >> $translation || exit -1
#nedit $translation

# If data files not given, parse recursively the data files...
if [ "$files" = "" ]
then
   files=`find . -name '*'.data`   
fi

[ "$fix" = "no" ] && fix="-print"

while [ "$fix" != "" ]
do
   for file in $files
   do
      if [ -f $file ]
      then
	 CHECKOUT $file 1>/dev/null 2>&1
	 sed -f $translation $file > $tmp
	 if [ "`diff $file $tmp`" != "" ]
	 then
            if [ $fix = -fix_datafiles ]
	    then
	       echo "Translating $file done."
	       cat $tmp > $file
	       # Run lance_test to check immediatly the translation:
	       if [ -f ${file%.data}.lml.gz ]
	       then
  	          echo `basename $file .data` | lance_test 1>/dev/null 2>&1
	          [ $? != 0 ] && echo "Error with lance_test on `basename $file .data`" && exit -1
	       fi
	    else
	       echo "Translating $file ?"
	       diff $file $tmp 
	    fi
	 fi
      else
	 echo "File $file not found!"
      fi
   done
   if [ "$fix" != -fix_datafiles ]
   then
      echo "Return to exit now or enter -fix_datafiles to fix the datafiles:"
      read fix
   else
      fix=""
   fi
done

rm -f $translation $tmp $dictionary
echo
