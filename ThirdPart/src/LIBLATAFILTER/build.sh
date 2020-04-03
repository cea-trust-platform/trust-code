#!/bin/bash

ORG=`pwd`
ln -s -f ../../../Outils/lata2dx/lata2dx/commun_triou .
mkdir -p $TRUST_LATAFILTER/lib
cd $TRUST_LATAFILTER
mkdir -p include build

cd build
ln -sf $ORG/commun_triou/* . 1>/dev/null 2>&1
if [ ! -f makefile ] || [ $ORG/makefile.sa -nt makefile ]
then   
   cp $ORG/makefile.sa makefile
   make clean
fi
make depend
$TRUST_MAKE || exit -1

# Construction du repertoire include
mkdir -p $TRUST_LATAFILTER/include
for f in `ls $ORG/commun_triou/*.h`
do
   [ "`diff $f $TRUST_LATAFILTER/include 2>&1`" != "" ] && cp $f $TRUST_LATAFILTER/include && echo $f updated
done
exit 0
