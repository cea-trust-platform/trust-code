#!/bin/bash

#########################################################################################################################################
#																	#
#               Ce programme sert a construire la Matrice des Cas tests. (temps d'execution total ~ 1 h 50 mn)				#
#																	#
#########################################################################################################################################



#########################################################################################################################################
# Aide
AIDE()
{
   echo ""
   echo "Usage :	Matrice.sh [-no_verbose] [-help] [-test] [-html] [-pdf]"
   echo ""
   echo "	-no_verbose	: Le programme ne parle pas"
   echo "	-help		: L'aide est affichee"
   echo "	-test		: Le programme ne lance pas les cas tests modifies"
   echo "			  pour le QC (plus rapide mais moins complet)"
   echo "	-html		: Le programme ne cree que les fichiers html"
   echo "			  (cette option n'est valable que si vous disposez"
   echo "			  des fichiers \"Cas_valid\" et/ou \"Cas_test\" et/ou"
   echo "			  \"Cas_test_modifie_QC_OK\" issus d'une precedante"
   echo "			  execution du programme)"
   echo "	-pdf		: Le programme ne cree que le fichier pdf si le"
   echo "			  fichier \"Matrice.html\" existe."
   echo ""
}



#########################################################################################################################################
# Si l'environnement TRUST n'est pas charge, je stop
ENVIRONNEMENT_TRUST()
{
   if [ "$TRUST_ROOT" = "" ]
   then
      echo ""
      echo "L'environnement TRUST n'est pas charge !"
      echo ""
      exit
   else
      . $TRUST_ROOT/Outils/TRIOXDATA/IHMTRIO.sh
      executable=`echo $exec | awk -F"/" '{print $NF}'`
      export PYTHONPATH=`dirname $0`:$PYTHONPATH
      REP_VALID=$TRUST_SHARE/Jeu_de_donnees_Validation
      HTML2PS=$TRUST_ROOT/doc/Validation/html2ps/html2ps
      [ ! -s $HTML2PS ] && HTML2PS=none
      PS2PDF=`whereis ps2pdf | awk '{print $2}'`
      i=1
      while [ "$PS2PDF" = "" ]
      do
         case $i in
            1)	PS2PDF=`whereis ps2pdf12 | awk '{print $2}'`;;
            2)	PS2PDF=`whereis ps2pdf13 | awk '{print $2}'`;;
            3)	PS2PDF=`whereis ps2pdf14 | awk '{print $2}'`;;
            4)	PS2PDF=`whereis ps2pdfpress | awk '{print $2}'`;;
            5)	PS2PDF=`whereis ps2pdfwr | awk '{print $2}'`;;
	    *)	PS2PDF=none;;
         esac
         ((i=i+1))
      done
      echo "Environnement TRUST"
      echo ""
      echo "Executable TRUST     : $executable"
      echo "Executable html2ps    : $HTML2PS"
      echo "Executable ps2pdf     : $PS2PDF"
      echo "Repertoire validation : $REP_VALID"
      echo ""
      echo ""
   fi
}



#########################################################################################################################################
# Temps CPU
CPU_TIME()
{
   time_fin=`echo "\`date +%Y\` \`date +%j\` \`date +%H\` \`date +%M\` \`date +%S\`"`
   # Debut
   annee_debut=`echo $time_debut | awk '{print $1}'`
   jour_debut=`echo $time_debut | awk '{print $2}'`
   heure_debut=`echo $time_debut | awk '{print $3}'`
   minute_debut=`echo $time_debut | awk '{print $4}'`
   seconde_debut=`echo $time_debut | awk '{print $5}'`
   # Fin
   annee_fin=`echo $time_fin | awk '{print $1}'`
   jour_fin=`echo $time_fin | awk '{print $2}'`
   heure_fin=`echo $time_fin | awk '{print $3}'`
   minute_fin=`echo $time_fin | awk '{print $4}'`
   seconde_fin=`echo $time_fin | awk '{print $5}'`
   # Calul
   coeff=`awk "BEGIN {printf (\"%i\",(($annee_fin-$annee_debut)*365*24*3600)+(($jour_fin-$jour_debut)*24*3600))}"`
   heure_total=`awk "BEGIN {printf (\"%i\",($heure_fin*3600+$minute_fin*60+$seconde_fin-$heure_debut*3600-$minute_debut*60-$seconde_debut+$coeff)/3600)}"`
   minute_total=`awk "BEGIN {printf (\"%i\",(($heure_fin*3600+$minute_fin*60+$seconde_fin-$heure_debut*3600-$minute_debut*60-$seconde_debut+$coeff)/60)-$heure_total*60)}"`
   seconde_total=`awk "BEGIN {printf (\"%i\",(($heure_fin*3600+$minute_fin*60+$seconde_fin-$heure_debut*3600-$minute_debut*60-$seconde_debut+$coeff)-$heure_total*3600)-$minute_total*60)}"`
   # Impression
   echo "*********************************************"
   echo "Temps total d'execution = $heure_total h  $minute_total mn  $seconde_total s"
   echo "*********************************************"
   echo ""
}



