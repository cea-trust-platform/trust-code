#!/bin/bash
#
# Ce fichier sert a generer automatiquement ArrOfInt.h et ArrOfDouble.h
# Il s''agit d''un shell script executable.
# On construit les fichiers en remplacant certains mots cles par
# d''autres.
#
# Les fonctions "replace_keywords_double" et "replace_keywords_entier"
# prennent un fichier et remplacent les mots cles
# par Double ou double ou entier ...
#
# Les fonctions "create_h_proto" et "create_cpp_proto" ecrivent le fichier
# prototype (avec les mot cles a la place de double ou entier).
#
# A la fin du script se trouvent les appels a ces fonctions.


check_file ()
{
  local file_to_update=$1
  local new_file=$2
  if diff $file_to_update $new_file >/dev/null
  then
	  echo File $file_to_update already up to date
	  rm -f $new_file
  else
	  echo Updating file $file_to_update Backup file $file_to_update.old
	  CHECKOUT $file_to_update 1>/dev/null 2>&1
	  cp $file_to_update $file_to_update.old
	  mv -f $new_file $file_to_update
  fi
}

copy_triou_file ()
{
	local src=`Find $1`
	local dest=$2
	if [ -f $dest ]
	then
		echo Error in check.sh copy_triou_file, file $dest already exists
	 	exit
	fi
	echo // File copied from TRUST tree and modified: source file info below >$dest
	cat $src >>$dest
}

# *********************
#  programme principal
# *********************
for file in ArrOfDouble.h ArrOfInt.h ArrOfDouble.cpp ArrOfInt.cpp ArrOfFloat.h ArrOfFloat.cpp
do
  prepro="python $TRUST_ROOT/bin/KSH/preprocessor.py"
  j=`find . -name "*.P" -newer $file`
  j=""
  if test -z "$j"
      then
      echo Generating $file.P
      $prepro $file.P $file.tmp
      cmd=`grep 'style=' $TRUST_ROOT/bin/KSH/indent_file.sh`
      tmp=`echo $file.tmp`
      echo $tmp
      eval $cmd
      #xemacs -batch $file.tmp -l $TRUST_ROOT/bin/KSH/indent.emacs 1>/dev/null 2>&1
      check_file $file ${file}.tmp
  fi
done

# Verification des sources construites a partir de TRUST
if [ -z "$TRUST_ROOT" ]
then
  echo TRUST_ROOT directory not set: files built from trio_u tree not checked
else
  list="Static_Int_Lists.h Static_Int_Lists.cpp Connectivite_som_elem.cpp Connectivite_som_elem.h ArrOfBit.cpp ArrOfBit.h Octree_Int.h Octree_Int.cpp Octree_Double.h Octree_Double.cpp"
  list=
  for i in $list
  do
    copy_triou_file $i tmp
    check_file $i tmp 
  done
fi
rm -f sed_script.tmp
