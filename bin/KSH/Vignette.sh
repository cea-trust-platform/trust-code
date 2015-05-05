#!/bin/bash

CLEARCASE()
{
    # fait juste un checkout de l'element $1
    dir=`dirname $1`
    el=`basename $1`
    cd $dir
    CHECKOUT $el
    cd -
}
#########################################################################################################################################
#																	#
#  Ce programme sert a creer une image du maillage de cas test										#
#																	#
#########################################################################################################################################

#########################################################################################################################################
# Usage
echo ""
echo "Usage :	Vignette.sh [-no_verbose] [jdd]"
echo ""
echo "! ATTENTION, par defaut, sans jdd, tous les cas tests sont traites !"
echo ""



#########################################################################################################################################
# Environnement
verbose=1 && [ "$1" = -no_verbose ] && verbose=0 && shift
PATH_LANGUAGE=`pwd`



#########################################################################################################################################
# Si l'environnement TRUST de la version Test est charge, je continue
if [ "$TRUST_ROOT" != "" ]
then
   nb_mot=`echo "$TRUST_ROOT" | sed 's?/? ?g' | wc -w`
   ((test=nb_mot+1))
   ((ref=nb_mot+2))
   ((rep=nb_mot+3))
   if [ "$1" != "" ]
   then
      Repertoire_data=$TRUST_TESTS/*/$1
      if [ ! -d $Repertoire_data ]
      then
         if [ $verbose = 1 ]
         then
            echo "Le cas test \"$1\" n'existe pas (jdd sans .data) !"
            echo ""
	 fi
	 exit
      fi
   else
      echo -n "Vous etes sure de vouloir traiter tous les cas ? (o/n) : "
      read reponse
      echo ""
      if [ "$reponse" != "o" ]
      then
         exit
      fi
      Repertoire_data=$TRUST_TESTS/*
   fi
else
# Sinon, j'arrete le process
   if [ $verbose = 1 ]
   then
      echo "L'environnement TRUST n'est pas charge !"
      echo ""
   fi
   exit
fi




#########################################################################################################################################
# Je cherche les noms des fichiers .data a traiter
data=`du -a $Repertoire_data | grep '.*data$' | awk '{print $2}' | sort -u | sed '/\/FA\//d' | sed 's?/? ?g' | awk '{print "'$TRUST_ROOT/'"$'$test'"/"$'$ref'"/"$'$rep'}' | sort -u`
nb_data=`echo "$data" | wc -l`
i=1
a=0
b=0
echo 'Etat	 |	Nom du cas' > $PATH_LANGUAGE/Vignette.log
while (( i <= nb_data ))
do

   # Je cherche les noms des fichiers .data existants dans la base de cas tests
   fichier_data=`echo "$data" | head -$i | tail -1`
   nb_mot=`echo "$fichier_data" | sed 's?/? ?g' | wc -w`
   Nom_data=`echo "$fichier_data" | sed 's?/? ?g' | awk '{print $'$nb_mot'".data"}'`
   
   mkdir $fichier_data/TMP_VIGNETTES
   cd $fichier_data/TMP_VIGNETTES

   # Je copie ces fichiers un a un dans mon repertoire locale
   cp $fichier_data/* $fichier_data/TMP_VIGNETTES/.
   chmod u+wx $Nom_data

   # Je rajoute si possible le format meshtv
   [ "`grep -i 'format.*meshtv' $Nom_data`" = "" ] && echo $ECHO_OPTS '1,$ s?[cC][hH][aA][mM][pP][sS].*[dD][tT]_[pP][oO][sS][tT]?format meshtv Champs dt_post?g\nw' | ed $Nom_data 1>/dev/null 2>&1

   # Si je ne trouve pas en fin de compte "format meshtv" je ne peux pas traiter le fichier
   if [ "`grep -i 'format.*meshtv' $Nom_data`" = "" ]
   then
      change=0
   else
   # Si je trouve en fin de compte "format meshtv" je peux traiter le fichier
      change=1
      ((a=a+1))

      # Je ne fais qu'un seul pas de temps
      if [ "`grep -i 'nb_pas_dt_max' $Nom_data`" == "" ]
      then
         nb_Tinit=`grep -i 'Tinit' $Nom_data | wc -l | awk '{print $1}'`
         l=1
         while (( l <= nb_Tinit ))
         do
            Ligne_Tinit=`grep -in 'Tinit' $Nom_data | head -$l | tail -1 | awk -F':' '{print $1}'`
	    fichier=`sed "$Ligne_Tinit a\	nb_pas_dt_max 1" $Nom_data`
	    echo "$fichier" > $Nom_data
	    ((l=l+1))
         done
      else
         fichier=`sed '1,$ s?[nN][bB]_[pP][aA][sS]_[dD][tT]_[mM][aA][xX].*[^#]$?nb_pas_dt_max 1?g' $Nom_data`
         echo "$fichier" > $Nom_data
      fi
   fi

   if [ -f ${Nom_data%.data}.lml.gif ] && [ -f ${Nom_data%.data}.lml.gif.fin ]
   then
      change=0
      echo "Already	 |	$Nom_data" >> $PATH_LANGUAGE/Vignette.log
   fi
   
   # Si je peux traiter le fichier, je lance TRUST puis VISIT pour faire mes images
   if (( change == 1 ))
   then
      echo "Running...	$Nom_data" >> $PATH_LANGUAGE/Vignette.log

      # Je lance TRUST sur mon cas
      if [ -f prepare ]
      then
         ./prepare 1>prepare.out 2>prepare.err
      fi
      $exec ${Nom_data%.data} 1>${Nom_data%.data}.out 2>${Nom_data%.data}.err

      # Je cree le fichier Python a utiliser pour visit
      echo "# J'ouvre le fichier visit"							>  Maillage.py
      echo "OpenDatabase(\"$fichier_data/TMP_VIGNETTES/${Nom_data%.data}.visit\")"	>> Maillage.py
      echo ""										>> Maillage.py
      echo "# Je regle les attributs de sauvegarde"					>> Maillage.py
      echo "s = SaveWindowAttributes()"							>> Maillage.py
      echo "s.format = s.JPEG"								>> Maillage.py
      echo "s.fileName = \"Maillage\""							>> Maillage.py
      echo "s.screenCapture = 0"							>> Maillage.py
      echo "SetSaveWindowAttributes(s)"							>> Maillage.py
      echo ""										>> Maillage.py
      echo "# Je Trace le maillage"							>> Maillage.py

      nb_domaine=`grep -i "domaine" $Nom_data | wc -l | awk '{print $1}'`
      j=1
      while (( j <= nb_domaine ))
      do
         domaine=`grep -i "domaine" $Nom_data | head -$j | tail -1 | awk '{print $2}'`
	 nb_motlu=`grep -i "associer.*$domaine" $Nom_data | wc -l | awk '{print $1}'`
	 k=1
         while (( k <= nb_motlu ))
         do
	    motlu=`grep -i "associer.*$domaine" $Nom_data | head -$k | tail -1 | awk '{print $3}'`
	    if [ "$motlu" == "$domaine" ]
	    then
               echo "AddPlot(\"Mesh\", \"$domaine\")"					>> Maillage.py
	    fi
	    ((k=k+1))
	 done
         ((j=j+1))
      done

      echo "DrawPlots()"								>> Maillage.py
      echo ""										>> Maillage.py
      echo "# Je sauvegarde  mon image"							>> Maillage.py
      echo "SaveWindow()"								>> Maillage.py
      echo ""										>> Maillage.py
      echo "# Je ferme le fichier visit"						>> Maillage.py
      echo "Close()"									>> Maillage.py

      # Je lance Visit avec comme parametres le fichier Python
      visit -cli -s Maillage.py 1>/dev/null 2>/dev/null &

      # Je stocke le PID de Visit au cas ou !
      PID_process_visit=`echo $! | awk '{print $1}'`

      # Tant que mon fichier JPEG n'a pas ete cree, je dors
      while [ ! -e Maillage0000.jpeg ]
      do
         sleep 1
      done

      sleep 1

      # Je deplace mon fichier JPEG en Nom du cas .lml.gif et .lml.gif.fin
      if [ -s Maillage0000.jpeg ]
      then
         gif=$fichier_data/${Nom_data%.data}.lml.gif
         gif_fin=$fichier_data/${Nom_data%.data}.lml.gif.fin
	 CLEARCASE $gif_fin
         convert Maillage0000.jpeg -resize 100x100 gif:$gif_fin
	 CLEARCASE $gif
         convert Maillage0000.jpeg -resize 800x800 $gif
	 ((b=b+1))
         Vignette=`sed "1,$ s?Running...	$Nom_data?OK	 |	$Nom_data?g" $PATH_LANGUAGE/Vignette.log`
	 echo "$Vignette" > $PATH_LANGUAGE/Vignette.log
      else
         Vignette=`sed "1,$ s?Running...	$Nom_data?No Match |	$Nom_data?g" $PATH_LANGUAGE/Vignette.log`
	 echo "$Vignette" > $PATH_LANGUAGE/Vignette.log
      fi
   fi

   # Je fais le menage
   rm -rf $fichier_data/TMP_VIGNETTES 1>/dev/null 2>/dev/null
   ((i=i+1))
done 2>/dev/null

# Fin
if [ $verbose = 1 ]
then
   echo "" >> $PATH_LANGUAGE/Vignette.log
   echo "$a fichiers cas tests ont ete traites sur les $nb_data fichiers existants." >> $PATH_LANGUAGE/Vignette.log
   echo "$b fichiers cas tests ont produits une image." >> $PATH_LANGUAGE/Vignette.log
   echo "" >> $PATH_LANGUAGE/Vignette.log
   echo ""
   echo "$a fichiers cas tests ont ete traites sur les $nb_data fichiers existants."
   echo "$b fichiers cas tests ont produits une image."
   echo ""
   echo "Tous les resultats sont consultables dans le fichier \"$PATH_LANGUAGE/Vignette.log\""
   echo ""
fi
