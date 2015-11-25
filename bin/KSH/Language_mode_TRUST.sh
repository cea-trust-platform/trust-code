#!/bin/bash

#########################################################################################################################################
#																	#
#               Ce programme sert a definir pour l'editeur Nedit, le language des jeux de donnees TRUST.				#
#																	#
#########################################################################################################################################



#########################################################################################################################################
# Environnement
verbose=1 && [ "$1" = -no_verbose ] && verbose=0
PATH_LANGUAGE=`pwd`
aujourdhui=`date +"%D-a-%Hh-%Mmin" | awk -F'/' '{print "copie-du-"$2"-"$1"-"$3}'`



#########################################################################################################################################
# Je recherche le numero de version de Nedit (s'il existe)
NEdit_exe=`whereis -b nedit | awk '{print $2}' 2>/dev/null`
if [ "$NEdit_exe" != "" ]
then
   Version511=`grep '5\.1\.1' $NEdit_exe`
   Version53=`grep '5\.3' $NEdit_exe`
   Version54=`grep '5\.4' $NEdit_exe`
   Version55=`grep '5\.5' $NEdit_exe`
   Version56=`grep '5\.6' $NEdit_exe`
   if [ "$Version511" != "" ] || [ "$Version53" != "" ]
   then
      Indice_Version=0
   else
      if [ "$Version54" != "" ] || [ "$Version55" != "" ] || [ "$Version56" != "" ]
      then
  	 Indice_Version=1
      else
         if [ $verbose  = 1 ]
	 then
	    echo "The current version of Nedit is not supported."
	 fi
	 exit
      fi
   fi
else
   if [ $verbose = 1 ]
   then
      echo "There is no version of Nedit."
   fi
   exit
fi



#########################################################################################################################################
# Si l'environnement TRUST est charge, je continue
if [ "$TRUST_ROOT" != "" ]
then
   #
   # Si le fichier "$HOME/.nedit/nedit.rc" existe alors c'est le fichier de configuration Nedit
   if [ -f $HOME/.nedit/nedit.rc ]
   then
      Nedit_file=$HOME/.nedit/nedit.rc
   else
   # Sinon, si le fichier "$HOME/.nedit" existe alors c'est le fichier de configuration Nedit
      if [ -f $HOME/.nedit ]
      then
         Nedit_file=$HOME/.nedit
      else
      # Sinon, le fichier de configuration Nedit n'existe pas
         if [ $verbose = 1 ]
	 then
           echo "There is no Nedit configuration file."
           echo "To create this file :	- Run \"nedit\""
           echo "			- In the menu \"Preferences->Save Defaults...\""
           echo "			- Click on \"OK\" and quit nedit"
         fi
	 exit
      fi
   fi
   echo "Configure Nedit: $Nedit_file"
   # Add tags to the Nedit configuration
   if [ "`grep 'nedit.tagFile: tags' $Nedit_file`" = "" ]
   then
      echo "Nedit has been configured to used TRUST tags."
      echo "nedit.tagFile: tags" >> $Nedit_file
   fi

   # Si le fichier "$TRUST_TESTS/doc/TRUST/Keywords.txt" est plus recent que le fichier de configuration Nedit
   # ou si je ne trouve pas "TRUST:1:" dans le fichier de configuration Nedit
   # alors je continue la mise a jour, en faisant une copie du fichier de configuration nedit
   fichier_index_keyword=$TRUST_ROOT/doc/TRUST/Keywords.txt
   if [ $fichier_index_keyword -nt $Nedit_file ] || [ "`grep 'TRUST:1:' $Nedit_file`" = "" ]
   then
      KeywordsTRUST=`$TRUST_Awk -F"|" '!/\|/ {k=k" "$1} /\|/ {if ($1!="" && gsub("XXX","",$1)==0) k=k$1"|";k=k$2"|"} END {print k}' $fichier_index_keyword`
      rm -f $Nedit_file.*
      mv -f $Nedit_file $Nedit_file.$aujourdhui
      cp -f $Nedit_file.$aujourdhui $Nedit_file
   else
   # Sinon, j'arrete le process
      exit
   fi
else
# Sinon, j'arrete le process
   if [ $verbose = 1 ]
   then
      echo "TRUST environment not initialized."
   fi
   exit
fi

#########################################################################################################################################
# Je cree la variable "Comment" definissant les commentaires dans TRUST
if (( Indice_Version == 0 ))
then
   Comment='		Comment:"#[\\s\\r|\\n]":"[\\s|\\n]#"::Comment::\n\'
else
   Comment='		Comment:"(?<!\\Y)#[\\s\\r|\\n]":"[\\s|\\n]#"::Comment::\n\'
fi



#########################################################################################################################################
# Je cree la variable "Keyword" contenant tous les mots clefs de TRUST
if (( Indice_Version == 0 ))
then
   Keyword='		Keyword:"(?i<('
else
   Keyword='		Keyword:"(?<!\\Y)(?<!\\.)(?i<('
fi


if (( Indice_Version == 0 ))
then
   end='|)>)[\\s\\n]":::Keyword::D\n\'
else
   end='|)>)(?!\\Y)[\\s\\n]":::Keyword::D\n\'
fi

Keyword=`echo "$Keyword$KeywordsTRUST$end"`

#########################################################################################################################################
# Je cree la variable "Problem" definissant les erreurs syntaxique dans TRUST
Problem='		Problem:"\\{\\w|\\}\\w|\\w\\{|\\w\\}|#\\w|\\w#|\\};|;\\}|\\},|,\\}|\\{;|;\\{|\\{,|,\\{|##|\\{}|\\.\\{|\\.\\}|\\{\\.|\\}\\.|":::Problem::D\n\
		Comment Problem:"(\\w#|#\\w|##)[\\s\\w\\n]":::Problem:Comment:\n\'



