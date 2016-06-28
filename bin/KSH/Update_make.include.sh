#!/bin/bash
# on utilise l'ancienne methode make depend pour verifier le fichier make.include
sed "s/opt_compile/_opt/" $TRUST_ENV/makefile.rep > makefile
make depend  || exit -1
make all_depend
cp make.include make.include.sa
[ ! -f make.include.test ] && echo "uptodate" && rm makefile  make.include.sa&& exit 0

cp make.include.test make.include
touch CMakeLists.txt
make depend || (cp make.include.sa make.include ; exit -1)

echo recompil ?
read ok
make_this_dir clean
make_this_dir 
 rm makefile make.include.test make.include.sa
git status .