#########################################################################################################################################
# Je lance une recherche de tous les cas de validation et je fais le trie
CAS_VALID()
{
   if [ -s $REP_VALID ]
   then
      echo "Recherche de tous les cas de validation"
      echo ""
      mkdir $PATH_MATRICE/MATRICE_TMP
      echo "Creation du fichier Cas_valid"
      for file in `ls $REP_VALID`
      do
         cp  $REP_VALID/$file $PATH_MATRICE/MATRICE_TMP/.
	 sed -e "s?[rR][eE][sS][oO][uU][dD][rR][eE]_[qQ][cC][oO][mM][pP]?Resoudre?g" \
	     -e "s?[pP][rR][aA][nN][dD][tT][lL]_[sS][oO][uU][sS]_[mM][aA][iI][lL][lL][eE]?Prandtl?g" \
	     -e "s?[lL][oO][nN][gG][uU][eE][uU][rR] [0-9].*??g" \
	     $PATH_MATRICE/MATRICE_TMP/$file > $PATH_MATRICE/MATRICE_TMP/$file.tmp
	 mv -f $PATH_MATRICE/MATRICE_TMP/$file.tmp $PATH_MATRICE/MATRICE_TMP/$file
      done
      cd $PATH_MATRICE/MATRICE_TMP
      chmod u+w *
      cp -f $TRUST_ROOT/doc/Validation/classe_test.py .
      chmod u+x classe_test.py
      # on analyse les cas tests
      time python `dirname $0`/classe_test.py `find . -name '*'.data -exec basename {} .data \;`  >& err
      mv -f res $PATH_MATRICE/Cas_valid
      cd $PATH_MATRICE
      rm -rf $PATH_MATRICE/MATRICE_TMP
      echo ""
   fi
}