#########################################################################################################################################
# Je cherche le numero de ligne de "nedit.languageModes" dans $Nedit_file et le numero de ligne de "Keyword:black"
num_ligne=`grep -n 'nedit.languageModes' $Nedit_file | awk -F: '{print $1}'`
num_ligne=`awk 'BEGIN{print '$num_ligne'-1}'`
num_ligne_tot=`wc -l $Nedit_file | awk '{print $1}'`
num_ligne_style=`grep -n 'Keyword:b' $Nedit_file | awk -F: '{print $1}'`

# Je cherche si le mode highlightPatterns TRUST existe dans $Nedit_file
L_mode1=`grep -n 'TRUST:1:' $Nedit_file | awk -F: '{print $1}'`
# Je cherche si le mode languageModes TRUST existe dans $Nedit_file
L_mode2=`grep -n 'TRUST:.d' $Nedit_file | awk -F: '{print $1}'`



#########################################################################################################################################
# Si le mode highlightPatterns TRUST n'existe pas alors je mets dans neditrc tout le debut du fichier
# jusqu'a "nedit.languageModes" et j'ajoute les variables "$Comment" "$Keyword" & "$Problem"
if [ "$L_mode1" == "" ]
then
   neditrc=`[ "$num_ligne" != "" ] && head -$num_ligne $Nedit_file`
   neditrc=`echo "$neditrc"'\n\' ; echo '	TRUST:1:0{\n\' ; echo "$Comment" ; echo "$Keyword" ; echo "$Problem" ; echo '	}'`
else
# Sinon je mets dans neditrc, tout le debut du fichier jusqu'a "TRUST:1:"
   L_mode1=`awk 'BEGIN{print '$L_mode1'-1}'`
   neditrc=`[ "$L_mode1" != "" ] && head -$L_mode1 $Nedit_file`
   neditrc=`echo "$neditrc" ; echo '	TRUST:1:0{\n\' ; echo "$Comment" ; echo "$Keyword" ; echo "$Problem" ; echo '	}'`
fi

#########################################################################################################################################
# Si le languageModes TRUST n'existe pas alors je defini dans neditrc le languageModes TRUST
if (( Indice_Version == 0 ))
then
   TRUSTData='nedit.languageModes: 	TRUST:.data .geo::::::\n\'
   Problemred='	Problem:red:Bold\n\'
else
   TRUSTData='nedit.languageModes: 	TRUST:.data .geo:::::::\n\'
   Problemred='	Problem:red/red:Bold\n\'
fi

if [ "$L_mode2" == "" ]
then
   entre=`awk 'BEGIN{print '$num_ligne_style'-'$num_ligne'-1}'`
   fin=`awk 'BEGIN{print '$num_ligne_tot'-'$num_ligne_style'}'`
   neditrc=`echo "$neditrc" ; echo "$TRUSTData" ; grep 'nedit.languageModes' $Nedit_file | awk -F'nedit.languageModes:' '{print $2}'`
   neditrc=`echo "$neditrc" ; [ "$num_ligne_style" != "" ] && head -$num_ligne_style $Nedit_file | tail -$entre`
   # Si le style "Problem" n'existe pas, je le cree
   if [ "`grep 'Problem:red' $Nedit_file`" == "" ]
   then
      neditrc=`echo "$neditrc" ; echo "$Problemred"`
   fi
   neditrc=`echo "$neditrc" ; [ "$num_ligne_tot" != "" ] && head -$num_ligne_tot $Nedit_file | tail -$fin`
else
# Sinon je regarde ou il est situe, et je le remplace
   L_mode2=`awk 'BEGIN{print '$L_mode2'-1}'`
   entre=`awk 'BEGIN{print '$num_ligne_style'-'$L_mode2'-1}'`
   fin=`awk 'BEGIN{print '$num_ligne_tot'-'$num_ligne_style'}'`
   if (( L_mode2 == num_ligne ))
   then
      neditrc=`echo "$neditrc" ; echo "$TRUSTData"`
      neditrc=`echo "$neditrc"; [ "$num_ligne_style" != "" ] && head -$num_ligne_style $Nedit_file | tail -$entre`
      # Si le style "Problem" n'existe pas, je le cree
      if [ "`grep 'Problem:red' $Nedit_file`" == "" ]
      then
	 neditrc=`echo "$neditrc"; echo "$Problemred"`
      fi
      neditrc=`echo "$neditrc"; [ "$num_ligne_tot" != "" ] && head -$num_ligne_tot $Nedit_file | tail -$fin`
   else
      ecart=`awk 'BEGIN{print '$L_mode2'-'$num_ligne'}'`
      neditrc=`echo "$neditrc" ; [ "$L_mode2" != "" ] && head -$L_mode2 $Nedit_file | tail -$ecart ; echo "$TRUSTData"`
      neditrc=`echo "$neditrc" ; [ "$num_ligne_style" != "" ] && head -$num_ligne_style $Nedit_file | tail -$entre`
      # Si le style "Problem" n'existe pas, je le cree
      if [ "`grep 'Problem:red' $Nedit_file`" == "" ]
      then
	 neditrc=`echo "$neditrc" ; echo "$Problemred"`
      fi
      neditrc=`echo "$neditrc" ; [ "$num_ligne_tot" != "" ] && head -$num_ligne_tot $Nedit_file | tail -$fin`
   fi
fi



#########################################################################################################################################
# Je cree le nouveau fichier de configuration
echo "$neditrc" > $Nedit_file
echo "$Nedit_file updated..."
# Fin
