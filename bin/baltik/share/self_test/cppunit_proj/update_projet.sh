#!/bin/bash


test_update()
{

    if [ "`diff $f1 $2`" != "" ]
	then
	CHECKOUT $2
	cp $f1 $2
    fi
}

cd src
f1=`mktemp_`
sed "s/PE_Groups::groupe_TRUST().abort();/throw RangeError(\"triou n'a pas compris\");PE_Groups::groupe_TRUST().abort();/;s?//     Constructeur par defaut?#include <RangeError.h>?" $TRUST_ROOT/src/Kernel/Utilitaires/process.cpp > $f1
test_update $f1 process.cpp
rm -f $f1


