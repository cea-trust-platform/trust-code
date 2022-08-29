#!/bin/bash

ORG=`pwd`
ln -s -f ../../../Outils/lata_tools/src/trust_commun .
mkdir -p $TRUST_LATAFILTER/lib
cd $TRUST_LATAFILTER
mkdir -p include build

cd build
ln -sf ../../../../ThirdPart/src/LIBLATAFILTER/trust_commun/* . 1>/dev/null 2>&1

# If lata_tools srcs are touched, rebuild LIBLATAFILTER !
[ "$(find  $TRUST_ROOT/Outils/lata_tools/src/trust_commun -type f -newer $ORG/makefile.sa)" != "" ]  && touch $ORG/makefile.sa
if [ ! -f makefile ] || [ $ORG/makefile.sa -nt makefile ]
then   
   cp $ORG/makefile.sa makefile
   make clean
fi
make depend
$TRUST_MAKE || exit -1

# Construction du repertoire include
mkdir -p $TRUST_LATAFILTER/include
for f in `ls $ORG/trust_commun/*.h`
do
   [ "`diff $f $TRUST_LATAFILTER/include 2>&1`" != "" ] && cp $f $TRUST_LATAFILTER/include && echo $f updated
done
exit 0