#########################################################################################################################################
# Je lance une recherche de tous les cas tests de non-regression et je fais le trie
CAS_TEST()
{
   echo "Recherche de tous les cas tests"
   echo ""
   echo "cherche.ksh dimension"
   cherche.ksh dimension

   if [ -s liste_cherche ]
   then
      mkdir $PATH_MATRICE/MATRICE_TMP
      echo ""
      echo "Creation du fichier Cas_test"
      for file in `cat liste_cherche`
      do
         cp  $TRUST_ROOT/tests/*/$file/$file.data $PATH_MATRICE/MATRICE_TMP/.
      done
      cd $PATH_MATRICE/MATRICE_TMP
      chmod u+w *
      cp -f $TRUST_ROOT/doc/Validation/classe_test.py .
      chmod u+x classe_test.py
      # on analyse les cas tests
      time python `dirname $0`/classe_test.py `find . -name '*'.data -exec basename {} .data \;`  >& err
      mv -f res $PATH_MATRICE/Cas_test
      cd $PATH_MATRICE
      rm -rf $PATH_MATRICE/MATRICE_TMP liste_cherche
   fi
   echo ""
}



#########################################################################################################################################
# Je lance une recherche de tous les cas tests de non-regression et je les transforme en QC s'ils sont compatibles
CAS_TEST_MODIFIE()
{
   CREE_LISTS_KO()
   {
      echo "Creation de la liste des cas KO"
      cat /dev/null > List_pbs_connus
      for mot in Schema_Predictor_Corrector Schema_Crank_Nicholson  Echangeur bidim_axi constituant fluide_ostwald Sch_CN_iteratif Sch_CN_EX_iteratif
      do
         echo "cherche.ksh $mot"
         cherche.ksh $mot
         if [ -s liste_cherche ]
         then
            cat liste_cherche >> List_pbs_connus
         fi
         rm -f out liste_cherche
      done
      for cas in `grep "diffusion_implicite 1" $TRUST_ROOT/tests/*/*/*.data | grep -v seuil | awk -F: '{print $1}'`
      do
         echo `basename $cas .data` >> List_pbs_connus
      done
      
      # Ce cas bloque en parallele
      #echo "Pb_couples" >> List_pbs_connus

      sort -u  List_pbs_connus  > List_pbs_connus.tmp
      mv -f List_pbs_connus.tmp List_pbs_connus
   #   sed "s/QC_//" liste_rate  | grep -v -f List_pbs_connus
      echo ""
   }

   CREE_LISTS()
   {
      CREE_LISTS_KO
      echo "Creation de la liste des cas OK"
      echo "cherche.ksh navier_stokes_standard"
      cherche.ksh navier_stokes_standard
      grep -v -f List_pbs_connus liste_cherche > Liste_lam
      echo "cherche.ksh navier_stokes_turbulent"
      cherche.ksh navier_stokes_turbulent
      grep -v -f List_pbs_connus liste_cherche > Liste_turb
      cat Liste_lam Liste_turb > liste_cherche
      rm -f Liste_lam Liste_turb List_pbs_connus
      echo ""
   }

   echo "Recherche de tous les cas tests modifiables en QC"
   echo ""
   CREE_LISTS
   if [ -s liste_cherche ]
   then
      mkdir $PATH_MATRICE/MATRICE_TMP
      cd $PATH_MATRICE/MATRICE_TMP
      sed "s?#BALISE_MODULE?ksh $TRUST_ROOT/doc/Validation/modif \$NOM.data ; NOM=QC_\${NOM}?" $TRUST_ROOT/bin/lance_test  > $PATH_MATRICE/MATRICE_TMP/lance_test_modif
      chmod +x $PATH_MATRICE/MATRICE_TMP/lance_test_modif
      mv $PATH_MATRICE/liste_cherche .
      echo "Lance_test sur les cas OK"
      export PAR_F=0
      echo liste_cherche | ./lance_test_modif -nomail $exec $PATH_MATRICE/MATRICE_TMP
      echo ""
      rm -rf tests_$executable
      if [ -s .tests_$executable ]
      then
         grep    "OK" .tests_$executable | sed 's?ATTENTION.*??g' | awk '{print $NF}' | grep -v "PAR_" | awk -F"|" '{print $2}' | awk -F"^QC_" '{print $2}' 2>/dev/null > Cas_test_modifie_QC_OK
         grep -v "OK" .tests_$executable | sed 's?ATTENTION.*??g' | awk '{print $NF}' | grep -v "PAR_" | awk -F"|" '{print $2}' | awk -F"^QC_" '{print $2}' 2>/dev/null > Cas_test_modifie_QC_KO
	 [ $verbose = 0 ] && cat .tests_$executable && echo ""
	 for fichier in Cas_test_modifie_QC_OK Cas_test_modifie_QC_KO
	 do
	    if [ -s $fichier ]
	    then
               echo "Creation du fichier $fichier"
	       cp -f $TRUST_ROOT/doc/Validation/modif.py .
               chmod u+x modif.py
               for file in `cat $fichier`
               do
	          cp  $TRUST_ROOT/tests/*/$file/$file.data $PATH_MATRICE/MATRICE_TMP/.
	          chmod u+w $file.data
	          # on modifie les cas tests
	          [ -s $file.data ] && python `dirname $0`/modif.py $file 1>modif.log 2>&1
	          [ -s mod_$file.data ] && rm -f mod_$file.data
	          [ -s QC_$file.data ] && mv -f QC_$file.data $file.data
               done
               cp $TRUST_ROOT/doc/Validation/classe_test.py .
	       chmod u+x classe_test.py
               # on analyse les cas tests
               time python `dirname $0`/classe_test.py `find . -name '*'.data -exec basename {} .data \;`  >& err
               mv -f res $PATH_MATRICE/$fichier
	    fi
	    rm -f *.data err modif.py classe_test.py modif.log
	 done
         cd $PATH_MATRICE
         rm -rf $PATH_MATRICE/MATRICE_TMP liste_cherche
      fi
   fi
   echo ""
}



