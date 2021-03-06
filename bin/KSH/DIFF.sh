#!/bin/bash
cd $rep_dev
echo $rep_dev
MODE=$1
[ "$1" = "RECUP" ] && echo > cmd
[ "$MODE" = "NET" ] && mkdir -p inut_files
[ "$MODE" = "NET" ] && mkdir -p Org_files version_files
shift
echo "usage [RECUP|COMP|DIFF]"
listef=`ls *.h *.cpp *.f`
[ "$1" != "" ] && listef="$*"
for file in  $listef
do
  file2="`$TRUST_ROOT/bin/Find.sh $file`"

  # on determine le niveau de modification (0 pas modifie, 1 modifie, 2 cree, 3 modifie par entete
  m=0
  if [  "$file2" = "" ] 
  then
    echo $file cree!!!
    m=2
  else
    if [ "`diff $file2 $file `" != "" ]
    then
      if [ "`diff $file2 $file --ignore-all-space `" != "" ]
      then
        if [ "`diff -I "// Version" $file2 $file`" != "" ] 
        then
          echo $file modifie && m=1
        else
	  echo $file modifie par numero de version && m=3
	fi
      else
        echo $file modifie par des blancs &&m=1
      fi
    else 
      echo $file inchange
    fi
  fi

  [ "$MODE" = "DIFF" ] && [ $m = 1 ] && diff $file $file2 
  dirrere=`dirname $file2 2>/dev/null`
  [ "$dirrere" = "." ] && dirrere="\$newdir"
  [ "$dirrere" = "" ] && dirrere="\$newdir"
  [ "$MODE" = "RECUP" ] && ( [ $m = 1 ] || [ $m = 2 ] ) && echo install_un_fichier `basename $file`  $dirrere >>cmd
  [ "$MODE" = "MERG" ] && [ $m = 1 ] && merg_files $file $file2 
  [ "$MODE" = "COMP" ] && ( [ $m = 1 ] || [ $m = 3 ] ) &&  comp_files $file $file2 
  [ "$MODE" = "COMP3" ] && ( [ $m = 1 ]  ) &&  comp_files $file $file2 
  [ "$MODE" = "COMP2" ] && ( [ $m = 1 ] || [ $m = 3 ] ) &&  tkdiff $file $file2 
  [ "$MODE" = "CPP" ] && [ $m = 1 ] && /lib/cpp -P -DNDEBUG -I $DIPHA_ROOT/include -I $TRUST_ROOT/include -I/usr/lib/gcc-lib/i686-pc-linux-gnu/pgcc-2.91.66/include -I/usr/include/g++ $file > $file.p && /lib/cpp -P -DNDEBUG -I $DIPHA_ROOT/include -I $TRUST_ROOT/include -I/usr/lib/gcc-lib/i686-pc-linux-gnu/pgcc-2.91.66/include -I/usr/include/g++ $file2 > $file.porg && comp_files $file.p $file.porg 
  if [ "$MODE" = "NET" ]
  then
    if [ $m = 1 ] 
    then
       cp $file2 Org_files
    else
       [ $m -eq 0  ]  && mv $file inut_files
       [ $m -eq 3 ] && mv $file version_files&& cp $file2 Org_files
    fi
  fi
done
chmod -R +w Org_files
[ "$MODE" = "RECUP" ] &&  echo executer cmd pour installer
[ "$MODE" = "NET" ] &&  DIFF.sh DIFF > $rep_dev/DIFF.out
