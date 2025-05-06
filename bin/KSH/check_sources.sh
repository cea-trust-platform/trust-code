#!/bin/bash
# Check TRUST sources in the current directory 
erreur()
{
   [ "$1" != 0 ] && err=1
}

check_src_in_gc()
{
   ############
   # Git checks
   ############
   # Source files should be tracked:
   #if [ -d $TRUST_ROOT/.git ]
   git tag 1>/dev/null 2>&1
   if [ $? = 0 ]
   then
      pwd_=`pwd` && git status -s *.cpp *.h *.f *.c *.P 2>/dev/null | grep -v config.h | $TRUST_Awk -v dir=$pwd_ 'BEGIN {err=0}
   !/~/ && ($1=="??") {err=1;print "\nThe file "dir"/"$2" is not staged under Git!\nStage the file with \"git add "dir"/"$2"\" or delete this file."}
   END {exit err}' || erreur 1
   fi
}

check_recent_src()
{
    file=`ls -l $1 | awk '{print $NF}'` # Pour pointer sur le bon fichier si lien dans Baltik
    indent_file.sh $file
    
    # pas daccumulation de <<  sinon cela met des heures avec gcc 6.6.1 #
#    gros_pipe=`awk -F\<\< '{old=0;if (NF>1) { te=$0;a=gsub(";","ok",te); if (((NF+old)>16)&&(a!=0))  {print ("line:",FNR, "number",NF+old, $0)}  ; if ((NF>1)&&(a==0)) { old=NF+old } else { old =0 } }}' $file`
#    [ "$gros_pipe" != "" ] && echo $file two many pipes $gros_pipe && erreur 1

    #############################################################
    # Interdiction de entier au lieu de int a partir de la v1.6.8
    #############################################################
#    grep -e "[$,( ]entier[& ]" $file | grep -v "XD attr" && echo "-> Error in $file: entier type should be replace by int type" && echo && erreur 1
    
    ################################################
    # Only UTF-8 or ASCII encoding since v1.7.6
    ################################################
    #grep "[àâçéèêëïîôùûüÿ]" $file && echo "-> Error in $file: French accents are not supported anymore ! Please, consider writing comments or messages in English." && echo && erreur 1
    #[ "`file $file | grep ISO`" != ""  ]&& echo "-> Error in $file: French accents are not supported anymore ! Please, consider writing comments or messages in English." && echo && erreur 1
    [ `uname -s` != "Darwin" ] && file_opt="-i"
	([ "`file $file_opt $file | grep -i ascii`" == "" ] && [ "`file $file_opt $file | grep -i utf-8`" == "" ] && [ -s $file ])  && echo "-> Error in $file: only ASCII or UTF-8 encoding is accepted for source files! Please, consider writing comments or messages in English and/or save your file in UTF8." && echo && erreur 1

    ###############################################################
    # Interdiction du francais dans les messages d'erreur du Kernel
    ###############################################################
    if [ "`echo $PWD | grep /TRUST/src/Kernel/`" != "" ]
    then
       $TRUST_ROOT/bin/KSH/forbid_french.ct $file
       erreur $?
    fi
    # Macro _OPENMP -> _OPENMP_TARGET
    grep " _OPENMP" $file
    [ $? = 0 ] && echo "Replace in $file: _OPENMP macro by TRUST_USE_GPU" && erreur 1

    #################################################
    # Interdiction des include sous la forme "toto.h"
    #################################################
    ok=`grep '#include "' $file 2>/dev/null`
    if [ "$ok" != "" ]
    then
       echo "**************************************************************"
       echo "An include file is written under the form: #include \"toto.h\""
       echo "It should be written like: #include <toto.h>"
       echo "**************************************************************"
       erreur 1
    fi  

    ####################################################################
    # Check source header: (Baltik rep="" so not checked yet for Baltik)
    ####################################################################
    [ "$rep" != "" ] && check_en_tete $rep/$file     
}

