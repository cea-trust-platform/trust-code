#!/bin/bash
# Parcours la base des tests et sort un rapport
echo "Usage: `basename $0` [-reference_only] keywords"
echo "Mettre @ pour les espaces entre mots. Exemple: `basename $0` PARALLEL@OK"
[ $TRUST_ARCH = linux ] && echo "Mettre * pour chercher des mots incomplets. Exemple: `basename $0` VEF*" 
filtre="*" && [ "$1" = "-reference_only" ] && filtre="Reference" && shift
mots=$*
i=0
TMP_REP=`mktemp_ -d`
for mot in $mots
do
    let i=$i+1
    if [ $TRUST_ARCH = linux ]
    then
       # Le \\* est important car * ne marche pas sur ioulia...
       # echo $mot | $TRUST_Awk '{gsub("@"," ",$0);gsub("\\*","[a-z0-9_]\\*",$0);print $0}' > $TMP_REP/mot$i
       # 2016/04/28
       # avec \\*, "cherche.ksh VEF*" renvoie rien
       # avec \*, ca renvoie bien la liste souhaitee mais avec message "gawk: cmd. line:1: warning: escape sequence `\*' treated as plain `*'"
       # avec * seul, ca renvoie la liste sans warning
       echo $mot | $TRUST_Awk '{gsub("@"," ",$0);gsub("*","[a-z0-9_]*",$0);print $0}' > $TMP_REP/mot$i
    else
       echo $mot | $TRUST_Awk '{gsub("@"," ",$0);print $0}' > $TMP_REP/mot$i
    fi
done

format=txt && [ "$1" = "-html" ] && format=html && mots=${mots#-html}
sens=colonne && [ "$1" = "-ligne" ] && sens=ligne && mots=${mots#-ligne}
liste_cherche=liste_cherche
rm -f $liste_cherche
liste_cherche=liste_cherche
rm -f $liste_cherche
liste=`mktemp_`
#ls $TRUST_TESTS/$filtre/[A-C]*/*.lml.gz 1>$liste
#ls $TRUST_TESTS/$filtre/[D-G]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[H-L]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[M-P]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[Q-R]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[S-T]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[U-Z]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[a-n]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[o-z]*/*.lml.gz 1>>$liste
#ls $TRUST_TESTS/$filtre/[0-9]*/*.lml.gz 1>>$liste
find $TRUST_TESTS/$filtre/ -follow -name *.lml.gz -print 2>/dev/null>$liste
# On fait un sort -u car [a-z] bugge sous sh Mandrake 9.x
tmp=`mktemp_`
# -w pour ne chercher que des mots complets (marche pas sur SGI,HP... !)
w="-w"
if [ $TRUST_ARCH = SGI ] || [ $TRUST_ARCH = HPPA_11 ]
then
   w=""
fi 
for rep in `cat $liste | sort -u`
do
   file=`dirname $rep`/`basename $rep .lml.gz`.data
   if [ -f $file ]
   then
      # Fait un .data sans commentaire sauf PARALLEL et quitter apres le resoudre
      $TRUST_Awk '!/#/ || /PARALLEL/ {print $0} /Resoudre/ {exit} ' $file > $tmp
      ok=1
      i=0
      for mot in $mots
      do
	 let i=$i+1
	 [ "`grep -i $w -f $TMP_REP/mot$i $tmp`" = "" ] && ok=0 && break
      done
      if [ $ok = 1 ]
      then
	 if [ $format = txt ]
	 then
            echo `basename $file .data` | tee -a $liste_cherche
	 fi
      fi
   fi
done
rm -f $tmp
rm -r -f $TMP_REP $liste
echo 
echo "Liste des cas ecrite dans le fichier `pwd`/$liste_cherche"
echo "pour pouvoir lancer avec lance_test par :"
echo "echo liste_cherche | lance_test [\$exec] [\$repertoire]"
echo "ou"
echo "trust -check liste_cherche"