#########################################################################################################################################
# Modification des fichiers HTML :
MODIF_HTML()
{
   substitution=0 && fichier=$TRUST_ROOT/doc/Validation/.Pb.htm && fichier_res=$pb\_$fluide.html
   [ -s $pb\_$fluide.html ] && fichier=$pb\_$fluide.html && fichier_res=$fichier.tmp && substitution=1
   case $turb$rayo$consti$discret in
      _laminaire_VEF)			CASE=1;;
      _laminaire_rayo_transparent_VEF)	CASE=19;;
      _laminaire_rayo_semi_transp_VEF)	CASE=25;;
      _laminaire_constituant_VEF)	CASE=31;;
      _turb_VEF)			CASE=15;;
      _turb_rayo_transparent_VEF)	CASE=21;;
      _turb_rayo_semi_transp_VEF)	CASE=27;;
      _turb_constituant_VEF)		CASE=33;;
      _k_eps_VEF)			CASE=8;;
      _k_eps_rayo_transparent_VEF)	CASE=20;;
      _k_eps_rayo_semi_transp_VEF)	CASE=26;;
      _k_eps_constituant_VEF)		CASE=32;;
      _laminaire_VDF)			CASE=1;;
      _laminaire_rayo_transparent_VDF)	CASE=4;;
      _laminaire_rayo_semi_transp_VDF)	CASE=5;;
      _laminaire_constituant_VDF)	CASE=6;;
      _turb_VDF)			CASE=15;;
      _turb_rayo_transparent_VDF)	CASE=16;;
      _turb_rayo_semi_transp_VDF)	CASE=17;;
      _turb_constituant_VDF)		CASE=18;;
      _k_eps_VDF)			CASE=8;;
      _k_eps_rayo_transparent_VDF)	CASE=10;;
      _k_eps_rayo_semi_transp_VDF)	CASE=11;;
      _k_eps_constituant_VDF)		CASE=12;;
      *)				CASE=0;;
   esac
   
   if (( CASE != 0 ))
   then
      ligne=`grep -n "CASE $CASE " $fichier | awk -F: '{print $1}'`
      color="red" && (( nb_cas_ok != 0 )) && color="#FFCC00" && [ $extension = "_valid.html" ] && color="lime"
      ((lignecolor=ligne+1))
      if ( (( CASE == 1 )) || (( CASE == 8 )) || (( CASE == 15 )) )
      then
         if [ "$discret" = "VDF" ]
	 then
	    ((ligne=ligne+7))
	    lignecolor=$ligne
	 else
	    ((ligne=ligne+8))
	    lignecolor=$ligne
         fi
      else
         ((ligne=ligne+5))
      fi
      title=`head -$nb4 html/$pb\_$fluide$turb$rayo$consti$discret$extension | tail -1`
      title=`echo "${title%<br>}"` && (( 1 < nb_cas_ok )) && title=`echo "$title..."`
      if [ $extension = "_valid.html" ]
      then
	 sed -e "11 s?TITRE?$pb\_$fluide?" -e "s?TITRE_PB?Probleme ${pb#Pb_} $fluide?g" -e "$lignecolor s?bgcolor=\"red\"?bgcolor=\"$color\"?" -e "$ligne s?>0\.?><a href=\"$pb\_$fluide$turb$rayo$consti$discret$extension\" title=\"$title\">$nb_cas_ok<\/a>\.?" $fichier > $fichier_res
      else
         if [ "$extension" = ".html" ]
         then
	    sed -e "11 s?TITRE?$pb\_$fluide?" -e "s?TITRE_PB?Probleme ${pb#Pb_} $fluide?g" -e "$lignecolor s?bgcolor=\"red\"?bgcolor=\"$color\"?" -e "$ligne s?\.0\.?\.<a href=\"$pb\_$fluide$turb$rayo$consti$discret$extension\" title=\"$title\">$nb_cas_ok<\/a>\.?" $fichier > $fichier_res
	 else
	    if [ $nbl = $nbligne ]
	    then
	       sed -e "11 s?TITRE?$pb\_$fluide?" -e "s?TITRE_PB?Probleme ${pb#Pb_} $fluide?g" -e "$lignecolor s?bgcolor=\"red\"?bgcolor=\"$color\"?" -e "$ligne s?\.0<?\.<a href=\"$pb\_$fluide$turb$rayo$consti$discret$extension\" title=\"$title\">$nb_cas_ok<\/a><?" -e "106 s?<\!\[if \!supportEmptyParas\]>\&nbsp;<\!\[endif\]>?nombre de cas test modifie pour QC (OK \/ Total) = $nb_cas_ok_total \/ $nb_cas_all_total?" $fichier > $fichier_res
	    else
	       sed -e "11 s?TITRE?$pb\_$fluide?" -e "s?TITRE_PB?Probleme ${pb#Pb_} $fluide?g" -e "$lignecolor s?bgcolor=\"red\"?bgcolor=\"$color\"?" -e "$ligne s?\.0<?\.<a href=\"$pb\_$fluide$turb$rayo$consti$discret$extension\" title=\"$title\">$nb_cas_ok<\/a><?" $fichier > $fichier_res
	    fi
         fi
      fi
      (( substitution == 1 )) && mv -f $fichier_res $fichier
   fi
}