check_update_P()
{
  #######################################################################
   # Dans chaque repertoire on genere les fichiers a partir des patrons .P
   #######################################################################
 
   check=""
   [ "$1" = "-check" ] && check="-check"&& shift
   files_P=$*
   for file in $files_P
   do
     #echo $file
     #process_generated_source.sh -check $file
     process_generated_source.sh $check $file
     status=$?
     erreur $status
     [ $status -ne 0 ] && echo "execute: cd $PWD;process_generated_source.sh $file"
   done
}

check_src()
{  
   ##############################
   # Verification des cast en dur
   ##############################
   grp_cmd=
   res=`grep -En "\(.*& *\)" *.cpp 2>/dev/null | grep -v "::"| $TRUST_Awk -F\/\/ '{print $1}' | grep -v catch | grep -v Eval_ |grep -v "iter.reconstructeur()"|grep -v ":[ ]*\*"|grep -v ":/\*!"| $TRUST_Awk  '{if (NF>1) {print $0}}' ` 
   if [ "$res" != "" ]
   then
      echo $res
      echo $ECHO_OPTS "Vous devez corriger les cast en dur en utilisant ref_cast ou ref_cast_non_const dans le(s) fichier(s):"
      grep -En "\(.*& *\)" *.cpp | grep -v "\:\:"| $TRUST_Awk -F\/\/ '{print $1}' | grep -v Eval_ | grep -v "iter.reconstructeur()"|grep -v ":[ ]*\*"|grep -v ":/\*!"| $TRUST_Awk '{if (NF>1) {print $0}}'| $TRUST_Awk -F: '{print $1}' | sort -u
      erreur 1
   fi

   ############################
   # Verification des same_type
   ############################
   res=`grep same_type *.cpp 2>/dev/null` 
   if [ "$res" != "" ]
   then
      grep same_type *.cpp
      echo "Vous devez corriger les same_type par des sub_type."
      erreur 1
   fi

   #######################
   # Verification des exit
   #######################
   res=`grep "exit(" *.cpp *.h 2>/dev/null | grep -v "exit()" | grep -v "Process" | grep -v "std::atexit"` 
   if [ "$res" != "" ]
   then
      echo $res
      echo "Regle de programmation TRUST:"
      echo "Utiliser Process::exit() au lieu du exit(int) systeme."
      erreur 1
   fi     

   #######################
   # Verification des NULL
   #######################
   res=`grep "NULL" *.cpp *.h 2>/dev/null | grep -v "_NULL" | grep -v "NULL_" | grep -v "NULLE" | grep -v "\"NULL\""`
   if [ "$res" != "" ]
   then
      echo $res
      echo "Regle de programmation c++:"
      echo "Utiliser nullptr au lieu de NULL"
      erreur 1
   fi

   #######################
   # Verification des NULL
   #######################
   res=`grep "REF(" *.cpp *.h 2>/dev/null | grep -v "#define REF(_"`
   if [ "$res" != "" ]
   then
      echo $res
      echo "Changement dans TRUST 1.9.5:"
      echo "Remplacer REF(classe) par OBS_PTR(classe)"
      erreur 1
   fi

   #######################################
   # Verification des declare_instanciable
   #######################################
   res=`grep -i "declare_instanciable" *.cpp 2>/dev/null`
   if [ "$res" != "" ]
   then
      grep -i "declare_instanciable" *.cpp
      echo "Vous ne devez pas declarer de classes instanciables dans un fichier .cpp mais uniquement dans un fichier .h."
      echo "Sinon, le fichier instancie.cpp n'instanciera pas votre classe qui pourra alors ne pas etre reconnue lors de l'execution."
      erreur 1
   fi     

   #####################################
   # Verification des includes obsoletes
   #####################################
   res=`grep -e "<fstream\.h>" -e "<iostream\.h>" -e "<new\.h>" -e "<strstream\.h>" *.cpp *.h 2>/dev/null` 
   if [ "$res" != "" ]
   then
      grep -e "<fstream\.h>" -e "<iostream\.h>" -e "<new\.h>" -e "<strstream\.h>" *.cpp *.h 2>/dev/null
      echo "Vous ne devez plus utiliser ces includes systemes qui sont obsoletes et n'existent plus sur GNU 4.3.x"
      echo "Utiliser strstream, new, fstream, ou iostream a la place."
      erreur 1
   fi

   #######################################################################################################
   # Verification que les Interprete qui modifient un domaine heritent bien de Interprete_geometrique_base
   #######################################################################################################
   for cpp in `$TRUST_Awk '/implemente_/ && /Interprete\)/ {print FILENAME}' *.cpp 2>/dev/null | sort -u`
   do
      if [ $cpp != Interprete_geometrique_base.cpp ] && [ "`$TRUST_Awk '!/const / && /Domaine/ && /=/ && /objet\(/ {print FILENAME}' $cpp`" != "" ]
      then
	 if [ "`grep implemente_instanciable $cpp | grep Interprete_geometrique_base`" = "" ] || [ "`grep 'associer_domaine(' $cpp`" = "" ]
	 then
           echo "La classe codee dans le fichier $cpp doit heriter de la classe Interprete_geometrique_base"
           echo "et non de la classe Interpreter. Elle modifie un domaine et doit donc beneficier des appels"
           echo "de methodes effectues dans Interprete_geometrique_base::interpreter(Entree&). Corriger afin de pouvoir compiler."
           erreur -1
	 fi
      fi
   done  

   ################################################################
   # Verification que implemente_ et declare_ ne sont plus utilises
   ################################################################
   if [ "`grep -e declare_ -e implemente_ *.cpp *.h 2>/dev/null | grep -e '_base(' -e '_base_sans_' -e '_instanciable(' -e '_instanciable_sans_' -e '_ref(' -e '_liste(' -e '_deriv(' -e '_vect(' | grep -v __dummy__ | grep -v '#define'`" != "" ]
   then
      grep -e declare_ -e implemente_ *.cpp *.h | grep -e '_base(' -e '_base_sans_' -e '_instanciable(' -e '_instanciable_sans_' -e '_ref(' -e '_liste(' -e '_deriv(' -e '_vect(' | grep -v __dummy__ | grep -v '#define'
      echo "********************************************************************************************"
      echo "Des fichiers utilisent des macros declare_XXX(YYY) et implemente_XXX(YYY) bientot obsoletes." 
      echo "Utiliser a la place Declare_XXX(YYY); et Implemente_XXX(YYY);"
      echo "********************************************************************************************"
      erreur -1
   fi 

   #########################################################################
   # On lance cpplint pour detecter les caracteres accentues non supportes #
   #########################################################################
   non_ascii_sources=`file *.cpp *.h | grep -v ASCII | awk -F: '{print $1}'`
   if [ "$non_ascii_sources" != "" ] && [ $TRUST_HOST_ADMIN = $HOST ]
   then
      # On ne lance que sur la machine TMA pour le moment
      command="python $TRUST_ROOT/bin/KSH/cpplint.py $non_ascii_sources 2>&1"
      if [ "`eval $command | grep UTF`" != "" ]
      then
	 echo "Several files MUST be fixed to suppress invalid UTF-8 caracters (Surely French accents? Check below...):"
	 eval $command | grep UTF
	 echo 
	 erreur 1
      fi
   fi  

   ###################################################
   # Interdiction des const reference vers type simple
   ###################################################
#   grep -e "^[ ]*const int& [A-Za-z0-9_]*=" -e "^[ ]*const double& [A-Za-z0-9_]*=" *.cpp *.h
#   if [ "``" != "" ]
#   then
#      grep -e "const int&" -e "const double&" *.cpp *.h
#      echo "********************************************************************************************"
#      echo "Remplacer dans ces fichiers les const reference \"const int|double&\" par \"const int|double \"" 
#      echo "********************************************************************************************"
#      erreur -1
#   fi
}

