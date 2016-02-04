#!/bin/bash

HELP()
{
   echo ""
   echo "	Ce programme recherche dans toutes les sources du Kernel par"
   echo "	defaut, ou dans le fichier passe en parametre, les sorties de"
   echo "	TRUST (Cerr ou Cout) et propose si l'utilisateur le"
   echo "	souhaite, une traduction en anglais automatique via Google."
   echo ""
   echo "	L'option '-dir rep' permet de ne pas traiter le repertoire rep"
   echo "	et l'option '-dir all' permet de traiter tous les repertoires"
   echo "	(par defaut uniquement Kernel)."
   echo ""
   echo "	Le Pourcentage indique en haut, a droite permet de determiner"
   echo "	l'avancement du travail (le but etant d'arriver a 100%). Si l'option"
   echo "	-f a ete utilise, cet avancement est relatif au fichier \"file\""
   echo ""
   echo "	Pour arreter en cours de traduction, il suffit de faire CTRL+C."
   echo "	Cependant, si l'arret a lieu durant la traduction d'un fichier,"
   echo "	le travail deja fait sur le fichier sera perdu !"
   echo ""
   echo "	Pour redemarrer, il suffit de relancer le programme. Celui-ci"
   echo "	reprendra la ou il s'etait arreter."
   echo ""
   echo "	Il est conseille d'utiliser ce programme sans argument."
   echo ""
   echo "	-h, --help	: l'aide est affichee"
   echo "	-dir dir	: dir est le repertoire TRUST a ne pas traiter"
   echo "	     all	: all permet de traiter tous les repertoires"
   echo "	-f file		: file est le nom du fichier a traiter"
   echo "	-nolabel 	: aucun label n'est pose et le(s) fichier(s) n'est"
   echo "			: pas change en gestion de configuration"
   echo ""
}

