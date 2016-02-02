#!/bin/bash
echo "Usage: gdb.sh [-valgrind] \$exec"
valgrind=0 && [ "$1" = -valgrind ] && valgrind=1 && shift
directory=""
if [ "$1" != "" ] && [ -f $1 ]
then
   #Le lancement prend desormais en compte un repertoire d'include pour gdb 6.x
   exec=`basename $1`
   MonoDir=$TRUST_ROOT/MonoDir${exec#TRUST} && [ -d $MonoDir/src ] && directory="--directory $MonoDir/src --directory $MonoDir/include"
   # Pour prendre en compte l'atelier on ajoute aussi le repertoire du binaire
   directory=$directory" --directory `dirname $1`"
fi

if [ "$valgrind" = 1 ]
then
   valgrind --leak-check=full --track-origins=yes --show-reachable=yes --suppressions=$TRUST_ROOT/Outils/valgrind/suppressions --num-callers=15 --db-attach=yes --db-command="/usr/bin/gdb $directory -nw %f %p" $*
else
   # Surcharge de gdb pour pouvoir afficher des tableaux
   # Lancement de valgrind
   options=$TRUST_ROOT/bin/KSH/gdb.options
   echo "---------------------------------------------------------------------"
   cat $options | grep "To print"
   echo "# To debug dynamically-linked executable, use: set auto-solib-add off"
   echo "Then load the library you want to debug: share name_of_library.so"
   echo "---------------------------------------------------------------------"
   echo "Lancement de: /usr/bin/gdb $directory -x $options $*"
   /usr/bin/gdb $directory -x $options $* 
fi
