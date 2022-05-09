#!/bin/bash
# Script d'indentation des jeux de donnees TRUST
# a lancer a la main
# Si un commentaire avec # dans le jdd sur plusieurs lignes, remplacer les balises par /* xxx */ avant d'appeler ce script

echo "Usage:"
echo "   astyle_jdd.sh [-folder folderpath]"
echo "If -folder not specified, astyle_jdd.sh will be run on tests/Reference|Obsolete|Non-validated|EF and Validation/Rapports_automatiques/Verification/"
echo ""
if [ "$1" = -folder ]
then
   shift
   folder=$1
   files=`find $folder -name '*'.data 2>/dev/null`
elif [ "$1" != "" ]
then
   echo "option $1 not supported"
else
   files=`find $TRUST_ROOT/Validation/Rapports_automatiques/Verification/ -name '*'.data`
   files="$files "`find $TRUST_ROOT/tests/Reference -name '*'.data`
   files="$files "`find $TRUST_ROOT/tests/Obsolete -name '*'.data`
   files="$files "`find $TRUST_ROOT/tests/Non-Validated -name '*'.data`
   files="$files "`find $TRUST_ROOT/tests/EF -name '*'.data`
fi

[ "$files" = "" ] && echo "Error: No datafile found" && exit

for file in $files
do
   # Si le fichier source existe
   if [ -f $file ]
   then
      if [ ${file%.data} != $file ] && [ "`grep -i dimension $file`" != "" ]
      then
         # TRUST data file
	 tmp=`mktemp_`
	 cp $file $tmp
	 sed -i "s?ascii:fieldsplit?ascii_fieldsplit?g" $tmp
	 sed -i "s?//?twoslashes?g" $tmp
	 sed -i "s?#?//?g" $tmp
	 LC_ALL="en_US.UTF-8" $TRUST_ROOT/exec/astyle/bin/astyle -n --indent=spaces=4 $tmp 1>/dev/null
	 sed -i "s?//?#?g" $tmp
	 sed -i "s?twoslashes?//?g" $tmp
	 sed -i "s?ascii_fieldsplit?ascii:fieldsplit?g" $tmp
	 cp $tmp $file
      fi
   fi
done

echo "...done"
