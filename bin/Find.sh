#!/bin/bash
nom=$1
[ ${#TRUST_ROOT} = 0 ] && echo "Initialisez l'environnement TRUST." && exit
# Cas des fichiers dans $TRUST_ROOT
# qui ont File pour reperer leur nom en-tete
# et Directory pour reperer le repertoire ou ils sont places
# Bien afficher le File ou le Directory a la fin car probleme
# si dans le fichier s.fichier evolution de l'en tete !!!
if [ 1 -eq 2 ]
then
if [ -f $nom ] 
then
   nom_fic=`$TRUST_Awk '/File/ {File=$NF} END {print File}' $nom` 
   dir_fic=`$TRUST_Awk '/Directory/ {Directory=$NF} END {print Directory}' $nom` 
   dir_fic=$TRUST_ROOT/${dir_fic#'$TRUST_ROOT/'}
else
   nom_fic=$nom
   dir_fic="."
fi
fi
nomfichier=nontrouve
#nomfichier=$dir_fic/$nom_fic

if [ ! -f $nomfichier ]  
then
   #nomfichier=`find $TRUST_ROOT -name $nom -print`
   for dir in `cat $TRUST_ENV/rep.TRUST`
   do 
      rep=$TRUST_ROOT/$dir
      if [ -f $rep/$nom ] && nomfichier=$rep/$nom 
      then
         echo $rep/$nom
         [ "$2" = -copy ] && cp -f $rep/$nom .
      fi
   done
else
   echo $nomfichier
   [ "$2" = -copy ] && cp -f $nomfichier .
fi