ACCEPTE()
{
   Nb_t=2
   num_text=1
   texte_total_traduit=$texte_total
   while (( $Nb_t <= $Nb_texte ))
   do
      texte_a_traduire=`grep -Ei "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | head -$Nb_C | tail -1 | sed 's?\\\"?SLAGUILLEMET?g' | $TRUST_Awk -F"\"" '{print $'$Nb_t'}' | sed -e "s?^ *??g" -e "s? *\\\$??g" -e 's?SLAGUILLEMET?\\\"?g'`
      echo ""
      echo "Texte a  remplacer    : $texte_a_traduire"
      [ "$texte_a_traduire" != "`echo "$texte_a_traduire" | sed 's?\\\?SLASH?g'`" ] && echo "! Attention pour faire apparaitre un \"\\\" dans le texte de remplacement, utiliser \"\\\\\""
      texte_a_traduire=`echo "$texte_a_traduire" | sed -e 's?\\\?SLASH?g' -e 's?*?\\\*?g' -e 's?&?\\\&?g' -e 's?\??POINTDINTERRO?g' -e 's?\[?CROCHETOUVERT?g' -e 's?\]?CROCHETFERME?g'`
      echo -n "Texte de remplacement : "
      read texte_traduit
      texte_traduit=`echo "$texte_traduit" | sed -e 's?\\\?SLASH?g' -e 's?&?\\\&?g' -e 's?\??POINTDINTERRO?g' -e 's?\[?CROCHETOUVERT?g' -e 's?\]?CROCHETFERME?g'`
    if [ "$texte_a_traduire" != "$texte_traduit" ]
    then
      if [ ! -f $dir.trad_kernel.tmp ]
      then
         sed -e ''$num_ligne' s?\\?SLASH?g' -e ''$num_ligne' s?\??POINTDINTERRO?g' -e ''$num_ligne' s?\[?CROCHETOUVERT?g' -e ''$num_ligne' s?\]?CROCHETFERME?g' -e "$num_ligne s?$texte_a_traduire?$texte_traduit?g" -e ''$num_ligne' s?CROCHETFERME?\]?g' -e ''$num_ligne' s?CROCHETOUVERT?\[?g' -e ''$num_ligne' s?POINTDINTERRO?\??g' -e ''$num_ligne' s?SLASH?\\?g' $dir > $dir.trad_kernel.tmp
      else
         sed -e ''$num_ligne' s?\\?SLASH?g' -e ''$num_ligne' s?\??POINTDINTERRO?g' -e ''$num_ligne' s?\[?CROCHETOUVERT?g' -e ''$num_ligne' s?\]?CROCHETFERME?g' -e "$num_ligne s?$texte_a_traduire?$texte_traduit?g" -e ''$num_ligne' s?CROCHETFERME?\]?g' -e ''$num_ligne' s?CROCHETOUVERT?\[?g' -e ''$num_ligne' s?POINTDINTERRO?\??g' -e ''$num_ligne' s?SLASH?\\?g' $dir.trad_kernel.tmp > $dir.trad_kernel.tmp2
         mv -f $dir.trad_kernel.tmp2 $dir.trad_kernel.tmp
      fi
      [ ${#texte_a_traduire} == 0 ] && texte_a_traduire=" "
      [ ${#texte_traduit} == 0 ] && texte_traduit=" "
      texte_total_traduit=`echo "$texte_total_traduit" | sed -e 's?\\\?SLASH?g' -e 's?\??POINTDINTERRO?g' -e 's?\[?CROCHETOUVERT?g' -e 's?\]?CROCHETFERME?g' -e "s?$texte_a_traduire?$texte_traduit?g" -e 's?CROCHETFERME?\]?g' -e 's?CROCHETOUVERT?\[?g' -e 's?POINTDINTERRO?\??g' -e 's?SLASH?\\\?g'`
    fi
      ((num_text=num_text+1))
      ((Nb_t=Nb_t+2))
   done
   clear
   echo "Fichier : $dir		$pourcentage%"
   echo ""
   echo -n "Texte extrait          : "
   echo "$texte_total"
   echo -n "Traduction manuelle    : "
   echo "$texte_total_traduit"
   echo ""
   echo -n "Accepter (o/n)? "
   read reponse
}

TRADUCTION()
{
   Nb_Cerr=`grep -Ei "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | wc -l`
   Nb_C=1
   Traduction=0
   # Pour tous les Cerr|Cout trouves dans le fichier $dir
   while (( $Nb_C <= $Nb_Cerr ))
   do
      pourcentage=`$TRUST_Awk 'BEGIN {printf "%.3f",'$nb_cas'/'$nb_cas_tot'*100}'`
      clear
      echo "Fichier : $dir		$pourcentage%"
      echo ""
      # Je recupere la ligne $Nb_C
      texte_total=`grep -Ei "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | head -$Nb_C | tail -1`
      echo -n "Texte extrait          : "
      echo "$texte_total"
      echo ""
      echo -n "Traduire (o/n)? "; read reponse
      if [ "$reponse" == "O" ] || [ "$reponse" == "o" ]
      then
         clear
         echo "Fichier : $dir		$pourcentage%"
         echo ""
         echo -n "Texte extrait          : "
         echo "$texte_total"
         # Si je dois traduire, je copie dans le texte dans $texte_total_traduit
         texte_total_traduit=$texte_total
         # Je recupere le nombre de d'argument si le separateur est "
	 Nb_texte=`grep -Ei "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | head -$Nb_C | tail -1 | sed 's?\\\"?SLAGUILLEMET?g' | $TRUST_Awk -F"\"" '{print NF}'`
	 Nb_t=2
         # Je boucle sur tous les chiffres paires (texte a traduire)
	 while (( $Nb_t <= $Nb_texte ))
	 do
	    grep -Ei "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | head -$Nb_C | tail -1 | sed 's?\\\"?SLAGUILLEMET?g' | $TRUST_Awk -F"\"" '{print $'$Nb_t'}' | sed -e "s?^ *??g" -e "s? *\$??g" -e "s?\\\?SLASH?g" -e "s?#?CARRETOILE?g" -e 's?*?\\*?g' -e 's?\??POINTDINTERRO?g' -e 's?\[?CROCHETOUVERT?g' -e 's?\]?CROCHETFERME?g' > .texte_a_traduire
            texte_a_traduire=`cat .texte_a_traduire`
	    texte_traduit=`$TRUST_ROOT/bin/KSH/Traducteur.sh .texte_a_traduire Google`
	    rm -f .texte_a_traduire
	    num_ligne=`grep -Ein "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | head -$Nb_C | tail -1 | $TRUST_Awk -F: '{print $1}'`
	  if [ "$texte_a_traduire" != "$texte_traduit" ]
	  then
	    if [ ! -f $dir.trad_kernel.tmp ]
	    then
	       sed -e ''$num_ligne' s?\\\"?SLAGUILLEMET?g' -e ''$num_ligne' s?\\?SLASH?g' -e "$num_ligne s?#?CARRETOILE?g" -e ''$num_ligne' s?\??POINTDINTERRO?g' -e ''$num_ligne' s?\[?CROCHETOUVERT?g' -e ''$num_ligne' s?\]?CROCHETFERME?g' -e "$num_ligne s?$texte_a_traduire?$texte_traduit?g" -e ''$num_ligne' s?CROCHETFERME?\]?g' -e ''$num_ligne' s?CROCHETOUVERT?\[?g' -e ''$num_ligne' s?POINTDINTERRO?\??g' -e "$num_ligne s?CARRETOILE?#?g" -e ''$num_ligne' s?SLASH?\\?g' -e ''$num_ligne' s?SLAGUILLEMET?\\\"?g' $dir > $dir.trad_kernel.tmp
	    else
	       sed -e ''$num_ligne' s?\\\"?SLAGUILLEMET?g' -e ''$num_ligne' s?\\?SLASH?g' -e "$num_ligne s?#?CARRETOILE?g" -e ''$num_ligne' s?\??POINTDINTERRO?g' -e ''$num_ligne' s?\[?CROCHETOUVERT?g' -e ''$num_ligne' s?\]?CROCHETFERME?g' -e "$num_ligne s?$texte_a_traduire?$texte_traduit?g" -e ''$num_ligne' s?CROCHETFERME?\]?g' -e ''$num_ligne' s?CROCHETOUVERT?\[?g' -e ''$num_ligne' s?POINTDINTERRO?\??g' -e "$num_ligne s?CARRETOILE?#?g" -e ''$num_ligne' s?SLASH?\\?g' -e ''$num_ligne' s?SLAGUILLEMET?\\\"?g' $dir.trad_kernel.tmp > $dir.trad_kernel.tmp2
	       mv -f $dir.trad_kernel.tmp2 $dir.trad_kernel.tmp
	    fi
            [ ${#texte_a_traduire} == 0 ] && texte_a_traduire=" "
	    texte_total_traduit=`echo "$texte_total_traduit" | sed -e 's?\\\"?SLAGUILLEMET?g' -e 's?\\\?SLASH?g' -e "s?#?CARRETOILE?g" -e 's?\??POINTDINTERRO?g' -e 's?\[?CROCHETOUVERT?g' -e 's?\]?CROCHETFERME?g' -e "s?$texte_a_traduire?$texte_traduit?g" -e 's?CROCHETFERME?\]?g' -e 's?CROCHETOUVERT?\[?g' -e 's?POINTDINTERRO?\??g' -e "s?CARRETOILE?#?g" -e 's?SLASH?\\\?g' -e 's?SLAGUILLEMET?\\\"?g'`
	  fi
            ((Nb_t=Nb_t+2))
	 done
	 echo -n "Traduction automatique : "
	 echo "$texte_total_traduit"
         echo ""
	 echo -n "Accepter (o/n)? "
	 read reponse
	 while [ "$reponse" != "O" ] && [ "$reponse" != "o" ]
	 do
	    rm -f $dir.trad_kernel.tmp
	    [ -f $dir.trad_kernel.save ] && cp -f $dir.trad_kernel.save $dir.trad_kernel.tmp
	    ACCEPTE
	 done
	 [ -f $dir.trad_kernel.tmp ] && cp -f $dir.trad_kernel.tmp $dir.trad_kernel.save
         ((Traduction=Traduction+1))
      fi
      ((Nb_C=Nb_C+1))
      ((nb_cas=nb_cas+1))
   done
   echo "$dir" >> $TRUST_TMP/Fichier_deja_fait_Traduction_Kernel.tmp
}

CMD_CLEARCASE()
{
   if [ $Traduction != 0 ]
   then
      ((label==1)) && CHECKOUT $dir 1>/dev/null 2>&1
      ((label==1)) && [ -f $dir.trad_kernel.save ] && mv -f $dir.trad_kernel.save $dir
      ((label==0)) && [ -f $dir.trad_kernel.save ] && mv -f $dir.trad_kernel.save $dir.traduit
    #  ((label==1)) && MKLABEL -nc Traduction_Kernel_Anglais $dir 1>/dev/null 2>&1
   fi
}

A_FAIRE()
{
   Label=`cleartool describe $dir | grep "Traduction_Kernel_Anglais"`
   if [ "$Label" != "" ]
   then
      a_faire=0
      Nb_Cerr=`grep -Ei "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | wc -l`
      ((nb_cas=nb_cas+Nb_Cerr))
   else
      if [ "`grep "^$dir$" $TRUST_TMP/Fichier_deja_fait_Traduction_Kernel.tmp`" != "" ]
      then
         a_faire=0
         Nb_Cerr=`grep -Ei "Cerr|Cout|^[ ]*<<.*\".*\"" $dir | grep -Eiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | wc -l`
         ((nb_cas=nb_cas+Nb_Cerr))
      else
         a_faire=1
      fi
   fi
}

CHANGEMENT_REP()
{
   rep=`cleartool describe * | grep -i "version " | grep -v "make.include" | $TRUST_Awk -F@@ '{print $1}' | $TRUST_Awk -F\" '{print $2}'`
   for dir in $rep
   do
      if [ -d $dir ]
      then
         \cd $dir 2>/dev/null
         CHANGEMENT_REP
         \cd ../
      else
         A_FAIRE
	 rm -f $dir.trad_kernel.save $dir.trad_kernel.tmp
         ((a_faire)) && TRADUCTION
	 ((a_faire)) && CMD_CLEARCASE
         rm -f $dir.trad_kernel.save $dir.trad_kernel.tmp
      fi
   done
}

# defaut :
nb_argument=$#
nb_arg=0
verbose=0
label=1
rep=""
supprime_rep=""

echo "Usage : `basename $0` [-h, --help] [-nolabel] [-f file] [-dir rep|all]"
\cd $TRUST_ROOT

while (( nb_arg < nb_argument ))
do
   [ ${#1} != 0 ] && arg=$1
   case $arg in
      -h)		HELP && exit ;;
      --help)		HELP && exit ;;
      -nolabel)		label=0 && shift ;;
      -dir)		supprime_rep=$2 && shift && shift && ((nb_arg=nb_arg+1)) ;;
      -f)		rep=$2
                        if [ "`find . -name $rep | grep -v 'MonoDir'`" != "" ]
			then
			   rep=`find . -name $rep | grep -v 'MonoDir'`
			else
			   echo "" && echo "Le fichier \"$rep\" n'existe pas dans $TRUST_ROOT" && HELP && exit
			fi
			shift && shift && ((nb_arg=nb_arg+1)) ;;
      *)		echo ""
      			echo "Argument $arg non compris"
			HELP && exit ;;
   esac
   ((nb_arg=nb_arg+1))
done

[ "$supprime_rep" != "" ] && rep=`cat env/rep.TRUST | sed "s?\.\/??g" | grep -v "$supprime_rep" | $TRUST_Awk -F"/" '{print $1}' | sort -u`
[ "$rep" == "" ] && \cd Kernel && rep=`cleartool describe * | grep -i "version " | grep -v "make.include" | $TRUST_Awk -F@@ '{print $1}' | $TRUST_Awk -F\" '{print $2}'`
nb_cas=0
[ "$nb_cas_tot" == "" ] && nb_cas_tot=`grep -rEi "Cerr|Cout|^[ ]*<<.*\".*\"" $rep | grep -rEiv "//.*Cerr|//.*Cout|//.*^[ ]*<<.*\".*\"" | grep -rEiv "o concorde" | grep -rEiv "instancie_" | grep -rEiv "\.unloaded" | grep -rEiv "\.contrib" | grep -rEiv "\.keep" | grep -rEiv "\.cpp~|\.h~" | wc -l`

for dir in $rep
do
   if [ -d $dir ]
   then
      \cd $dir 2>/dev/null
      CHANGEMENT_REP
      \cd ../
   else
      A_FAIRE
      rm -f $dir.trad_kernel.save $dir.trad_kernel.tmp
      ((a_faire)) && TRADUCTION
      ((a_faire)) && CMD_CLEARCASE
      rm -f $dir.trad_kernel.save $dir.trad_kernel.tmp
   fi
done
