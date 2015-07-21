#!/bin/bash
# Check_maillage: 
# Ce programme realise  le jeu de donnees minimum pour le trace du maillage avec VisIt:
# traitement du cas ou plusieurs fichiers .data peuvent coexister dans le repertoire de l'etude
#
# Aide
AIDE()
{
   echo ""
   echo "	-help		: L'aide est affichee"
   echo "	-test	        : Permet de tester Check_maillage.ksh sur l'ensemble des"
   echo "			  cas tests de non regression"
   echo ""
}

# Fichier Python pour visu automatique
Ecriture_Python()
{
  fichier_config=$tmp/visit.py
  echo "# J'ouvre la session visit"				>  $fichier_config
  echo "OpenDatabase(\"$fichier_lata\")"			>> $fichier_config
  echo ""							>> $fichier_config
  echo "# Je trace le maillage"					>> $fichier_config
  let i=0
  color[1]="(0,255,0,255)"
  color[2]="(0,0,255,255)"
  color[3]="(0,255,255,255)"
  color[4]="(255,255,0,255)"
  for domaine in $domaines
  do
    let i=$i+1
    [ $i -gt 4 ] && color[$i]="(255,255,$i*100,255)"
    echo "AddPlot(\"Mesh\", \"$domaine\")"			>> $fichier_config
    if [ "$NB_CPUS" != "" ]
    then
       echo "AddPlot(\"Subset\", \"blocks($domaine)\")"		>> $fichier_config    
    else
       echo "AddPlot(\"Subset\", \"$domaine\")"			>> $fichier_config
       echo "s = SubsetAttributes()"				>> $fichier_config
       echo "s.colorType = 0"					>> $fichier_config
       echo "s.singleColor = ${color[$i]}"			>> $fichier_config
       echo "SetPlotOptions(s)"					>> $fichier_config
    fi
  done
  echo ""							>> $fichier_config
  echo "# Je trace le tout"					>> $fichier_config
  echo "DrawPlots()"						>> $fichier_config
  echo ""							>> $fichier_config
  echo "# Je ferme le fichier visit"				>> $fichier_config
  #echo "Close()"						>> $fichier_config
  #echo "exit()"							>> $fichier_config
}


#########
# Debut #
#########
echo "Usage: `basename $0` [-help] [-test] filename[.data]"
# default :
nb_argument=$#
nb_arg=0
test=0

while (( nb_arg < nb_argument ))
do
   [ ${#1} != 0 ] && arg=$1
   case $arg in
      -help)		AIDE && exit ;;
      -test)		test=1 && shift ;;
      -*)		echo ""
      			echo "Argument $arg non compris"
			AIDE && exit ;;
      *)		[ -f ${arg%.data}.data ] && break
      			[ ! -f ${arg%.data}.data ] && echo "File ${arg%.data}.data not found." && exit ;;
   esac
   ((nb_arg=nb_arg+1))
done

