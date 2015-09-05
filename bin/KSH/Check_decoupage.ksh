#!/bin/bash
# Check_decoupage
# Prend un cas et le decoupe jusqu'a plantage ou nprocs procs maximum
warning()
{
   echo "=================
Failure on $cas
=================" >> $big_log
   tail -10 $log >> $big_log 
}
send_and_exit()
{
   if [ -s $big_log ]
   then
      echo "Sending log..."
      cat $big_log
   fi
   rm -f $big_log
   exit $1
}
change_timestep()
{
   echo $ECHO_OPTS "1,$ s?nb_pas_dt_max [0-9]*?nb_pas_dt_max 3?g\nw" | ed $1 1>/dev/null 2>&1
   if [ $? != 0 ]
   then
      echo $ECHO_OPTS "1,$ s?tinit ?nb_pas_dt_max 3 tinit ?g\nw" | ed $1 1>/dev/null 2>&1
      if [ $? != 0 ]
      then
         echo $ECHO_OPTS "1,$ s?tmax ?nb_pas_dt_max 3 tmax ?g\nw" | ed $1 1>/dev/null 2>&1
      else
         echo "Impossible d'appliquer change_timestep sur $1."
	 send_and_exit -1
      fi
   fi
}
analyse()
{
   jdd=$1
   elem=`$TRUST_Awk '/Total number of elements/ {print $NF}' $jdd".err"`
   AXB=`$TRUST_Awk '/clock Ax=B/ && /solveur_pression/ {n++;if (n==1) first=$3;else total+=$3} END {printf(" A=LU %5.3g s x=A-1.B %5.3g s",first,total/(n-1))}' $jdd".out"`
   OK=$OK" "`$TRUST_Awk -v n=$NB_PROCS -v elem=$elem -v AXB="$AXB" '	/Total number of elements/ {elem=$NF} \
   									/Secondes/ && /pas de temps/ {cpu_per_timestep=$NF} \
                           						END {printf("%s s/dt %5.3g %d cells/procs",AXB,cpu_per_timestep,int(elem/n))}' $jdd.TU`
}

