#!/bin/bash
#
# Traducteur Francais -> Anglais via Google.fr ou Voila.fr
# 
# Usage : Traducteur.sh $1 $2
# 
#         $1 est le fichier a traduire (le fichier.err)
#         $2 est le traducteur (Google, Voila ou Sans)
#
#         ex : Traducteur.sh VAHL_DAVIS.err Google
#
# La traduction s'effectue ligne par ligne via la methode French2English()
#
# Debut de la methode French2English()
#
French2English()
{
	texte=$1
	
	if [ "$traducteur" == "Google" ]
	then
	   # Traduction Google
	   texte_entete="http://translate.google.fr/translate_s?hl=fr&q=TEXTE&sl=fr&tl=en"
	else
	   # Traduction Voila
	   texte_entete="http://tr.voila.fr/traduction_voila.php?isText=1&translationDirection=fe&stext=TEXTE"
	fi

	# Correction espace, a, a, c, e, e, e, ', ?
	[ "`echo $ECHO_OPTS $texte | grep " "`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s? ?+?g"`
	[ "`echo $ECHO_OPTS $texte | grep "a"`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s?a?%C3%A0?g"`
	[ "`echo $ECHO_OPTS $texte | grep "a"`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s?a?%C3%A2?g"`
	[ "`echo $ECHO_OPTS $texte | grep "c"`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s?c?%C3%A7?g"`
	[ "`echo $ECHO_OPTS $texte | grep "e"`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s?e?%C3%A8?g"`
	[ "`echo $ECHO_OPTS $texte | grep "e"`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s?e?%C3%A9?g"`
	[ "`echo $ECHO_OPTS $texte | grep "e"`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s?e?%C3%AA?g"`
	[ "`echo $ECHO_OPTS $texte | grep "\'"`" != "" ] && texte=`echo $ECHO_OPTS $texte | sed "s?'?%27?g"`
	[ "`echo $ECHO_OPTS $texte | grep "?"`" != "" ]  && texte=`echo $ECHO_OPTS $texte | sed "s?\??%3F?g"`

	# Substitution
	texte_entete=`echo $ECHO_OPTS $texte_entete | sed "s?TEXTE?$texte?g"`

	# Traduction
	# Option --user-agent voir http://www.delafond.org/traducmanfr/man/man1/wget.1.html
	wget -q --user-agent=NoSuchBrowser $texte_entete 2>/dev/null

	if [ "$traducteur" == "Google" ]
	then
	   # Traduction Google
	   OK=`ls translate_s* 2>/dev/null | wc -l | $TRUST_Awk '{print $1}'`
	   translate="" && ((OK==1)) && translate=`grep "var itq=" translate_s* | $TRUST_Awk -F"var itq=\'" '{print $2}' 2>/dev/null | $TRUST_Awk -F"\';function" '{print $1}' 2>/dev/null`
	   rm -f translate_s*
	   # Correction \x27 = '
	   if [ "`echo $ECHO_OPTS $translate | grep "\\x27"`" != "" ]
	   then
	      translate=`echo $ECHO_OPTS $translate | sed "s?\\\\\x27?'?g"`
	   fi
	else
	   # Traduction Voila
	   OK=`ls traduction_voila.php* 2>/dev/null | wc -l | $TRUST_Awk '{print $1}'`
	   translate="" && ((OK==1)) && translate=`grep "txtTrad" traduction_voila.php* | $TRUST_Awk -F"txtTrad\">" '{print $2}' 2>/dev/null | $TRUST_Awk -F'<' '{print $1}' 2>/dev/null`
	   rm -f traduction_voila.php*
	fi

	if (( ${#translate} == 0 ))
	then
	   echo $ECHO_OPTS "$texte"
	else
	   echo $ECHO_OPTS "$translate"
	fi
}
#
# Debut du programme principal
#
if [ "$1" != "" ]
then
   fichier_a_traduire=$1
else
   exit
fi
if [ "$2" != "" ]
then
   traducteur=$2
else
   exit
fi

nb_ligne=0 && [ -f $fichier_a_traduire ] && nb_ligne=`wc -l $fichier_a_traduire | $TRUST_Awk '{print $1}'`
i=1
while ((i<=nb_ligne))
do
   texte=`head -$i $fichier_a_traduire | tail -1`
   translate=""
   if [ "$texte" != "" ]
   then
      translate=`French2English "$texte"`
   fi
   [ "$translate" == "" ] && translate=" "
   echo $ECHO_OPTS "$translate"
   ((i=i+1))
done