#########################################################################################################################################
# Je genere les fichiers html
CREATION_HTML()
{
   echo "Creation des pages HTML"

   if [ ! -s Cas_valid ] && [ ! -s Cas_test ] && [ ! -s Cas_test_modifie_QC_OK ]
   then
      echo "Les fichiers Cas_valid, Cas_test et Cas_test_modifie_QC_OK sont vides ou inexistants"
   fi

   create=0
   for file in Cas_valid Cas_test Cas_test_modifie_QC_OK
   do
      extension=".html" && [ "$file" = "Cas_test_modifie_QC_OK" ] && extension="_modif.html"
      [ "$file" = "Cas_valid" ] && extension="_valid.html"
      if [ -s $file ]
      then
         ((create==0)) && rm -rf $PATH_MATRICE/html $PATH_MATRICE/Matrice.html && mkdir $PATH_MATRICE/html && create=1
         fileKO=`echo "${file%_OK}_KO"`
	 for fluide in QC Incompressible
	 do
	 for pb in Pb_Couple Pb_Simple
         do
            nbligne=`grep "$pb + $fluide +" $file | wc -l | awk '{print $1}'`
	    nbl=1
	    nb_cas_ok_total=0
	    add=0 && [ -s $fileKO ] && add=`grep "$pb +" $fileKO | awk -F":" '{print $2}' |sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"|sed "N;s?\n?+?g"`
	    nb_cas_all_total=`awk "BEGIN {print $add}"`
	    while (( nbl <= nbligne ))
	    do
	       turb=_laminaire && [ `grep "$pb + $fluide +" $file | head -$nbl | tail -1 | grep "+ turb" | wc -l` != 0 ] && turb=_turb && [ `grep "$pb + $fluide +" $file | head -$nbl | tail -1 | grep "+ turb K_eps" | wc -l` != 0 ] && turb=_k_eps
	       rayo=_ && [ `grep "$pb + $fluide +" $file | head -$nbl | tail -1 | grep "+ rayo_" | wc -l` != 0 ] && rayo=_rayo_transparent_ && [ `grep "$pb + $fluide +" $file | head -$nbl | tail -1 | grep "+ rayo_semi_transp" | wc -l` != 0 ] && rayo=_rayo_semi_transp_
	       consti="" && [ `grep "$pb + $fluide +" $file | head -$nbl | tail -1 | grep "+ constituant" | wc -l` != 0 ] && consti=constituant_
	       discret=unknown_discr && [ `grep "$pb + $fluide +" $file | head -$nbl | tail -1 | grep "+ VDF" | wc -l` != 0 ] && discret=VDF; [ `grep "$pb + $fluide +" $file | head -$nbl | tail -1 | grep "+ VEF" | wc -l` != 0 ] && discret=VEF
	       nb_cas_ok=`grep "$pb + $fluide +" $file | head -$nbl | tail -1 | awk -F" : " '{print $2}'`
	       if [ "$file" = "Cas_test_modifie_QC_OK" ]
	       then
	     	  TITRE=" modifie pour QC"; txthtm=$TRUST_ROOT/doc/Validation/.txt_modifQC.htm; sup="QC_"
	     	  nb1=123; nb2=113; nb3=130; nb4=114
	     	  letype=`grep "$pb + $fluide +" $file | head -$nbl | tail -1 | awk -F" : " '{print $1}'`
	     	  nb_cas_ko=`grep "$letype" $fileKO 2>/dev/null | awk -F" : " '{print $2}'` && [ "$nb_cas_ko" = "" ] && nb_cas_ko=0
	     	  ((nb_cas_all=nb_cas_ok+nb_cas_ko))
	     	  ((nb_cas_ok_total=nb_cas_ok_total+nb_cas_ok))
	     	  ((nb_cas_all_total=nb_cas_all_total+nb_cas_ok))
	       else
	     	  TITRE=" $fluide"; txthtm=$TRUST_ROOT/doc/Validation/.txt.htm; sup=""; nb_cas_all=""
	     	  nb1=119; nb2=109; nb3=126; nb4=110
	       fi
	       texte=`grep "$pb + $fluide +" $file | head -$nbl | tail -1 | awk -F"[" '{print $2}' | sed 's/]//g' | sed "s/'/<br>/g" | sed "s/, /\n/g" | sed "s/^<br>/$sup/g"`
	       a_ecrire=`sed -e "11 s?FICHIER_TXT?$pb\_$fluide$turb$rayo$consti$discret${extension%.html}?" -e "12 s?TITRE?$pb$turb$rayo$consti$discret$TITRE?" -e "99 s?TITRE?$pb$turb$rayo$consti$discret<br>$TITRE?" -e "106 s?nb_cas_ok?$nb_cas_ok?" -e "106 s?nb_cas_all?$nb_cas_all?" -e "$nb1 s?FICHIER_BACK?$pb\_$fluide.html?" $txthtm`
	       echo "$a_ecrire" | head -$nb2 > $PATH_MATRICE/html/$pb\_$fluide$turb$rayo$consti$discret$extension
	       echo "$texte" >> $PATH_MATRICE/html/$pb\_$fluide$turb$rayo$consti$discret$extension
	       echo "$a_ecrire" | head -$nb3 | tail -16 >> $PATH_MATRICE/html/$pb\_$fluide$turb$rayo$consti$discret$extension
	       MODIF_HTML
	       ((nbl=nbl+1))
	    done
         done
	 done
      fi
   done

# Je range les fichier :
   for fluide in QC Incompressible
   do
   for pb in Pb_Couple Pb_Simple
   do
      substitution=0 && fichier=$TRUST_ROOT/doc/Validation/.index.htm && fichier_res=Matrice.html && [ -s Matrice.html ] && fichier=Matrice.html && fichier_res=$fichier.tmp && substitution=1
      color="red"
      if [ -s $pb\_$fluide.html ]
      then
         color="#FFCC00";
         case $pb\_$fluide in
            Pb_Couple_Incompressible)	CASE=1;texte="couple";texte_remplace="Probleme couple fluide incompressible + conduction";;
	    Pb_Simple_Incompressible)	CASE=3;texte="simple";texte_remplace="Probleme simple fluide incompressible";;
	    Pb_Couple_QC)		CASE=2;texte="couple";texte_remplace="Probleme couple fluide quasi-compressible + conduction";;
	    Pb_Simple_QC)		CASE=5;texte="simple";texte_remplace="Probleme simple fluide quasi-compressible";;
	    *)				CASE=0;;
         esac
	 
	 if (( CASE != 0 ))
	 then
            ligne=`grep -n "CASE $CASE " $fichier | awk -F: '{print $1}'`
	    ((ligne1=ligne+1))
	    ((ligne2=ligne+6))
	    ((ligne3=ligne+8))
	    sed -e "$ligne1 s?bgcolor=\"red\"?bgcolor=\"$color\"?" \
	        -e "$ligne2 s?Probleme?<a href=\"html\/$pb\_$fluide.html\" title=\"$texte_remplace\">Probleme<\/a>?" \
	        -e "$ligne3 s?$texte?<a href=\"html\/$pb\_$fluide.html\" title=\"$texte_remplace\">$texte<\/a>?" $fichier > $fichier_res
	 else
	    echo "Erreur : le fichier $pb\_$fluide.html ne peut pas etre traite !"
	    exit
	 fi
         mv -f $pb\_$fluide.html $PATH_MATRICE/html/.
         (( substitution == 1 )) && mv -f $fichier_res $fichier
      fi
   done
   done
   echo ""
}