check_all()
{
  # Used by Baltik projects
  for file in `ls *.tpp *.cpp *.h *.c 2>/dev/null`
  do
     # ls 1>/dev/null
     check_recent_src $file
  done
  
  check_src_in_gc
    
  check_update_P -check `\ls *.P 2>/dev/null`

  check_src
# check_file.sh -print || exit -1  
}

#########
# START #
#########
if [ "`basename $0`" != "bash" ] && [ "`basename $0`" != "baltik_check_sources" ] && [ "$TRUST_DISABLE_CHECK_SRC" != "1" ]
then
   [ $TRUST_ARCH != linux ] && [ $TRUST_ARCH != darwin ] && exit 0
   org=`pwd`
   reffile=".check_sources.ok"
   [ "$1" != "" ] && cd $1 && reffile=$1"/.check_sources.ok"
   res=`ls *.cpp *.tpp *.h *.hpp 2>/dev/null`
   if [ "x$res" != "x" ]
   then
      err=0
      ############################################
      # Pour ne pas executer trop souvent check.sh
      # On cherche si des fichiers sources sont plus recents
      # Donc on exclue les repertoires avec -F | grep -v "/"
      # Donc on exclue les .o avec grep -v "\.o$"
      # Le deuxieme test ${reffile} -nt $0 verifie aussi que le script check_source.sh lui-meme n a pas ete change. 
      ############################################  
      [ "`\ls -AFt | grep -v / | grep -v "CMakeLists.txt" | head -1`" = "${reffile}" ] && [ ${reffile} -nt $0 ] && exit 0
      # Build a list of newer files than ${reffile} or this script to speed up some tests (eg: french check)
      if [ ! -f ${reffile} ] || [ $0 -nt ${reffile} ]
      then
         # check_source.ok n existe pas encore, on prend toutes les sources
         newer_files=`\ls *.cpp *.h *.tpp *.hpp *.c 2>/dev/null | grep -v / | grep -v "\.o$"`
         new_newer_files=$newer_files
      else
         # check_source.ok existe, on prend les sources plus recentes:
         newer_files=`find . -type f  \( -name '*'.cpp -o -name '*'.h -o -name '*'.tpp -o -name '*'.c -o -name '*'.hpp \) -newer ${reffile}`
         # Different list including everything (like .P files, etc):
         new_newer_files=`find . -maxdepth 1 -type f -newer ${reffile} | grep -v CMakeLists.txt` 
      fi
      [ "$new_newer_files" = "" ] && exit 0

      mk_Instancie
      #############
      # Git check #
      #############
      check_src_in_gc

      ################################
      # Check on the more recent files
      ################################
      rep=`pwd`
      for file in $newer_files
      do
        check_recent_src $file
      done   

      ##################
      # Check .P files #
      ##################
      # [ABN] 04/2024: .P have disappeared this could be removed:
      opt_newer=""
      [ -f  ${reffile} ] && opt_newer=" -newer ${reffile}"
      check_update_P `find . -maxdepth 1 -name '*.P' ${opt_newer} -exec basename {} \;`

      ########################################################
      # Dans chaque repertoire on execute check.sh s'il existe
      ########################################################
      if [ -f check.sh ]
      then
         ./check.sh
         erreur $?
      fi

      #################
      # Check sources #
      #################
      check_src 

      ######################
      # Check naming rules #
      ######################
      check_file.sh -print || exit -1

      ###################
      # Ajout de cppcheck 
      ###################
      #cppcheck -q -v -a -s *.cpp *.h

      #######################
      # Fin des verifications
      #######################
      cd $org

      if [ $err = 1 ]
      then
        rm -f ${reffile}
          exit 1
      else
         date >  ${reffile}   
         #[ "$1" != "" ] && touch check_sources.cpp
         exit 0
      fi
      cd $org
   fi
else
   [ "$TRUST_DISABLE_CHECK_SRC" = "1" ] && echo "No check sources"
fi
