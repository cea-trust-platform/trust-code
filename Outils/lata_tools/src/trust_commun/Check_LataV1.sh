#!/bin/bash
# Ce fichier sert a checker le plugin lata V1 avec la version de TRUST

Verif_LATAV1()
{
   for ext in h cpp
   do
	[ ! -f $TRUST_ROOT/Kernel/Utilitaires/LataV1_field_definitions.$ext ] && echo "le fichier $TRUST_ROOT/Kernel/Utilitaires/LataV1_field_definitions.$ext n'existe plus !" && echo "Cette methode Verif_LATAV1() n'a plus lieu d'etre..." && exit 1
	[ ! -f LataV1_field_definitions.$ext ] && echo "le fichier `pwd`/LataV1_field_definitions.$ext n'existe plus !" && echo "Cette methode Verif_LATAV1() n'a plus lieu d'etre..." && exit 1
	
	sed '1,7 d' $TRUST_ROOT/Kernel/Utilitaires/LataV1_field_definitions.$ext > file.tmp
	difference=`diff file.tmp LataV1_field_definitions.$ext`
	if [ "$difference" != "" ]
	then
	   blanc="          " && [ "$ext" == "cpp" ] && blanc="        "
	   echo "Erreur : les fichiers"
	   echo "$TRUST_ROOT/Kernel/Utilitaires/LataV1_field_definitions.$ext"
	   echo "et"
	   echo "`pwd`/LataV1_field_definitions.$ext"
	   echo "sont differents"
	   echo ""
	   echo "Pour un bon fonctionnement, il est imperatif que les fichiers"
	   echo "LataV1_field_definitions.h et LataV1_field_definitions.cpp"
	   echo "soient copies a l'identique dans le repertoire"
	   echo "$TRUST_ROOT/Kernel/Utilitaires"
	   echo ""
	   echo "version TRUST de LataV1_field_definitions.$ext$blanc| version lata2dx de LataV1_field_definitions.$ext"
	   sdiff -s -w 115 file.tmp LataV1_field_definitions.$ext
	   rm -f file.tmp
	   exit 1
	fi
	rm -f file.tmp
   done
}

Verif_LATAV1