#########################################################################################################################################
# Je genere le fichier pdf
CREATION_PDF()
{
   echo "Creation du fichier PDF"
   if [ $PS2PDF != "none" ] && [ $HTML2PS != "none" ]
   then
      if [ -s Matrice.html ]
      then
         $HTML2PS -f ${HTML2PS%html2ps}sample -U -u -W a -o Matrice.ps Matrice.html
         $PS2PDF Matrice.ps Matrice.pdf
         rm -f Matrice.ps html2ps.log
      else
         echo "Le fichier Matrice.html est vide ou inexistant"
      fi
   else
      echo "Le convertisseur html2ps et/ou ps2pdf n'est pas installe sur votre machine"
   fi
   echo ""
}



#########################################################################################################################################
# Programme principal :
MAIN()
{
   # Cas de validation
   CAS_VALID
   # Cas test de non regression
   CAS_TEST
   # Cas test de non regression modifie pour QC
   [ $test = 0 ] && CAS_TEST_MODIFIE
   # Generation des pages HTML
   CREATION_HTML
   # Generation du fichier PDF
   CREATION_PDF
   # Calcul temps CPU
   CPU_TIME
}



#########################################################################################################################################
# Environnement
time_debut=`echo "\`date +%Y\` \`date +%j\` \`date +%H\` \`date +%M\` \`date +%S\`"`
PATH_MATRICE=`pwd`

