#!/bin/bash
sed "s/\\\\-\\\\//g;s/,//;s/{\\\bf//;s/\\\\_/_/g" doc/doc.ind  | awk '{ if ($1=="\\item") {print "|"$2}}' | sort -u  > Keywords.txt
# pour supprimer les lignes contenant "|\hyperpage{99}," et "|{" -> sinon probleme avec nedit
# pour supprimer les lignes contenant "|/*" et "|\#" -> sinon probleme avec gedit
sed -i "/hyperpage/d; /{/d; /*/d; /#/d" Keywords.txt
