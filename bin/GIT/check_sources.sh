#!/bin/bash
# Check TRUST sources in the current directory 
erreur()
{
   [ "$1" != 0 ] && err=1
}
err=0
# Source files should be tracked:
#if [ -d $TRUST_ROOT/../.git ]
# on teste si on a git dans le repertoire courant
git tag 1>/dev/null 2>&1
if [ $? = 0 ] 
then
   pwd_=`pwd`
   git status -s *.cpp *.h *.f *.c *.P 2>/dev/null | grep -v instancie_ | grep -v config.h | $TRUST_Awk -v dir=$pwd_ 'BEGIN {err=0}
   !/~/ && ($1=="??") {err=1;print "\nThe file "dir"/"$2" is not staged under Git!\nStage the file with \"git add "dir"/"$2"\" or delete this file."}
   END {exit err}'
   erreur $?
   exit $err
fi

# sinon on essaye hg
hg tags 1>/dev/null 2>&1
if [ $? = 0 ] 
then
   pwd_=`pwd`
   hg status  -u *.cpp *.h *.f *.c 2>/dev/null | grep -v instancie_ | grep -v config.h | $TRUST_Awk -v dir=$pwd_ 'BEGIN {err=0} !/~/ && ($1=="?") {err=1;print "\nThe file "dir"/"$2" is not staged under HG!\nStage the file with \"hg add "dir"/"$2"\" or delete this file."}
   END {exit err}'
   erreur $?
   exit $err
fi

exit $err