if [ ${#1} = 0 ]
then
   ETUDE_TRUST=`QUELLE_ETUDE.ihm`
   export ETUDE_TRUST
   cd $ETUDE_TRUST
   j=0
   for i in `ls *.data`
   do
      echo "Case $i"
      let j=$j+1
   done
   if [ $j -gt 1 ]
   then
      echo $ECHO_OPTS "\nSeveral data files exist, which one of them?"
      read NOM
      NOM=${NOM%.data}
      if [ ! -f $NOM.data ]
      then
         echo "Case $NOM.data does not exist."
         eval $FIN
      fi
   else
      NOM=${i%.data}
   fi
else
   if ((test==1))
   then
      cherche.ksh PARALLEL@OK@8
      for file in `cat liste_cherche`
      do
         cd $TRUST_TESTS/*/$file
	 make_PAR.data $file 8
	 Check_maillage.ksh PAR_$file
      done
      exit
   fi
   NOM=${1%.data}
   date=0
   geom=0
   if [ -f $NOM.data ]
   then
      data=1
   else
      NOM=${1%.geom}
      [ -f $NOM.geom ] && geom=1
   fi
fi

if [ ${#exec} = 0 ] || [ ! -f $exec ]
then
   echo $ECHO_OPTS "Binary $exec not found !"
   eval $FIN
fi

##############################################
# Recherche du nombre de processeurs du calcul
##############################################
NB_CPUS=""
if [ "`grep '# BEGIN MESH #' $NOM.data`" = "" ] || [ "`grep '# BEGIN READ #' $NOM.data`" != "" ]
then
    Zones=`grep -i 'scatter ' $NOM.data | grep -v "#" | $TRUST_Awk '{print $2}' | sed "s?.Zones?_*.Zones?g"`
    if [ "$Zones" != "" ]
    then
       Nb_Zones=`echo "$Zones" | wc -l`
       NB_CPUS=`ls $Zones 2>/dev/null | wc -l`
       NB_CPUS=`awk "BEGIN {print $NB_CPUS/$Nb_Zones}"`
       [ "$NB_CPUS" = 0 ] && NB_CPUS=""
    fi
fi

################################################
# Construction d'un jeu de donnees allege pour #
# -lire le maillage
# -le discretiser
# -le postraiter 
################################################
tmp=`mktemp_ -d`
FILE=$tmp/.$NOM.data
fichier_lata=$tmp/.Check_mesh_$NOM.lata
# Recherche du numero de ligne qui contient  l'occurence Discretiser pour "clore" le fichier "allege"
num_lig=`grep -i -n -e discretiser -e discretize $NOM.data | cut -d":" -f1`
[ ${#num_lig} = 0 ] && echo "Keyword Discretize is in not your data file. Exit..." && exit
head -$num_lig $NOM.data > $FILE
# Permet de supprimer les commentaires dans $FILE
sed -i '/#/!b;:a;/.#/!{;N;ba;};s:#[^#]*#::g' $FILE 
$TRUST_ROOT/bin/KSH/dos2unix_ $FILE

###################################
# Recherche des domaines a afficher 
###################################
domaines=`grep -i Scatter $FILE | $TRUST_Awk '(NF==3) {print $3}'`
if [ ${#domaines} = 0 ]
then
   # Si on ne trouve pas avec Scatter on recherche domaine
   domaines=`grep -i domaine $FILE | $TRUST_Awk '(NF==2) || (NF==3) {print $NF}'`
fi
nombre_domaines=`echo $domaines | wc -w`

################################################
# Insertion du Postraiter_domaine au format LATA
################################################
echo $ECHO_OPTS "Postraiter_domaine { format lata domaine\c" >> $FILE
[ $nombre_domaines != 1 ] && echo $ECHO_OPTS "s {\c" >> $FILE     
for domaine in $domaines
do
   echo $ECHO_OPTS " $domaine\c" >> $FILE     
done
[ $nombre_domaines != 1 ] && echo $ECHO_OPTS " } \c" >> $FILE    
echo $ECHO_OPTS " fichier $fichier_lata }\nFin" >> $FILE

#####################################
# Lancement du calcul pour postraiter
#####################################
echo "Converting mesh->lata..."
etude=`pwd`
cd $tmp
for file in `ls $etude`
do
   ln -s -f $etude/$file .
done
trust .$NOM $NB_CPUS 1>.$NOM.out 2>.$NOM.err
erreur=$?
echo "$nombre_domaines domain(s) detected : "$domaines
echo "========================================"
grep "Number of elements:" .$NOM.err
grep "Number of nodes:"    .$NOM.err
echo "========================================"
if [ $erreur != 0 ] || [ ! -f $fichier_lata ]
then 
   echo "Error detected:"
   cat .$NOM.err
   exit -1
fi

# Ecriture du script Python
Ecriture_Python
# Bizarrerie de VisIt 2.5.2, il faut monter d'un niveau pour que cela marche
cd ..
#visit -debug 5 -s $fichier_config # 1>/dev/null 2>&1
# GF en 282 ne pas mettre -cli
visit  -s $fichier_config # 1>/dev/null 2>&1
erreur=$?

# On efface les fichiers si pas d'erreurs
if [ $erreur = 0 ]
then
   rm -r -f $tmp
   rm -f $fichier_lata
   rm -f ${fichier_lata%.lata}.py 
   rm -f ${fichier_lata%.lata}.tmp 
   rm -f $fichier_config
   rm -f visitlog.py
fi
