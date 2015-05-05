#!/bin/bash
check_only=0

[ "$1" = "-check" ] && check_only=1 && shift
TEMPLATE=$1
DIR=`dirname $0`
DIR=`(cd $DIR;pwd)`

DESTFILE=`basename $TEMPLATE .P`


TMP_DESTFILE=`mktemp_`

python $DIR/preprocessor.py -traitement_entete_trio $TEMPLATE $TMP_DESTFILE

# Indentation automatique du fichier genere
$TRUST_ROOT/bin/KSH/indent_file.sh $TMP_DESTFILE 1>/dev/null 2>&1

# remove header from destination file to compare
TMP1=`mktemp_`
TMP2=`mktemp_`

# GF on regarde si dans les 7 premieres c'est un commentaire ou pas... 
$TRUST_Awk  '{if (FNR>7) {print $0} else {if (index($1,"//")!=1) {print $0}}}'  $DESTFILE > $TMP1
$TRUST_Awk  '{if (FNR>7) {print $0} else {if (index($1,"//")!=1) {print $0}}}' $TMP_DESTFILE > $TMP2
if [ "`diff $TMP2 $TMP1`" ]
then
    if [ $check_only -eq 1 ]
	then
	echo "$DESTFILE must be updated."
	echo $TMP_DESTFILE
	exit 1
    else
	CHECKOUT $DESTFILE 1>/dev/null 2>&1
	mv -f $TMP_DESTFILE $DESTFILE
	echo "$DESTFILE has been updated."
    fi
else
   rm -f $TMP_DESTFILE
fi
rm -f $TMP1 $TMP2