big_log=`mktemp_`
[ ${#1} = 0 ] && echo "Usage: $0 nom_du_cas | -all [-cpu] [-np_max integer] [-quick] [-update]" && send_and_exit -1

#echo `basename $0`" "$*
DEST=$TRUST_TMP/Check_decoupage
mkdir -p $DEST
log=$DEST/`basename $0`.log
# Verifie que $exec existe et fonctionne
[ ! -f $exec ] && echo "$exec in not existing" && send_and_exit -1
 
# Cas ou l'on passe tous les cas tests
if [ "$1" = -all ] || [ "$1" = -all_failed ]
then
   rm -f $log
   cd $TRUST_TESTS
   echo "Mise en place de la procedure..."
   if [ "$1" = -all ]
   then
      cherche.ksh PARALLEL@OK 1>/dev/null 2>&1
      reps=`cat liste_cherche`
   fi
   if [ "$1" = -all_failed ]
   then
      reps=`\ls $DEST | grep -v .log`
   fi
   shift
   for rep in $reps
   do
      run=1
      # On ne teste pas les cas "# PARALLEL OK #" sauf si update
      [ "`echo $* | grep update`" = "" ] && [ "`grep '# PARALLEL OK #'  */$rep/$rep.data`" != "" ] && run=0
      #
      if [ $run = 1 ]
      then
         cd */$rep
         Check_decoupage.ksh $rep $*
         cd $TRUST_TESTS
      fi
   done
   echo "Fin du test a `date '+%H:%M:%S'`. Compte rendu complet dans $log et les fichiers .err et .out des calculs." | tee -a $log
   send_and_exit 0
fi
cas=${1%.data}
shift
[ ! -f $cas.data ] && echo "Cas $cas.data inexistant." && send_and_exit -1

update=0
quick=0
cpu=0
while [ "$1" != "" ]
do
   [ "$1" = -cpu ] && cpu=1
   [ "$1" = -update ] && update=1
   [ "$1" = -quick ] && quick=1
   [ "$1" = -np_max ] && shift && MAX_NB_PROCS=$1
   shift
done

# On copie le cas test
#if [ "`ls $TRUST_TESTS/*/$cas/$cas.lml.gz 2>/dev/null`" != "" ]
if [ "`find $TRUST_TESTS/ -follow -name $cas.lml.gz -print 2>/dev/null | sort`" != "" ]
then
   cd $DEST
   copie_cas_test $cas $TRUST_TESTS 1>copie_cas_test.log 2>&1
   cd $cas
else
   update=1
   rm -r -f $DEST/$cas
   mkdir -p $DEST/$cas
   cp * $DEST/$cas/.
   cd $DEST/$cas
   chmod +w $DEST/$cas/*
fi

# On prend le nombre max de processeurs dans le .data si on ne l'update pas:
if [ "$MAX_NB_PROCS" = "" ]
then
   if [ $update = 1 ]
   then
      MAX_NB_PROCS=8
   else 
      MAX_NB_PROCS=`$TRUST_Awk '/# PARALLEL OK/ && (NF==5) {max=$4} END {print max}' $cas.data`  
      if [ "$MAX_NB_PROCS" = "" ]
      then
	 echo "Le cas $cas n'est pas teste. Pour forcer le test, taper:"
	 echo "`basename $0` $cas -update"
	 send_and_exit -1
      fi
   fi
fi

if [ $quick = 0 ]
then
   echo "Calcul sur le cas $cas pour verifier le parallelisme jusqu'a $MAX_NB_PROCS processeurs:" | tee -a $log
else
   echo "Calcul limite a 2 pas de temps sur le cas $cas pour verifier le decoupage jusqu'a $MAX_NB_PROCS processeurs:" | tee -a $log 
fi
err=0
export NB_PROCS=0
while [ $err = 0 ] && [ $NB_PROCS != $MAX_NB_PROCS ]
do 
   let NB_PROCS=$NB_PROCS+1
   # On prepare
   if [ -f prepare ]
   then
      cat prepare | $TRUST_Awk -v n=$NB_PROCS '// {ch=$0;if ($1=="make_PAR.data" || $1=="trust" || $1=="triou" && NF==3) ch=$1" "$2" "n;print ch}' > PAR_prepare
      chmod +x PAR_prepare
      ./PAR_prepare 1>PAR_prepare.log 2>&1
   fi
   if [ $NB_PROCS = 1 ]
   then
      echo $ECHO_OPTS "$NB_PROCS processeur :\c"
      [ "$cpu" = 0 ] && echo $ECHO_OPTS "                Execution...\c" | tee -a $log 
      [ $quick = 1 ] && change_timestep $cas".data"
      trust $cas 1>$cas".out" 2>$cas".err"
      if [ $? = 0 ]
      then
         if [ "$cpu" = 0 ]
	 then
            OK="OK Validite...OK"
         else
	    OK=""
	    analyse $cas
	 fi
         echo $ECHO_OPTS $OK | tee -a $log 
      else
         echo "Arret Validite...KO" | tee -a $log
	 warning      
      fi 
      [ -f $cas*lml ] && mv $cas*lml ref.lml   
   else
      echo $ECHO_OPTS "$NB_PROCS processeurs:\c"
      [ "$cpu" = 0 ] && echo $ECHO_OPTS " Decoupage...\c" | tee -a $log 
      # On decoupe en changeant potentiellement le decoupage si on a PARALLEL OK N
      make_PAR.data $cas $NB_PROCS 1>>$log 2>&1
      err=$?
      if [ $err != 0 ]
      then
	 [ "$cpu" = 0 ] && echo $ECHO_OPTS "OK Execution...\c" | tee -a $log 
	 # On change le nombre pas de temps
	 [ $quick = 1 ] && change_timestep PAR_$cas".data"
	 # On lance le calcul en parallele
	 rm -f PAR_$cas*".lml"
	 trust PAR_$cas $NB_PROCS 1>PAR_$cas".out" 2>PAR_$cas".err"
	 err=$?
	 if [ $err = 0 ]
	 then
	    [ "$cpu" = 0 ] && echo $ECHO_OPTS "OK Validite...\c" | tee -a $log 
	    if [ -f ref.lml ] && [ -f PAR_$cas*".lml" ]
	    then
	       compare_lata ref.lml PAR_$cas*".lml" 1>>$log 2>&1
	       err=$?
	    else
	       err=0
	    fi
            if [ $err = 0 ]
            then
	       if [ "$cpu" = 0 ]
	       then
	          OK=OK
	       else
	          OK=""
                  analyse PAR_$cas
	       fi
               echo $OK | tee -a $log 
            else
               echo "Ecarts" | tee -a $log 
	       warning
            fi
	 else
            echo "Arret Validite...KO" | tee -a $log
	    warning
	 fi
      else
	 echo "Erreur" | tee -a $log 
	 warning
      fi
   fi
done
cd ..
if [ $err = 0 ]
then
   rm -r -f $cas
fi
# Mise a jour des jeux de donnees
if [ $update = 1 ] && [ -f $TRUST_TESTS/*/$cas/$cas.data ]
then
   cd $TRUST_TESTS/*/$cas
   [ $err != 0 ] && let NB_PROCS=$NB_PROCS-1
   if [ $NB_PROCS -ge 2 ] && [ "`grep 'PARALLEL OK '$NB_PROCS $cas.data`" = "" ]
   then
      echo "$cas.data modifie..."
      CHECKOUT $cas.data 1>/dev/null 2>&1
      PARALLEL="`grep 'PARALLEL OK' $cas.data`"
      if [ $NB_PROCS = 2 ]
      then
         PARALLEL_NEW="# PARALLEL OK #"
      else
         PARALLEL_NEW="# PARALLEL OK $NB_PROCS #"
      fi
      echo $ECHO_OPTS "1,$ s?$PARALLEL?$PARALLEL_NEW?g\nw" | ed $cas.data 1>/dev/null 2>&1
   fi
fi
send_and_exit
