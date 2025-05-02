#!/bin/bash
# supprime les mots cles en _base et _deriv car correspondent a des classes XD et non a des mots cles utilisables depuis jdd
sed "s/\\\\-\\\\//g;s/,//;s/{\\\bf//;s/\\\\_/_/g" doc/doc.ind  | awk '{ if ($1=="\\item") {print "|"$2}}'  | grep -v "_base" | grep -v "_deriv$" | sort -u  > Keywords.txt
# pour supprimer les lignes contenant "|\hyperpage{99}," et "|{" -> sinon probleme avec nedit
# pour supprimer les lignes contenant "|/*" et "|\#" -> sinon probleme avec gedit
sed -i "/hyperpage/d; /{/d; /*/d; /#/d" Keywords.txt