# default :
nb_argument=$#
nb_arg=0
verbose=1
test=0
html=0
pdf=0

while (( nb_arg < nb_argument ))
do
   [ ${#1} != 0 ] && arg=$1 && shift
   case $arg in
      -no_verbose)	verbose=0;;
      -help)		AIDE && exit ;;
      -test)		test=1;;
      -html)		html=1;;
      -pdf)		pdf=1;;
      *)		echo ""
      			echo "Argument $arg non compris"
			AIDE && exit ;;
   esac
   ((nb_arg=nb_arg+1))
done

if [ $verbose = 1 ]
then
   ENVIRONNEMENT_TRUST
   [ $html = 1 ] && CREATION_HTML && [ $pdf = 0 ] && CPU_TIME && exit
   [ $pdf = 1 ] && CREATION_PDF && CPU_TIME && exit
   MAIN
else
   ENVIRONNEMENT_TRUST 1>Matrice.log 2>&1
   [ $html = 1 ] && CREATION_HTML 1>>Matrice.log 2>&1 && [ $pdf = 0 ] && CPU_TIME 1>>Matrice.log 2>&1 && exit
   [ $pdf = 1 ] && CREATION_PDF 1>>Matrice.log 2>&1 && CPU_TIME 1>>Matrice.log 2>&1 && exit
   MAIN 1>>Matrice.log 2>&1
fi

# Fin
