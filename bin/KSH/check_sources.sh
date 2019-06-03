#!/bin/bash
# Check TRUST sources in the current directory 
erreur()
{
   [ "$1" != 0 ] && err=1
}

check_dependancy()
{
      file=`pwd`/make.include
      relative_path=${file#$TRUST_ROOT}
      mes=""
      # Premiere regle: Certains modules ne peuvent dependre que du Kernel et MAIN et/ou d'eux memes. Exemples:
      modules="Kernel ThSol"
      for module in $modules
      do    
         # Le make.include appartient t'il au module ?
         if [ "`echo $relative_path | grep /$module/ `" != "" ]
	 then  
	    # Si oui, a t'il une dependance vers un module autre que Kernel ou lui meme ?
            dependances=`grep "\-I\\\$(TRUST_ROOT)" $file | grep -v "/src/Kernel[\/]" | grep -v "/$module[\/]" | grep -v include/backward | grep -v "/src/MAIN[\/]"`
            [ ${#dependances} != 0 ] && mes="Error: The file $file is not possible ! You CAN NOT have a dependance of the module $module from these other TRUST modules: $dependances" && break
         fi
      done
      # Seconde regle: Certains modules ne doivent pas etre des dependances d'autres modules. Exemples: 
      modules="Interfaces Second_Gradient Rayonnement Rayonnement_semi_transp"
      for module in $modules
      do    
         # Le make.include appartient t'il au module ?
         if [ "`echo $relative_path | grep /$module/ `" = "" ]
	 then  
	    # Si non, a t'il une dependance vers le module en question ?
            dependances=`grep "\-I\\\$(TRUST_ROOT)" $file | grep "/$module[\/]"`
            [ ${#dependances} != 0 ] && mes="Error: The file $file is not possible ! You CAN NOT have a dependance of the module `basename ${file%/make.include}` from the module $module." && break
         fi
      done      
      # Troisieme regle: Certains modules ne doivent pas dependre des modules discretisation (VDF,VEF,...). Exemples:
      modules="ThHyd"
      for module in $modules
      do
         # Le make.include appartient t'il au module ?
         if [ "`echo $relative_path | grep /$module/ `" != "" ] && [ "`echo $relative_path | grep -e /VDF[\/] -e /VEF[\/] -e /EF[\/] -e /PolyMAC[\/]`" = "" ]
         then
             dependances=`grep "\-I\\\$(TRUST_ROOT)" $file | grep -e "/VDF[\/]" -e "/VEF[\/]" -e "/EF[\/]" -e "/PolyMAC[\/]"`
             [ ${#dependances} != 0 ] && mes="Error: The file $file is not possible ! You CAN NOT have a dependance of the module $module to a discretization module like VDF or VEF." && break
         fi
      done
      # Quatrieme regle: un module discretisation ne peut dependre d'un autre module discretisation
      modules="VDF VEF EF PolyMAC"
      for module in $modules
      do
         # Le make.include appartient t'il au module ?
         if [ "`echo $relative_path | grep /$module/ `" != "" ]
         then
            # Si oui, a t'il une dependance vers une autre discretisation ?
	    for dis in $modules
	    do
	       if [ $dis != $module ]
	       then
                  dependances=`grep "\-I\\\$(TRUST_ROOT)" $file | grep -e "/$dis[\/]"`
                  [ ${#dependances} != 0 ] && mes="Error: The file $file is not possible ! You CAN NOT have a dependance of the module $module to the discretization module $dis." && break
	       fi
	    done
         fi
      done
      # Gestion des erreurs
      if [ ${#mes} != 0 ] && [ "$1" != "make.include" ]
      then
         echo
         echo $ECHO_OPTS $mes
         exit 1
      fi
}


check_src_in_gc()
{
   ############
   # Git checks
   ############
   $TRUST_ROOT/bin/GIT/check_sources_GIT.sh ; erreur $?
}

check_recent_src()
{
    file=$1
	indent_file.sh $file
    # pas daccumulation de <<  sinon cela met des heures avec gcc 6.6.1 #
    #   gros_pipe=`awk -F\<\< '{if (NF>19) {print NF $0}}' $file`
    #gros_pipe=`awk -F\<\< '{if (NF>1) { te=$0;a=gsub(";","ok",te); if (((NF+old)>19)&&(a!=0))  {print ("line:",FNR, "number",NF+old, $0)}  ; if ((NF>1)&&(a==0)) { old=NF+old } else { old =0 } }}' $file`
    gros_pipe=`awk -F\<\< '{old=0;if (NF>1) { te=$0;a=gsub(";","ok",te); if (((NF+old)>16)&&(a!=0))  {print ("line:",FNR, "number",NF+old, $0)}  ; if ((NF>1)&&(a==0)) { old=NF+old } else { old =0 } }}' $file`


    [ "$gros_pipe" != "" ] && echo $file two many pipes $gros_pipe && erreur 1

    #############################################################
    # Interdiction de entier au lieu de int a partir de la v1.6.8
    #############################################################
    grep -e "[$,( ]entier[& ]" $file && echo "-> Error in $file: entier type should be replace by int type" && echo && erreur 1
    
    ################################################
    # Only UTF-8 or ASCII encoding since v1.7.6
    ################################################
    #grep "[àâçéèêëïîôùûüÿ]" $file && echo "-> Error in $file: French accents are not supported anymore ! Please, consider writing comments or messages in English." && echo && erreur 1
    #[ "`file $file | grep ISO`" != ""  ]&& echo "-> Error in $file: French accents are not supported anymore ! Please, consider writing comments or messages in English." && echo && erreur 1
    ([ "`file -i $file | grep ascii`" == "" ] && [ "`file -i $file | grep utf-8`" == "" ] && [ -s $file ])  && echo "-> Error in $file: only ASCII or UTF-8 encoding is accepted for source files! Please, consider writing comments or messages in English and/or save your file in UTF8." && echo && erreur 1

    ###############################################################
    # Interdiction du francais dans les messages d'erreur du Kernel
    ###############################################################
    if [ "`echo $PWD | grep /TRUST/src/Kernel/`" != "" ]
	then
	$TRUST_ROOT/bin/KSH/forbid_french.ct $file
	erreur $?
    fi

    #################################################
    # Interdiction des include sous la forme "toto.h"
    #################################################
    ok=`grep include $file | grep "#" | grep "\"*\"" 2>/dev/null`
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
   res=`grep -En "\(.*& *\)" *.cpp 2>/dev/null | grep -v "\:\:"| $TRUST_Awk -F\/\/ '{print $1}' | grep -v Eval_ |grep -v "iter.reconstructeur()"| $TRUST_Awk  '{if (NF>1) {print $0}}' ` 
   if [ "$res" != "" ]
   then
      echo $res
      echo $ECHO_OPTS "Vous devez corriger les cast en dur en utilisant ref_cast ou ref_cast_non_const dans le(s) fichier(s):"
      grep -En "\(.*& *\)" *.cpp | grep -v "\:\:"| $TRUST_Awk -F\/\/ '{print $1}' | grep -v Eval_ | grep -v "iter.reconstructeur()"| $TRUST_Awk '{if (NF>1) {print $0}}'| $TRUST_Awk -F: '{print $1}' | sort -u
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
   res=`grep "exit(" *.cpp *.h 2>/dev/null | grep -v "exit()" | grep -v "Process." | grep -v "process."`
   if [ "$res" != "" ]
   then
      grep "exit(" *.cpp *.h | grep -v "exit()"
      echo "Regle de programmation TRUST:"
      echo "Utiliser Process::exit() au lieu du exit(int) systeme."
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
}

check_all()
{
  # Used by Baltik projects
  if [ 1 -eq 1 ]
  then
    for file in `ls *.cpp *.h *.c 2>/dev/null` 
    do
       # ls 1>/dev/null
       check_recent_src $file
    done
  fi
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
   [ $TRUST_ARCH != linux ] && exit 0
   # make.include detectes we are in a TRUST source directory
   org=`pwd`
   reffile=".check_sources.ok"
   [ "$1" != "" ] && cd $1 && reffile=$org"/check_sources.ok"
   if [ -f make.include ]
   then
      err=0
      ############################################
      # Pour ne pas executer trop souvent check.sh
      # On cherche si des fichiers sources sont plus recents
      # Donc on exclue les repertoires avec -F | grep -v "/"
      # Donc on exclue les .o avec grep -v "\.o$"
      ############################################  
      [ "`\ls -AFt | grep -v / | grep -v "\.o$"  | grep -v "CMakeLists.txt" | head -1`" = "${reffile}" ] && [ ${reffile} -nt $0 ] && exit 0
      # Build a list of newer files than ${reffile} or this script to speed up some tests (eg: french check)
      if [ ! -f ${reffile} ] || [ $0 -nt ${reffile} ]
      then
	 newer_files=`\ls make.include *.cpp *.h *.c 2>/dev/null | grep -v / | grep -v "\.o$"`
	 new_newer_files=$newer_files
      else
	 newer_files=`find * -type f  \( -name make.include -o -name '*'.cpp -o -name '*'.h -o -name '*'.c \) -newer ${reffile} | grep -v / | grep -v "\.o$"`
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
	 # Interdiction de briser la dependance des modules
	 if [ $file = make.include ]
	 then
            check_dependancy || exit 1
	 else
	    check_recent_src $file
	 fi
      done   

      ##################
      # Check .P files #
      ##################
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
