#!/bin/bash

if [ "`ls -art | grep SAUV-`" == "" ]
then
   echo "Unable to find any directory SAUV-* !"
   exit
else
   i=0
   for SAUV in `ls -art | grep SAUV-`
   do
      if [ -d $SAUV ]
      then
         \cd $SAUV
	 [ "`ls TU.tar.gz 2>/dev/null`" != "" ] && tar xvfz TU.tar.gz 1>/dev/null 2>&1
	 ((i == 0)) && NOM=`ls *.TU 2>/dev/null | grep -v "detail" | head -1` && NOM=${NOM%.TU}
	 [ -f $NOM.TU ] && ((i=i+1))
	 \cd ../
      fi
   done
   if [ "$NOM" != "" ]
   then
      Temps_total_init=0
      Temps_total_calc=0
      Timesteps=0
      Secondes_dt=0
      Nb_solveur=0
      Nb_ass_impl=0
      Secondes_solv=0
      Secondes_ass_impl=0
      It=0
      for SAUV in `ls -art | grep SAUV-`
      do
         if [ -d $SAUV ]
         then
	    if [ "`grep 'Temps total' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Temps total' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Temps_total_init=`$TRUST_Awk "BEGIN{print $Temps_total_init+$X}"`
	    fi
	    if [ "`grep 'Temps total' $SAUV/$NOM.TU | tail -1`" != "" ]
	    then
	       X=`grep 'Temps total' $SAUV/$NOM.TU | tail -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Temps_total_calc=`$TRUST_Awk "BEGIN{print $Temps_total_calc+$X}"`
	    fi
	    if [ "`grep 'Timesteps' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Timesteps' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Timesteps=`$TRUST_Awk "BEGIN{print $Timesteps+$X}"`
	    fi
	    if [ "`grep 'Secondes / pas de temps' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Secondes / pas de temps' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Secondes_dt=`$TRUST_Awk "BEGIN{print $Secondes_dt+$X/$i}"`
	    fi
	    if [ "`grep 'Nb solveur / pas de temps' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Nb solveur / pas de temps' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Nb_solveur=`$TRUST_Awk "BEGIN{print $Nb_solveur+$X/$i}"`
	    fi
	    if [ "`grep 'Nb assemblage implicite' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Nb assemblage implicite' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Nb_ass_impl=`$TRUST_Awk "BEGIN{print $Nb_ass_impl+$X}"`
	    fi
	    if [ "`grep 'Secondes / solveur' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Secondes / solveur' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Secondes_solv=`$TRUST_Awk "BEGIN{print $Secondes_solv+$X/$i}"`
	    fi
	    if [ "`grep 'Secondes / assemblage implicite' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Secondes / assemblage implicite' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       Secondes_ass_impl=`$TRUST_Awk "BEGIN{print $Secondes_ass_impl+$X/$i}"`
	    fi
	    if [ "`grep 'Iterations / solveur' $SAUV/$NOM.TU | head -1`" != "" ]
	    then
	       X=`grep 'Iterations / solveur' $SAUV/$NOM.TU | head -1 | $TRUST_Awk '{print $NF}'`; [ "$X" == "" ] && X=0
	       It=`$TRUST_Awk "BEGIN{print $It+$X/$i}"`
	    fi
         fi
      done
      rm -f $NOM.TU.total
      if [ $Temps_total_init != 0 ]
      then
         echo "Statistiques d'initialisation du calcul Total" >> $NOM.TU.total
         echo "" >> $NOM.TU.total
         echo "Temps total                       $Temps_total_init" >> $NOM.TU.total
         echo "" >> $NOM.TU.total
      fi
      if [ $Temps_total_calc != 0 ]
      then
         echo "Statistiques de resolution du probleme Total" >> $NOM.TU.total
         echo "" >> $NOM.TU.total
         echo "Temps total                       $Temps_total_calc" >> $NOM.TU.total
      fi
      if [ $Timesteps != 0 ]
      then
         echo "Timesteps                         $Timesteps" >> $NOM.TU.total
      fi
      if [ $Secondes_dt != 0 ]
      then
         echo "Secondes / pas de temps           $Secondes_dt" >> $NOM.TU.total
      fi
      if [ $Nb_solveur != 0 ]
      then
         echo "Nb solveur / pas de temps         $Nb_solveur" >> $NOM.TU.total
      fi
      if [ $Nb_ass_impl != 0 ]
      then
         echo "Nb assemblage implicite           $Nb_ass_impl" >> $NOM.TU.total
      fi
      if [ $Secondes_solv != 0 ]
      then
         echo "Secondes / solveur                $Secondes_solv" >> $NOM.TU.total
      fi
      if [ $Secondes_ass_impl != 0 ]
      then
         echo "Secondes / assemblage implicite   $Secondes_ass_impl" >> $NOM.TU.total
      fi
      if [ $It != 0 ]
      then
         echo "Iterations / solveur              $It" >> $NOM.TU.total
      fi
      if [ -s $NOM.TU.total ]
      then
         echo "" >> $NOM.TU.total
         echo "Timesteps = nombre de pas de temps" >> $NOM.TU.total
         echo "Nb solveur = nombre de resolutions de systeme lineaire" >> $NOM.TU.total
         echo "Nb assemblage implicite = nombre d'assemblage de matrice pour le schema implicite" >> $NOM.TU.total
         echo "Iterations = nombre moyen d'iterations du solveur" >> $NOM.TU.total
         echo "Communications = fraction du temps passe dans les" >> $NOM.TU.total
         echo "                 communications entre processeurs (hors io fichiers)" >> $NOM.TU.total
         echo "Network latency = temps d'un mpsum mesure par un bench interne sur 0.1s" >> $NOM.TU.total
         echo "Network bandwidth = maximum sur l'ensemble des processeurs" >> $NOM.TU.total
         echo "                    de la bande passante moyenne des operations send_recv" >> $NOM.TU.total
         echo "Waiting time = estimation du temps d'attente des differents processeurs" >> $NOM.TU.total
         echo "" >> $NOM.TU.total
         echo "Max_waiting_time grand  => probablement mauvais decoupage" >> $NOM.TU.total
         echo "Communications > 30%    => trop de processeurs ou reseau trop lent" >> $NOM.TU.total
         echo "File $NOM.TU.total correctly constructed."
      else
         echo "File not constructed."
      fi
   fi
fi
