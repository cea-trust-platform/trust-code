#!/bin/bash
TEMPLATE=dlsinterf.fP
DIR=`(cd $TRUST_ROOT/bin/KSH;pwd)`

DESTFILE=dlsinterf.f


TMP_DESTFILE=`mktemp_`

python $DIR/preprocessor.py  $TEMPLATE $TMP_DESTFILE


# remove header from destination file to compare
TMP1=`mktemp_`
TMP2=`mktemp_`
$TRUST_Awk 'FNR>7' $DESTFILE > $TMP1
$TRUST_Awk 'FNR>7' $TMP_DESTFILE > $TMP2
if [ "`diff $TMP2 $TMP1`" ]
then
   CHECKOUT $DESTFILE 1>/dev/null 2>&1
   mv -f $TMP_DESTFILE $DESTFILE
   echo "$DESTFILE has been updated."
else
   rm -f $TMP_DESTFILE
fi
rm -f $TMP1 $TMP2
